#include <BreakingWave.h>
#include <iostream>
/****************************************************************************/
/****************************************************************************/
BreakingWave::BreakingWave(Vector3f center, vector<WaveGroup*> wg_acting, vector<bool> wg_active, float t)
{
	this->center = center;
	for(int i=0;i<wg_acting.size();i++){
		this->wg_acting.push_back(wg_acting[i]);
		this->wg_active.push_back(wg_active[i]);
	}
	gridBreaking = new GridBreaking(this->center,this->wg_acting,this->wg_active);
	
	Vector3f centerSPH = this->center;
	centerSPH[0] = centerSPH[0] + (gridBreaking->getMax()[0]-gridBreaking->getMin()[0])/4;
	solver = new WCSPH(centerSPH,gridBreaking->getMin(),gridBreaking->getMax());
	solver->getGrid()->setColor(gridBreaking->getColor());	

	gridBreaking->setT(t);

	Vector3f min = gridBreaking->getMin(); min[1]=0;
	Vector3f max = gridBreaking->getMax(); max[1]=0;
	gridBreaking->setMin(min);gridBreaking->setMax(max);

	isActive = true;
	print();
}
/****************************************************************************/
BreakingWave::~BreakingWave()
{
	wg_acting.clear();
	wg_acting.shrink_to_fit();
	wg_acting.~vector();

	wg_active.clear();
	wg_active.shrink_to_fit();
	wg_active.~vector();

	delete(solver);
	isActive = false;
	delete gridBreaking;
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
	assert(index<wg_active.size());
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
	while(i<wg_acting.size()){
		WaveGroup *wg1 = wg_acting[i];
		if(wg->getN()==wg1->getN() && wg->getK()==wg1->getK() && wg->getTheta()==wg1->getTheta()
		&& wg->getR()==wg1->getR() && wg->getPhi0()==wg1->getPhi0() && wg->getZeta()==wg1->getZeta())
			return i;
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
	assert(index<wg_active.size());
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
		while(n<wg_active.size()){
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
	int indexWg = gridBreaking->getIndexMaxLambda();
	WaveGroup* wg = wg_acting[indexWg];
	Vector2f ps = Vector2f(wg->getPs()*wg->getCosTheta(), wg->getPs()*wg->getSinTheta());
	float S = gridBreaking->getDx()*gridBreaking->getDz();
	
	for(int i=0; i< gridBreaking->getNbActivePts(); i++){
		int index = gridBreaking->getActivePt(i);
		Vector3f pos = gridBreaking->getPos(index);
		Vector3f vel = gridBreaking->getVel(index);
		Vector3f dVel = gridBreaking->getDVel(index);
		float mass = solver->getRho0()*S*hypot(vel[0]-ps[0],vel[2]-ps[1]);
		solver->generateParticle(pos,vel,dVel,mass);
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
void BreakingWave::update(float dt)
{
	if(gridBreaking) 
		gridBreaking->update(wg_acting,dt);
	if(solver)
		solver->update(wg_acting,gridBreaking->getT(),dt);

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
	/*printf("**** LISTE DES GROUPES AGISSANT ***\n");
	for(int i=0;i<wg_acting.size();i++){
		if(wg_active[i]) printf("ACTIF params: ");
		else printf("NON ACTIF params: ");
		wg_acting[i]->print();
	}*/
}	
/****************************************************************************/





