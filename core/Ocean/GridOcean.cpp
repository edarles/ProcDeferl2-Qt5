
#include <utils.h>

#include <GridOcean.h>
#include <BreakingWave.h>
#include <Eigen/Dense>
#include <QGLWidget>

#include <GL/gl.h>
#include <shaders.h>
#include <noise.h>

#include <vector>
#include <iostream>

using namespace Eigen;
using namespace std;

GLuint GridOcean::m_program1 = 0;
GLuint GridOcean::m_program2 = 0;
GLuint GridOcean::tex1 = 0;
/****************************************************************************/
/****************************************************************************/
GridOcean::GridOcean():Grid()
{
	init();
	this->m_t = 0;
	
	tx = m_max[0]-m_min[0];
	tz = m_max[2]-m_min[2];
	
	initTexture();

	if(m_program1==0) m_program1 = _compileProgram(triangleVertexShader, trianglePixelShaderWithTexture);
	if(m_program2==0) m_program2 = _compileProgram(triangleVertexShader, trianglePixelShaderWithoutTexture);

	sprays = new Sprays();
}
/****************************************************************************/
GridOcean::GridOcean(Vector3f min, Vector3f max, float dx, float dz) : Grid(min, max, dx, 0, dz)
{
	init();
	this->m_t = 0;
	tx = m_max[0]-m_min[0];
	tz = m_max[2]-m_min[2];
	
	initTexture();
	if(m_program1==0) m_program1 = _compileProgram(triangleVertexShader, trianglePixelShaderWithTexture);
	if(m_program2==0) m_program2 = _compileProgram(triangleVertexShader, trianglePixelShaderWithoutTexture);

	sprays = new Sprays();
}
/****************************************************************************/
GridOcean::~GridOcean()
{
	m_initPos.clear();
	vector<Vector3f>().swap(m_initPos);
	m_initPos.shrink_to_fit();

	m_vel.clear();
	vector<Vector3f>().swap(m_vel);
	m_vel.shrink_to_fit();
	
	m_dVel.clear();
	vector<Vector3f>().swap(m_dVel);
	m_dVel.shrink_to_fit();

	free(posDisplay);
	free(colors);
	free(uv);

	glDeleteTextures( 1, &tex1);

	delete sprays;
}
/****************************************************************************/
void GridOcean::initTexture()
{
	QImage img("mousse2.jpg");
	texBubbles = QGLWidget::convertToGLFormat(img);
	for(int i=0; i<texBubbles.width(); i++){
		for(int j=0; j<texBubbles.height(); j++){
			QRgb rgb = texBubbles.pixel(i,j);
			texBubbles.setPixel(i,j,qRgba(qRed(rgb),qGreen(rgb),qBlue(rgb),0));
		}
	}	
	if(tex1==0) glGenTextures(1, &tex1);
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
QImage GridOcean::getTexBubbles()
{
	return texBubbles;
}
/****************************************************************************/
float GridOcean::getLifeTimeBubbles(int index)
{
	assert(index<m_n);
	return lifeTimeBubbles[index];
}
/****************************************************************************/
Sprays* GridOcean::getSprays()
{
	return sprays;
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
void GridOcean::setTexBubbles(QImage tex)
{
	texBubbles = tex;
}
/****************************************************************************/
void GridOcean::setLifeTimeBubbles(int index, float lifeTime)
{
	assert(index < m_n);
	this->lifeTimeBubbles[index] = lifeTime;
}
/****************************************************************************/
// Version Cuda : A faire directement depuis le GPU
/****************************************************************************/
void GridOcean::init()
{
	posDisplay = (float*) malloc(3*(m_nx-1)*(m_nz-1)*6*sizeof(float));
	colors = (float*) malloc(3*(m_nx-1)*(m_nz-1)*6*sizeof(float));
	uv = (float*) malloc(2*(m_nx-1)*(m_nz-1)*6*sizeof(float));

	nb=0;
	m_initPos.clear();
	m_vel.clear();
	m_dVel.clear();
	lifeTimeBubbles.clear();

	for(int i=0; i<m_n;i++){
		Vector3f pos = m_pos[i];
		m_initPos.push_back(pos);
		m_vel.push_back(Vector3f(0,0,0));
		m_dVel.push_back(Vector3f(0,0,0));
		lifeTimeBubbles.push_back(0.0);
	}

	for(int i=0;i<m_nx;i++){
		for(int j=0;j<m_nz;j++){
			int index = i + j*m_nx;
			if(i<m_nx-1 && j<m_nz-1){
				storePolygon(i,j,nb);
				storeColor(nb);
				storeUV(i,j,nb);
				nb+=6;

			}
		}
	}
}
/****************************************************************************/
// Version Cuda : A faire directement depuis le GPU
/****************************************************************************/
void GridOcean::reinitPos()
{
	m_pos.clear();
	m_vel.clear();
	m_dVel.clear();
	lifeTimeBubbles.clear();
	nb = 0;
	for(int i=0;i<m_nx;i++){
		for(int j=0;j<m_nz;j++){
			int index = i + j*m_nx;
			Vector3f pos = m_initPos[index];
			m_pos.push_back(pos);
			m_vel.push_back(Vector3f(0,0,0));
			m_dVel.push_back(Vector3f(0,0,0));
			lifeTimeBubbles.push_back(0.0);
			if(i<m_nx-1 && j<m_nz-1){
				storePolygon(i,j,nb);
				storeColor(nb);
				storeUV(i,j,nb);
				nb+=6;
			}
		}
	}
	m_t = 0;
}
/****************************************************************************/
/****************************************************************************/
void GridOcean::storePolygon(int i, int j, int nb)
{
	Vector3f pos1 = m_pos[i+j*m_nx];
	Vector3f pos2 = m_pos[i+(j+1)*m_nx];
	Vector3f pos3 = m_pos[(i+1)+(j+1)*m_nx];
	Vector3f pos4 = m_pos[(i+1)+j*m_nx];

	// Triangle 1
	posDisplay[nb*3] = pos1[0]; posDisplay[nb*3+1] = pos1[1]; posDisplay[nb*3+2] = pos1[2];
	posDisplay[(nb+1)*3] = pos2[0]; posDisplay[(nb+1)*3+1] = pos2[1]; posDisplay[(nb+1)*3+2] = pos2[2];
	posDisplay[(nb+2)*3] = pos3[0]; posDisplay[(nb+2)*3+1] = pos3[1]; posDisplay[(nb+2)*3+2] = pos3[2];


	// Triangle 2
	posDisplay[(nb+3)*3] = pos4[0]; posDisplay[(nb+3)*3+1] = pos4[1]; posDisplay[(nb+3)*3+2] = pos4[2];
	posDisplay[(nb+4)*3] = pos1[0]; posDisplay[(nb+4)*3+1] = pos1[1]; posDisplay[(nb+4)*3+2] = pos1[2];
	posDisplay[(nb+5)*3] = pos3[0]; posDisplay[(nb+5)*3+1] = pos3[1]; posDisplay[(nb+5)*3+2] = pos3[2];

}
/****************************************************************************/
void GridOcean::storeUV(int i, int j, int nb)
{
	// Triangle 1
	uv[nb*2] = i/(float)(m_nx-1); uv[nb*2+1] = j/(float)(m_nz-1); 
	uv[(nb+1)*2] = i/(float)(m_nx-1); uv[(nb+1)*2+1] = (j+1)/(float)(m_nz-1); 
	uv[(nb+2)*2] = (i+1)/(float)(m_nx-1); uv[(nb+2)*2+1] = (j+1)/(float)(m_nz-1); 

	// Triangle 2
	uv[(nb+3)*2] = (i+1)/(float)(m_nx-1); uv[(nb+3)*2+1] = j/(float)(m_nz-1); 
	uv[(nb+4)*2] = i/(float)(m_nx-1); uv[(nb+4)*2+1] = j/(float)(m_nz-1); 
	uv[(nb+5)*2] = (i+1)/(float)(m_nx-1); uv[(nb+5)*2+1] = (j+1)/(float)(m_nz-1); 
}
/****************************************************************************/
void GridOcean::storeColor(int nb)
{
	// Triangle 1
	colors[nb*3] = 0; colors[nb*3+1] = 0; colors[nb*3+2] = 1;
	colors[(nb+1)*3] = 0; colors[(nb+1)*3+1] = 0; colors[(nb+1)*3+2] = 1; 
	colors[(nb+2)*3] = 0; colors[(nb+2)*3+1] = 0; colors[(nb+2)*3+2] = 1; 

	// Triangle 2
	colors[(nb+3)*3] = 0; colors[(nb+3)*3+1] = 0; colors[(nb+3)*3+2] = 1;
	colors[(nb+4)*3] = 0; colors[(nb+4)*3+1] = 0; colors[(nb+4)*3+2] = 1;
	colors[(nb+5)*3] = 0; colors[(nb+5)*3+1] = 0; colors[(nb+5)*3+2] = 1;
}
/****************************************************************************/
/****************************************************************************/
void GridOcean::update(std::vector<WaveGroup*> waveGroups, float dt)
{
	for(int i=0;i<m_nx;i++){
		for(int j=0;j<m_nz;j++){
			int index = i + j*m_nx;
			m_pos[index] = m_initPos[index];
			m_vel[index]=Vector3f(0,0,0);
			m_dVel[index]=Vector3f(0,0,0);
			for(unsigned int n=0; n<waveGroups.size(); n++)
			{
			  	Vector3f dPos(0,0,0);
			  	Vector3f vel(0,0,0);
			  	Vector3f dVel(0,0,0);
			  	waveGroups[n]->computeMovement(m_initPos[index],m_t,&dPos,&vel,&dVel);
			  	dPos[0]*=waveGroups[n]->getCosTheta(); dPos[2]*=waveGroups[n]->getSinTheta(); 
			  	vel[0]*=waveGroups[n]->getCosTheta(); vel[2]*=waveGroups[n]->getSinTheta();
			  	dVel[0]*=waveGroups[n]->getCosTheta(); dVel[2]*=waveGroups[n]->getSinTheta();
			  	m_pos[index] += dPos;
			  	m_vel[index] += vel;
			  	m_dVel[index] += dVel;
			}
			if(FBM == 1)
				m_pos[index][1] += SCALE_AMP*perlin_two(m_pos[index][0],m_pos[index][2],GAIN,OCTAVES,tx);
		}
	}
	nb=0;
	for(int i=0;i<m_nx;i++){
		for(int j=0;j<m_nz;j++){
			if(i<m_nx-1 && j<m_nz-1){
				storePolygon(i,j,nb);
				relaxBubbles(i,j);
				nb+=6;
			}
		}
	}

	// updateSprays
	sprays->update(dt);

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
				BreakingWave* br = new BreakingWave(m_initPos[indexCell],wgActing,wgActive,this->m_t-dt,tx*2,tz*2);
				// Mise à jour des points et vel pour regarder les points actifs
				br->update(dt,this);
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
void GridOcean::relaxBubbles(int ix, int iz)
{
	int index = ix + iz*m_nx;

	int nbx = (rand()/(double)RAND_MAX) *2*(texBubbles.width()/(2*getNx()));
 	int nby = (rand()/(double)RAND_MAX) *2*(texBubbles.height()/(2*getNz()));
	
	int x = (int) floor((ix/(float)(m_nx-1))*texBubbles.width());
	int y = (int) floor((iz/(float)(m_nz-1))*texBubbles.height());

	QRgb rgb = texBubbles.pixel(x,y);
	int alpha = qAlpha(rgb);
	// Diminution de la durée de vie du texel
	if(alpha>0 && lifeTimeBubbles[index]>0)
	{
		lifeTimeBubbles[index]--;
		//cout << "Alpha " << alpha << endl;
	}
	float lMax = lifeTimeBubbles[index]; float lMin = 0;
	//cout << "Size: " << texBubbles.width() << " " << texBubbles.height() << endl;
	// Diminution de la transparence pour chaque pixel du texel
	for(int i=x-nbx; i<=x+nbx; i++){
		for(int j=y-nby; j<=y+nby; j++){
			if(i>=0 && j>=0 && i<texBubbles.width() && j<texBubbles.height()){
				QRgb rgb = texBubbles.pixel(i,j);
				int alpha = qAlpha(rgb);
				float dAlpha = (rand()/(double)RAND_MAX)*5;
				//if(dAlpha>0)cout << "dAlpha " << dAlpha << endl;
				texBubbles.setPixel(i,j,qRgba(qRed(rgb),qGreen(rgb),qBlue(rgb),fmax(alpha-dAlpha,0)));
			}
		}
	}				
}
/****************************************************************************/
/****************************************************************************/
void GridOcean::display()
{
	displayPatchesWithTexture(m_program1,tex1,texBubbles, posDisplay,colors,uv,nb);
	displayPatchesWithoutTexture(m_program2,posDisplay,colors,nb);
	if(sprays) sprays->display();
}
/****************************************************************************/
/****************************************************************************/
