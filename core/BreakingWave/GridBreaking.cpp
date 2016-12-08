#include <utils.h>
#include <GridBreaking.h>
#include <Grid.h>
#include <GL/gl.h> 
#include <math.h> 
#include <iostream>
#include <noise.h>
using namespace Eigen;

/****************************************************************************/
/****************************************************************************/
GridBreaking::GridBreaking(Vector3f center, vector<WaveGroup*> wg, vector<bool> wg_active, float tx, float tz)
{
	m_center = center;
	angleRotation = 0;
	this->tx = tx;
	this->tz = tz;
	createGrid(wg, wg_active);
	init();
	this->m_t = 0;
	color = Vector4f(rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,1);
}
/****************************************************************************/
GridBreaking::~GridBreaking()
{
	activePts.clear();
	vector<int>().swap(activePts);
	activePts.shrink_to_fit();

	m_initPos.clear();
	vector<Vector3f>().swap(m_initPos);
	m_initPos.shrink_to_fit();

	m_vel.clear();
	vector<Vector3f>().swap(m_vel);
	m_vel.shrink_to_fit();
}
/****************************************************************************/
int GridBreaking::getActivePt(int i)
{
	assert(i<(int)activePts.size());
	return activePts[i];
}
/****************************************************************************/
void GridBreaking::addActivePt(int index)
{
	activePts.push_back(index);
}
/****************************************************************************/
int GridBreaking::getNbActivePts()
{
	return activePts.size();
}
/****************************************************************************/
float GridBreaking::getMaxLambda()
{
	return maxLambda;
}
/****************************************************************************/
float GridBreaking::getMaxExtens()
{
	return maxExtens;
}
/****************************************************************************/
int GridBreaking::getIndexMaxLambda()
{
	return indexMaxLambda;
}
/****************************************************************************/
Vector3f GridBreaking::getMinAct()
{
	return minAct;
}
/****************************************************************************/
Vector3f GridBreaking::getMaxAct()
{
	return maxAct;
}
/****************************************************************************/
Vector3f GridBreaking::getVel(int index)
{
	assert(index < m_n);
	return m_vel[index];
}
/****************************************************************************/
float   GridBreaking::getT()
{
	return m_t;
}
/****************************************************************************/
/****************************************************************************/
void    GridBreaking::setT(float t)
{
	m_t = t;
}
/****************************************************************************/
/****************************************************************************/
void GridBreaking::init()
{
	angleRotation = 0;
	m_initPos.clear();
	m_vel.clear();
	for(int i=0; i<m_n; i++){
		m_initPos.push_back(m_pos[i]);
		m_vel.push_back(Vector3f(0,0,0));
	}
}
/****************************************************************************/
/****************************************************************************/
void GridBreaking::createGrid(vector<WaveGroup*> wg, vector<bool> wg_active)
{
	maxLambda = 0;
	float sumA = 0;
	maxExtens = 0;
	int maxNx,maxNz;
	bool toCreate = false;
	for(unsigned int i=0; i<wg.size();i++){
		if(wg_active[i]){
			float lambda = wg[i]->getK()*2*M_PI;
			if(lambda>=maxLambda){
			 	maxLambda = lambda;
			 	maxNx = wg[i]->getN();
				indexMaxLambda = i;
			}
			float extens = wg[i]->getExtens();
			if(extens>=maxExtens){
				maxExtens = sqrt(extens);
				maxNz = wg[i]->getN();
			}
			toCreate = true;
		}
		sumA += 2*wg[i]->getR();
	}
	if(toCreate){ 
		maxExtens /= 2;
		m_min = Vector3f(m_center[0] - maxLambda/2, m_center[1]-sumA, m_center[2] - maxExtens);
		m_max = Vector3f(m_center[0] + maxLambda/2, m_center[1]+sumA, m_center[2] + maxExtens);
		
		m_dx = powf((maxLambda/2)*sumA*maxExtens,(1/3.0))/10;//*wg[indexMaxLambda]->getN()/tx;
		m_dz = powf((maxLambda/2)*sumA*maxExtens,(1/3.0))/10;//*wg[indexMaxLambda]->getN()/tz;
		m_dy = 0;
		m_center = (m_min+m_max)/2;
		Grid::createGrid();
		init();
	}
}
/****************************************************************************/
/****************************************************************************/
void GridBreaking::checkActiveWgs(vector<WaveGroup*> wgs, vector<bool> *wg_active)
{
	for(unsigned int n=0; n<wgs.size(); n++)
	{
		WaveGroup *wg = wgs[n];
		float lambda = wg->getK()*2*M_PI;
		bool actif = false;
		int i=0;
		while(i<m_nx && !actif){
			int j=0;
			while(j<m_nz && !actif){
				int indexCell = i + j*m_nx;
				float norm = m_vel[indexCell][0]*wg->getPs()*wg->getCosTheta()+m_vel[indexCell][2]*wg->getPs()*wg->getSinTheta(); // vitesse particule projetée sur vitesse du groupe
				if(norm>=wg->getPs()*wg->getPs()){ // la particule va plus vite que la vague
					actif = true;
					if(maxLambda<lambda){
						maxLambda = lambda;
						indexMaxLambda = n;
					}
				}
				j++;
			}
			i++;
		}
		(*wg_active)[n] = actif;
	}
}
/****************************************************************************/
/****************************************************************************/
void GridBreaking::checkActivePts(vector<WaveGroup*> wgs, vector<bool> wg_active)
{
	minAct = Vector3f(INF,m_center[1],INF);
	maxAct = Vector3f(-INF,m_center[1],-INF);

	activePts.clear();
	for(int i=0;i<m_nx;i++){
		for(int j=0;j<m_nz;j++){
			int indexCell = i + j*m_nx;
			Vector3f pos = getLocalRotated(m_pos[indexCell]);
			bool actif = false;
			unsigned int n = 0;
			while(!actif && n<wgs.size()){
				if(wg_active[n]){
					WaveGroup *wg = wgs[n];
					float norm = m_vel[indexCell][0]*wg->getPs()*wg->getCosTheta()+m_vel[indexCell][2]*wg->getPs()*wg->getSinTheta();
					if(norm>=wg->getPs()*wg->getPs()){
						actif = true;
						minAct[0] = fmin(minAct[0],pos[0]);
						minAct[2] = fmin(minAct[2],pos[2]);
	
						maxAct[0] = fmax(maxAct[0],pos[0]);
						maxAct[2] = fmax(maxAct[2],pos[2]);	
					}
				}
				n++;
			}
			if(actif)
				activePts.push_back(indexCell);
		}
	}
}
/****************************************************************************/
/****************************************************************************/
Vector3f GridBreaking::calculScale()
{
	Vector3f scale(1,1,1);
	Vector3f min, max;
	min << INF,m_min[1],INF; max <<-INF,m_max[1],-INF;
	if(activePts.size()>0){
		for(unsigned int i=0;i<activePts.size();i++){
			Vector3f pt = getLocalRotated(m_pos[activePts[i]]);
			min[0] = (float)fmin((double)min[0],(double)pt[0]);
			min[2] = (float)fmin((double)min[2],(double)pt[2]);
			max[0] = (float)fmax((double)max[0],(double)pt[0]);
			max[2] = (float)fmax((double)max[2],(double)pt[2]);
		}
		if(m_max[0]-m_min[0]!=0) scale[0] = (float)fabs((double)((max[0]-min[0])/(m_max[0]-m_min[0])))*2;
		if(m_max[2]-m_min[2]!=0) scale[2] = (float)fabs((double)((max[2]-min[2])/(m_max[2]-m_min[2])))*2;
		
		
		if(scale[0]<maxLambda) scale[0]=1;
		if(scale[2]<maxExtens/1.35) scale[2]=1;
		//printf("scale:%f %f ",scale[0],scale[2]);
	}
	return scale;
}
/****************************************************************************/
/****************************************************************************/
Vector3f GridBreaking::calculTranslation()
{
	barycenter << 0,0,0;
	if(activePts.size()>0){
		for(unsigned int i=0;i<activePts.size();i++){
			Vector3f pt = m_pos[activePts[i]];
			barycenter+=pt;
		}
		barycenter[0]/=activePts.size();
		barycenter[1]=0;
		barycenter[2]/=activePts.size();
		return barycenter - m_center;
	}
	return Vector3f(0,0,0);
}
/****************************************************************************/
/****************************************************************************/
float GridBreaking::calculRotation()
{
	if(activePts.size()>0){
		float maxVel = 0;
		int indexMaxVel = 0;
		for(unsigned int i=0;i<activePts.size();i++){
			Vector3f vel = m_vel[activePts[i]];
			if(vel.norm()>maxVel){
				maxVel = vel.norm();
				indexMaxVel = activePts[i];
			}
		}
		if(m_vel[indexMaxVel].norm()>0){
			Vector3f pt = m_vel[indexMaxVel]/m_vel[indexMaxVel].norm();
			double angle = atan2((double)pt[2],(double)pt[0]);
			indexMax = indexMaxVel;
			double d_angle = angle - angleRotation;
			angleRotation = angle;
			return d_angle;
		}
		return 0;
	}
	return 0;
}
/****************************************************************************/
/****************************************************************************/
void GridBreaking::scale(Vector3f s)
{
	Vector3f omax = m_max - m_center;
	Vector3f omin = m_min - m_center;
	m_min[0] = m_center[0] + omin[0]*s[0]; m_min[2] = m_center[2] + omin[2]*s[2];
	m_max[0] = m_center[0] + omax[0]*s[0]; m_max[2] = m_center[2] + omax[2]*s[2];
	
	for(int ix=0; ix<m_nx; ix++){
		for(int iz=0; iz<m_nz; iz++){
			int indexCell = ix + iz*m_nx;
			Vector3f posAv = this->getLocalRotated(m_initPos[indexCell]) - m_center;
			posAv[0]= m_center[0] + posAv[0]*s[0]; 
			posAv[2]= m_center[2] + posAv[2]*s[2];
			m_initPos[indexCell] = getLocalRotate(posAv);
		}
	}
	m_center = (m_max+m_min)/2;
	m_dx*=s[0]; m_dz*=s[2];
}
/*************************************************/
void GridBreaking::translate(Vector3f t)
{
	m_max += t;
	m_min += t;
	m_center += t;
	for(int ix=0; ix<m_nx; ix++){
		for(int iz=0; iz<m_nz; iz++){
			int indexCell = ix + iz*m_nx;
			m_initPos[indexCell]+=t;
		}
	}
}
/*************************************************/
void GridBreaking::rotate(float theta)
{
	Matrix3f M;
	M << cos(theta), 0, -sin(theta), 0, 1, 0, sin(theta), 0, cos(theta);
	Vector3f c = m_center;

	translate(-c);
	for(int ix=0; ix<m_nx; ix++){
		for(int iz=0; iz<m_nz; iz++){
			int indexCell = ix + iz*m_nx;
			m_initPos[indexCell] = M*m_initPos[indexCell];
		}
	}
	translate(c);
}
/****************************************************************************/
/****************************************************************************/
void GridBreaking::merge(GridBreaking* other)
{
	Grid::merge(other);
	init();
}
/****************************************************************************/
/****************************************************************************/
void GridBreaking::update(vector<WaveGroup*> waveGroups, float dt)
{
	for(int i=0;i<m_nx;i++){
		for(int j=0;j<m_nz;j++){
			int index = i + j*m_nx;
			m_pos[index] = m_initPos[index];	
			m_vel[index]=Vector3f(0,0,0);
			for(unsigned int n=0; n<waveGroups.size(); n++)
			{
			  	Vector3f dPos(0,0,0);
			  	Vector3f vel(0,0,0);
			  	Vector3f dVel(0,0,0);
			  	waveGroups[n]->computeMovement(m_initPos[index],m_t,&dPos,&vel,&dVel);
				// rotation theta autour axe y (vertical)
			  	dPos[2]=dPos[0]; dPos[0]*=waveGroups[n]->getCosTheta(); dPos[2]*=waveGroups[n]->getSinTheta(); 
				vel[2]=vel[0]; vel[0]*=waveGroups[n]->getCosTheta(); vel[2]*=waveGroups[n]->getSinTheta();
				// accumulation linéaire des effets de chaque groupe
			  	m_pos[index] += dPos;
				m_vel[index] += vel;
				// si besoin des normales, voir GridOcean::update()
			}
			if(FBM == 1)
				m_pos[index][1] += SCALE_AMP*perlin_two(m_pos[index][0],m_pos[index][2],GAIN,OCTAVES,tx);
			
		}
	}
	m_t += dt;
}
/****************************************************************************/
/****************************************************************************/
void GridBreaking::display()
{
	Grid::display();
}
/****************************************************************************/
/****************************************************************************/
