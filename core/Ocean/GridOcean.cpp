#include <GridOcean.h>
#include <BreakingWave.h>
#include <Eigen/Dense>

#include <vector>

using namespace Eigen;
using namespace std;

/****************************************************************************/
/****************************************************************************/
GridOcean::GridOcean():Grid()
{
	init();
	this->m_t = 0;
}
/****************************************************************************/
GridOcean::GridOcean(Vector3f center, Vector3f min, Vector3f max, float dx, float dz) : Grid(center, min, max, dx, 0, dz)
{
	init();
	this->m_t = 0;
}
/****************************************************************************/
GridOcean::~GridOcean()
{
	m_initPos.clear();
	m_initPos.shrink_to_fit();
	//m_initPos.~vector();

	m_vel.clear();
	m_vel.shrink_to_fit();
	//m_vel.~vector();

	m_dVel.clear();
	m_dVel.shrink_to_fit();
	//m_dVel.~vector();
}
/****************************************************************************/
Vector3f GridOcean::getInitPos(int ix, int iz)
{
	assert(ix<m_nx && iz<m_nz);
	int indexCell = ix + iz*m_nx;
	return m_initPos[indexCell];
}
/****************************************************************************/
Vector3f GridOcean::getInitPos(int indexCell)
{
	assert(indexCell<m_n);
	return m_initPos[indexCell];
}
/****************************************************************************/
Vector3f GridOcean::getVel(int ix, int iz)
{
	assert(ix<m_nx && iz<m_nz);
	int indexCell = ix + iz*m_nx;
	return m_vel[indexCell];
}
/****************************************************************************/
Vector3f GridOcean::getVel(int index)
{
	assert(index<m_n);
	return m_vel[index];
}
/****************************************************************************/
Vector3f GridOcean::getDVel(int ix, int iz)
{
	assert(ix<m_nx && iz<m_nz);
	int indexCell = ix + iz*m_nx;
	return m_dVel[indexCell];
}
/****************************************************************************/
Vector3f GridOcean::getDVel(int index)
{
	assert(index<m_n);
	return m_dVel[index];
}
/****************************************************************************/
float GridOcean::getT()
{
	return m_t;
}
/****************************************************************************/
/****************************************************************************/
void GridOcean::setInitPos(int ix, int iz, Vector3f pos)
{
	int indexCell = ix + iz*m_nx;
	m_initPos[indexCell] = pos;
}
/****************************************************************************/
void GridOcean::setVel(int ix, int iz, Vector3f vel)
{
	int indexCell = ix + iz*m_nx;
	m_vel[indexCell] = vel;
}
/****************************************************************************/
void GridOcean::setDVel(int ix, int iz, Vector3f dVel)
{
	int indexCell = ix + iz*m_nx;
	m_dVel[indexCell] = dVel;
}
/****************************************************************************/
void GridOcean::setT(float t)
{
	this->m_t = t;
}
/****************************************************************************/
// Version Cuda : A faire directement depuis le GPU
/****************************************************************************/
void GridOcean::init()
{
	//printf("INIT GRID OCEAN");
	m_initPos.clear();
	m_vel.clear();
	m_dVel.clear();

	for(unsigned int i=0;i<m_pos.size();i++){
		Vector3f pos = m_pos[i];
		m_initPos.push_back(pos);
		m_vel.push_back(Vector3f(0,0,0));
		m_dVel.push_back(Vector3f(0,0,0));
	}
}
/****************************************************************************/
// Version Cuda : A faire directement depuis le GPU
/****************************************************************************/
void GridOcean::reinitPos()
{
	//printf("INIT GRID OCEAN");
	m_pos.clear();
	m_vel.clear();
	m_dVel.clear();

	for(unsigned int i=0;i<m_pos.size();i++){
		m_pos.push_back(m_initPos[i]);
		m_vel.push_back(Vector3f(0,0,0));
		m_dVel.push_back(Vector3f(0,0,0));
	}
	m_t = 0;
}
/****************************************************************************/
/****************************************************************************/
void GridOcean::update(std::vector<WaveGroup*> waveGroups, float dt)
{
	for(int i=0; i<m_n; i++){
		m_pos[i] = m_initPos[i];
		m_vel[i]=Vector3f(0,0,0);
		m_dVel[i]=Vector3f(0,0,0);
		for(unsigned int n=0; n<waveGroups.size(); n++)
		{
			  Vector3f dPos(0,0,0);
			  Vector3f vel(0,0,0);
			  Vector3f dVel(0,0,0);
			  waveGroups[n]->computeMovement(m_initPos[i],m_t,&dPos,&vel,&dVel);
			  dPos[0]*=waveGroups[n]->getCosTheta(); dPos[2]*=waveGroups[n]->getSinTheta(); 
			  vel[0]*=waveGroups[n]->getCosTheta(); vel[2]*=waveGroups[n]->getSinTheta();
			  dVel[0]*=waveGroups[n]->getCosTheta(); dVel[2]*=waveGroups[n]->getSinTheta();
			  m_pos[i] += dPos;
			  m_vel[i] += vel;
			  m_dVel[i] += dVel;
		}
	}
	m_t += dt;
}
/****************************************************************************/
/****************************************************************************/
void GridOcean::generateBreakingWaves(vector<WaveGroup*> waveGroups, vector<BreakingWave*> *breakingWaves, float dt)
{
	for(int ix=0; ix<m_nx; ix++){
		 for(int iz=0; iz<m_nz; iz++){
			int indexCell = ix + iz*m_nx;
			bool toCreate = false;
			vector<WaveGroup*> wgActing;
			vector<bool> wgActive;
			for(unsigned int n=0; n<waveGroups.size(); n++)
			{
				WaveGroup *wg = waveGroups[n];
				// On regarde si on est dans une zone de déferlement -> On regarde parmi les déferlements si une grille contient le point
				bool trouve = false;
				unsigned int nB = 0;
				while(!trouve && nB<breakingWaves->size()){
					GridBreaking* gB = breakingWaves->at(nB)->getGridBreaking();
					Vector3f min = gB->getMin(); Vector3f max = gB->getMax();
					Vector3f local = gB->getLocalRotated(m_pos[indexCell]);
					if(local[0]>=min[0] && local[2]>=min[2] 
					   && local[0]<=max[0] && local[2]<=max[2])
						trouve = true;
					nB++;
				}
				// On teste si on doit créer ou non une zone de déferlement
				float norm = m_vel[indexCell][0]*wg->getPs()*wg->getCosTheta()+m_vel[indexCell][2]*wg->getPs()*wg->getSinTheta();
				if(!trouve && norm>=wg->getPs()*wg->getPs())
					toCreate = true;
			}
			if(toCreate){
				for(unsigned int n=0; n<waveGroups.size(); n++){
					WaveGroup *wg = waveGroups[n];
					// On regarde si le groupe de vague agit localement dans la zone de déferlement
					// Et on regarde si ce groupe est actif ou non dans le déferlement (donne de l'énergie au déferlement)
					float x = m_pos[indexCell][0];
					float z = m_pos[indexCell][2];
					Vector3f MG = Vector3f(x,0,z) - wg->getG();
					float MG2 = MG[0]*MG[0]+MG[2]*MG[2];
					float L = 2*(wg->getLambda()*wg->getN())*(wg->getLambda()*wg->getN());
					if(MG2<=L){
						wgActing.push_back(wg);
						float norm = m_vel[indexCell][0]*wg->getPs()*wg->getCosTheta()+m_vel[indexCell][2]*wg->getPs()*wg->getSinTheta();
						if(norm>=wg->getPs()*wg->getPs())
							wgActive.push_back(true);
						else 
							wgActive.push_back(false);
					}
				}
				BreakingWave* br = new BreakingWave(m_initPos[indexCell],wgActing,wgActive,this->m_t-dt);
				// Mise à jour des points et vel pour regarder les points actifs
				br->update(dt);
				br->getGridBreaking()->setT(this->m_t-dt);
				// On stock la vague déferlante
				breakingWaves->push_back(br);
				
				// Note: la mise à jour front actif / front plus actif est faite dans la méthode update de l'objet Breaking Wave
			}
		}
	}
}
/****************************************************************************/
/****************************************************************************/
