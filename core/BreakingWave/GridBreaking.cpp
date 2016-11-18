#include <GridBreaking.h>
#include <GL/gl.h> 
#include <math.h> 
#include <iostream>

/****************************************************************************/
/****************************************************************************/
GridBreaking::GridBreaking(Vector3f center, vector<WaveGroup*> wg, vector<bool> wg_active)
{
	m_center = center;
	angleRotation = 0;
	createGrid(wg, wg_active);
	init();
	this->m_t = 0;
	color = Vector4f(rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,1);
}
/****************************************************************************/
GridBreaking::~GridBreaking()
{
	activePts.clear();
	activePts.shrink_to_fit();
	activePts.~vector();
}
/****************************************************************************/
int GridBreaking::getActivePt(int i)
{
	assert(i<activePts.size());
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
/****************************************************************************/
void GridBreaking::init()
{
	GridOcean::init();
	angleRotation = 0;
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
	for(int i=0; i<wg.size();i++){
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
		m_min << m_center[0] - maxLambda/2, m_center[1]-sumA, m_center[2] - maxExtens;
		m_max << m_center[0] + maxLambda/2, m_center[1]+sumA, m_center[2] + maxExtens;
		m_dx = m_dz = powf((maxLambda/2)*sumA*maxExtens,(1/3.0))/20;
		m_dy = 0;
		m_center = (m_min+m_max)/2;
		GridOcean::createGrid();
		init();
	}
}
/****************************************************************************/
/****************************************************************************/
void GridBreaking::checkActiveWgs(vector<WaveGroup*> wgs, vector<bool> *wg_active)
{
	for(int n=0; n<wgs.size(); n++)
	{
		WaveGroup *wg = wgs[n];
		float lambda = wg->getK()*2*M_PI;
		bool actif = false;
		int i=0;
		while(i<m_nx && !actif){
			int j=0;
			while(j<m_nz && !actif){
				int indexCell = i + j*m_nx;
				float norm = m_vel[indexCell][0]*wg->getPs()*wg->getCosTheta()+m_vel[indexCell][2]*wg->getPs()*wg->getSinTheta();
				if(norm>=wg->getPs()*wg->getPs()){
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
	activePts.clear();
	for(int i=0;i<m_nx;i++){
		for(int j=0;j<m_nz;j++){
			int indexCell = i + j*m_nx;
			bool actif = false;
			int n = 0;
			while(!actif && n<wgs.size()){
				if(wg_active[n]){
					WaveGroup *wg = wgs[n];
					float norm = m_vel[indexCell][0]*wg->getPs()*wg->getCosTheta()+m_vel[indexCell][2]*wg->getPs()*wg->getSinTheta();
					if(norm>=wg->getPs()*wg->getPs())
						actif = true;		
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
		for(int i=0;i<activePts.size();i++){
			Vector3f pt = getLocalRotated(m_pos[activePts[i]]);
			min[0] = fmin(min[0],pt[0]);
			min[2] = fmin(min[2],pt[2]);
			max[0] = fmax(max[0],pt[0]);
			max[2] = fmax(max[2],pt[2]);
		}
		if(m_max[0]-m_min[0]!=0) scale[0] = fabs((max[0]-min[0])/(m_max[0]-m_min[0]))*2;
		if(m_max[2]-m_min[2]!=0) scale[2] = fabs((max[2]-min[2])/(m_max[2]-m_min[2]))*2;
		
		
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
		for(int i=0;i<activePts.size();i++){
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
		for(int i=0;i<activePts.size();i++){
			Vector3f vel = m_vel[activePts[i]];
			if(vel.norm()>maxVel){
				maxVel = vel.norm();
				indexMaxVel = activePts[i];
			}
		}
		if(m_vel[indexMaxVel].norm()>0){
			Vector3f pt = m_vel[indexMaxVel]/m_vel[indexMaxVel].norm();
			double angle = atan2(pt[2],pt[0]);
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
			Vector3f posAv = getLocalRotated(m_initPos[indexCell]) - m_center;
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
	GridOcean::init();
}
/****************************************************************************/
/****************************************************************************/
void GridBreaking::update(vector<WaveGroup*> waveGroups, float dt)
{
	GridOcean::update(waveGroups,dt);
}
/****************************************************************************/
/****************************************************************************/
