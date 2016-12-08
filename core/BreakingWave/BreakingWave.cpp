#include <BreakingWave.h>
#include <iostream>

using namespace Eigen;

/****************************************************************************/
/****************************************************************************/
BreakingWave::BreakingWave(Vector3f center, vector<WaveGroup*> wg_acting, vector<bool> wg_active, float t, float tx, float tz)
{
	this->center = center;
	for(unsigned int i=0;i<wg_acting.size();i++){
		this->wg_acting.push_back(wg_acting[i]);
		this->wg_active.push_back(wg_active[i]);
	}
	gridBreaking = new GridBreaking(this->center,this->wg_acting,this->wg_active, tx, tz);
	
	Vector3f centerSPH = this->center;
	centerSPH[0] = centerSPH[0] + (gridBreaking->getMax()[0]-gridBreaking->getMin()[0])/4;
	Vector3f min = gridBreaking->getMin(); 
	Vector3f max = gridBreaking->getMax(); 
	min[0] = min[0] - (max[0]-min[0])/4;
	max[0] = max[0] + (max[0]-min[0])/4;
	min[2] = min[2] - (max[2]-min[2])/4;
	max[2] = max[2] + (max[2]-min[2])/4;
	solver = new WCSPH(centerSPH, min,max);
	solver->getGrid()->setColor(gridBreaking->getColor());	

	gridBreaking->setT(t);

	min = gridBreaking->getMin(); min[1]=0;
	max = gridBreaking->getMax(); max[1]=0;
	gridBreaking->setMin(min);gridBreaking->setMax(max);

	isActive = true;
	print();
}
/****************************************************************************/
BreakingWave::~BreakingWave()
{
	wg_acting.clear();
	vector<WaveGroup*>().swap(wg_acting);
	wg_acting.shrink_to_fit();
	
	wg_active.clear();
	vector<bool>().swap(wg_active);
	wg_active.shrink_to_fit();
	
	delete solver;

	delete gridBreaking;

	isActive = false;
}
/****************************************************************************/
/****************************************************************************/
GridBreaking* BreakingWave::getGridBreaking()
{
	return gridBreaking;
}
/****************************************************************************/
WaveGroup* BreakingWave::getWgActing(int index)
{
	return wg_acting[index];
}
/****************************************************************************/
int BreakingWave::getNbWgActing()
{
	return wg_acting.size();
}
/****************************************************************************/
bool BreakingWave::getWgActive(int index)
{
	assert(index<(int)wg_active.size());
	return wg_active[index];
}
/****************************************************************************/
SPH* BreakingWave::getSolver()
{
	return solver;
}
/****************************************************************************/
bool BreakingWave::getIsActive()
{
	return isActive;
}
/***************************************************************************/
void BreakingWave::setGridBreaking(GridBreaking* gridBreaking)
{
	this->gridBreaking = gridBreaking;
}
/***************************************************************************/
int BreakingWave::addWgActing(WaveGroup* wg)
{
	int pos = wgActingContains(wg);
	if(pos==-1)
		wg_acting.push_back(wg);
	return pos;
}
/***************************************************************************/
int BreakingWave::wgActingContains(WaveGroup* wg)
{
	int i = 0;
	while(i<(int)wg_acting.size()){
		WaveGroup *wg1 = wg_acting[i];
		if(wg->getN()==wg1->getN() && wg->getK()==wg1->getK() && wg->getTheta()==wg1->getTheta()
		&& wg->getR()==wg1->getR() && wg->getPhi0()==wg1->getPhi0() && wg->getZeta()==wg1->getZeta())
			return (int) i;
		i++;
	}
	return -1;
}	
/***************************************************************************/
void BreakingWave::addWgActive(bool active)
{
	wg_active.push_back(active);
}
/***************************************************************************/	
void BreakingWave::setWgActive(int index, bool active)
{
	assert(index<(int)wg_active.size());
	wg_active[index] = active;
}
/***************************************************************************/
void BreakingWave::setSolver(SPH* solver)
{
	this->solver = solver;
}
/***************************************************************************/
void BreakingWave::setIsActive(bool active)
{
	isActive = active;
}
/****************************************************************************/
/****************************************************************************/
void BreakingWave::checkActivePts()
{
	gridBreaking->checkActivePts(wg_acting,wg_active);
}
/****************************************************************************/
void BreakingWave::checkActiveWgs()
{
	gridBreaking->checkActiveWgs(wg_acting,&wg_active);
}
/****************************************************************************/
bool BreakingWave::checkIfActive()
{	
	if(solver->getNbParticles()>0)
		return true;
	else {
		int n = 0;
		while(n<(int)wg_active.size()){
			if(wg_active[n]==true)
				return true;
			n++;
		}
	}
	return false;
}
/****************************************************************************/
void BreakingWave::generateParticles(float dt)
{
	checkActivePts();
	int indexWg = gridBreaking->getIndexMaxLambda();
	WaveGroup* wg = wg_acting[indexWg];
	Vector2f ps = Vector2f(wg->getPs()*wg->getCosTheta(), wg->getPs()*wg->getSinTheta());
	Vector3f minAct = gridBreaking->getMinAct();
	Vector3f maxAct = gridBreaking->getMaxAct();
	int n = gridBreaking->getNbActivePts();
	if(n>0){
		float S = gridBreaking->getDx()*gridBreaking->getDz();
		for(int i=0; i< gridBreaking->getNbActivePts(); i++){
			int index = gridBreaking->getActivePt(i);
			Vector3f pos = gridBreaking->getPos(index);
			Vector3f vel = gridBreaking->getVel(index);
			
			float mass = solver->getRho0()*S*dt*((float)hypot((double)(vel[0]-ps[0]),(double)(vel[2]-ps[1])));
			if(mass>0)
				solver->generateParticle(pos,vel,mass);
		}
	}
}
/****************************************************************************/
/****************************************************************************/
void BreakingWave::scale()
{
	Vector3f S = gridBreaking->calculScale();
	gridBreaking->scale(S);
	solver->getGrid()->scale(S);
}
/****************************************************************************/
void BreakingWave::resetting()
{
	Vector3f T = gridBreaking->calculTranslation();
	gridBreaking->translate(T);
	solver->getGrid()->translate(T);
}
/****************************************************************************/
void BreakingWave::rotation()
{
	float angle = gridBreaking->calculRotation();
	solver->getGrid()->setAngleRotation(gridBreaking->getAngleRotation());
	gridBreaking->rotate(angle);
	solver->getGrid()->rotate(angle);
}
/****************************************************************************/
void BreakingWave::transform()
{
	scale();
	rotation();
	resetting();
}
/****************************************************************************/
void BreakingWave::merge(BreakingWave *br, float dt)
{
	//printf("FONCTION MERGE !!\n");
	gridBreaking->merge(br->getGridBreaking());
	solver->merge(br->getSolver());
	gridBreaking->rotate(gridBreaking->getAngleRotation());
	solver->getGrid()->setAngleRotation(gridBreaking->getAngleRotation());
	solver->getGrid()->rotate(gridBreaking->getAngleRotation());

	gridBreaking->setT(gridBreaking->getT()-dt);
	gridBreaking->update(wg_acting,dt);

        checkActiveWgs();
	checkActivePts();
	transform();

	gridBreaking->setT(gridBreaking->getT()-dt);
	gridBreaking->update(wg_acting,dt);
}

/****************************************************************************/
/****************************************************************************/
void BreakingWave::update(float dt, GridOcean* ocean)
{
	if(gridBreaking) 
		gridBreaking->update(wg_acting,dt);
	if(solver)
		solver->update(wg_acting,gridBreaking->getT(),dt,ocean);

}
/****************************************************************************/
/****************************************************************************/
void BreakingWave::display()
{
	gridBreaking->display();
	solver->display();
}
/****************************************************************************/
void BreakingWave::print()
{
	printf("**** LISTE DES GROUPES AGISSANT ***\n");
	for(unsigned int i=0;i<wg_acting.size();i++){
		if(wg_active[i]) printf("ACTIF params: ");
		else printf("NON ACTIF params: ");
		wg_acting[i]->print();
	}
}	
/****************************************************************************/





