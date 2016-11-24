#include <iostream>
#include <stdlib.h>
#include <vector>
#include <Grid.h>
#include <GridSPH.h>
#include <GL/gl.h>

using namespace std;
/****************************************************************************/
/****************************************************************************/
Grid::Grid()
{
	angleRotation = 0;
	m_center << 0,0,0;
	m_min << -10.0,0.0,-10.0;
	m_max << 10.0,0.0,10.0;
	m_dx = m_dy = m_dz = 0.1;
	m_pos.clear();
	createGrid();
	color << 1,1,1,1;
}
/****************************************************************************/
Grid::Grid(Vector3f min, Vector3f max, float dx, float dy, float dz)
{
	angleRotation = 0;
	this->m_center = Vector3f((max[0]+min[0])/2,(max[1]+min[1])/2,(max[2]+min[2])/2);
	this->m_min = min;
	this->m_max = max;
	this->m_dx = dx;
	this->m_dy = dy;
	this->m_dz = dz;
	m_pos.clear();
	createGrid();
	color << 1,1,1,1;
}
/****************************************************************************/
Grid::~Grid()
{
	m_pos.clear();
	vector<Vector3f>().swap(m_pos);
	m_pos.shrink_to_fit();
}
/*************************************************/
/*************************************************/
Vector3f Grid::getCenter()
{
	return m_center;
}
/*************************************************/
Vector3f Grid::getMin()
{
	return m_min;
}
/*************************************************/
Vector3f Grid::getMax()
{
	return m_max;
}
/*************************************************/
float    Grid::getDx()
{
	return m_dx;
}
/*************************************************/
float    Grid::getDy()
{
	return m_dy;
}
/*************************************************/
float    Grid::getDz()
{
	return m_dz;
}
/*************************************************/
int      Grid::getNx()
{
	return m_nx;
}
/*************************************************/
int      Grid::getNy()
{
	return m_ny;
}
/*************************************************/
int      Grid::getNz()
{
	return m_nz;
}
/*************************************************/
int      Grid::getMn()
{
	return m_n;
}
/*************************************************/
Vector3f Grid::getPos(int ix, int iy, int iz)
{
	assert(ix<m_nx && iy<m_ny && iz<m_nz);
	int indexCell = ix + iz*m_nx + iy*m_nx*m_ny;
	return m_pos[indexCell];
}
/*************************************************/
Vector3f Grid::getPos(int indexCell)
{
	assert(indexCell < m_n);
	return m_pos[indexCell];
}
/*************************************************/
Vector4f Grid::getColor()
{
	return color;
}
/*************************************************/
float Grid::getAngleRotation()
{
	return angleRotation;
}
/*************************************************/
/*************************************************/
void Grid::setCenter(Vector3f center)
{
	m_center = center;
}
/*************************************************/
void Grid::setMin(Vector3f min)
{
	m_min = min;
}
/*************************************************/
void Grid::setMax(Vector3f max)
{
	m_max = max;
}
/*************************************************/
void Grid::setDx(float dx)
{
	m_dx = dx;
}
/*************************************************/
void Grid::setDy(float dy)
{
	m_dy = dy;
}
/*************************************************/
void Grid::setDz(float dz)
{
	m_dz = dz;
}
/*************************************************/
void Grid::setNx(int nx)
{
	m_nx = nx;
}
/*************************************************/
void Grid::setNy(int ny)
{
	m_ny = ny;
}
/*************************************************/
void Grid::setNz(int nz)
{
	m_nz = nz;
}
/*************************************************/
void Grid::setPos(int ix, int iy, int iz, Vector3f pos)
{
	int indexCell = ix + iz*m_nx + iy*m_nx*m_ny;
	m_pos[indexCell] = pos;
}
/*************************************************/
void Grid::setColor(Vector4f color)
{
	this->color = color;
}
void Grid::setAngleRotation(float angle)
{
	this->angleRotation = angle;
}
/*************************************************/
void Grid::init()
{
}
/*************************************************/
Vector3f Grid::getLocalRotated(Vector3f pos)
{
	Matrix3f M;
	M << cos(-angleRotation), 0, -sin(-angleRotation), 0, 1, 0, sin(-angleRotation), 0, cos(-angleRotation);
	Vector3f local = pos;
	local -= m_center;
	local = M*local;
	local += m_center;
	return local;
}
/****************************************************************************/
Vector3f Grid::getLocalRotate(Vector3f pos)
{
	Matrix3f M;
	M << cos(angleRotation), 0, -sin(angleRotation), 0, 1, 0, sin(angleRotation), 0, cos(angleRotation);
	Vector3f local = pos;
	local -= m_center;
	local = M*local;
	local += m_center;
	return local;
}
/****************************************************************************/
/****************************************************************************/
void Grid::translate(Vector3f t)
{
	m_max += t;
	m_min += t;
	m_center += t;

	for(int ix=0; ix<m_nx; ix++){
		for(int iz=0; iz<m_nz; iz++){
		 	if(m_ny>0){
				 for(int iy=0; iy<m_ny; iy++){
					int indexCell = ix + iz*m_nx + iy*m_nx*m_nz;
					m_pos[indexCell]+=t;
				}
			}
			else {
					int indexCell = ix + iz*m_nx;
					m_pos[indexCell]+=t;
				}
		}
	}
}
/*************************************************/
void Grid::scale(Vector3f s)
{
	Vector3f omax = m_max - m_center;
	Vector3f omin = m_min - m_center;
	
	m_min[0] = m_center[0] + omin[0]*s[0]; m_min[2] = m_center[2] + omin[2]*s[2];
	m_max[0] = m_center[0] + omax[0]*s[0]; m_max[2] = m_center[2] + omax[2]*s[2];
	
	if(m_ny>0){
		m_min[1] = m_center[1] + omin[1]*s[1]; 
		m_max[1] = m_center[1] + omax[1]*s[1]; 
	}
	for(int ix=0; ix<m_nx; ix++){
		for(int iz=0; iz<m_nz; iz++){
			if(m_ny>0){
				 for(int iy=0; iy<m_ny; iy++){
					int indexCell = ix + iz*m_nx + iy*m_nx*m_nz;
					Vector3f posAv = getLocalRotated(m_pos[indexCell]) - m_center;
					posAv[0]= m_center[0] + posAv[0]*s[0]; 
					posAv[1]= m_center[1] + posAv[1]*s[1]; 
					posAv[2]= m_center[2] + posAv[2]*s[2];
					m_pos[indexCell] = getLocalRotate(posAv);
				}
			}
			else {
				int indexCell = ix + iz*m_nx;
				Vector3f posAv = getLocalRotated(m_pos[indexCell]) - m_center;
				posAv[0]= m_center[0] + posAv[0]*s[0]; 
				posAv[2]= m_center[2] + posAv[2]*s[2];
				m_pos[indexCell] = getLocalRotate(posAv);
			}
		}
	}
	m_center = (m_max+m_min)/2;
	m_dx*=s[0];m_dy*=s[1];m_dz*=s[2];
}
/*************************************************/
void Grid::rotate(float theta)
{
	Vector3f c = m_center;
	Matrix3f M;
	M << cos(theta), 0, -sin(theta), 0, 1, 0, sin(theta), 0, cos(theta);
	
	translate(-c);
	for(int ix=0; ix<m_nx; ix++){
		for(int iz=0; iz<m_nz; iz++){
			if(m_ny>0){
				 for(int iy=0; iy<m_ny; iy++){
					int indexCell = ix + iz*m_nx + iy*m_nx*m_nz;
					m_pos[indexCell]=M*m_pos[indexCell];
				}
			}
			else {
					int indexCell = ix + iz*m_nx;
					m_pos[indexCell]=M*m_pos[indexCell];
			}
		}
	}
	translate(c);
}
/****************************************************************************/
/****************************************************************************/
void Grid::createGrid()
{
	m_pos.clear();
	vector<Vector3f>().swap(m_pos);

	float l = m_max[0]-m_min[0]; m_sizeX = l;
	float w = m_max[1]-m_min[1]; m_sizeY = w;
	float d = m_max[2]-m_min[2]; m_sizeZ = d;

	m_nx = ceil(l/m_dx); 
	m_nz = ceil(d/m_dz); 

	if(w==0 || m_dy==0) m_ny=0; 
	else m_ny = ceil(w/m_dy); 

	m_n = m_nx+m_nx*m_nz+m_nx*m_ny*m_nz;

	for(int i=0;i<m_n;i++)
		m_pos.push_back(Vector3f(0,0,0));
		
	for(int ix=0; ix<m_nx; ix++){
		for(int iz=0; iz<m_nz; iz++){
		 	if(m_ny>0){
				 for(int iy=0; iy<m_ny; iy++){
					int indexCell = ix + iz*m_nx + iy*m_nx*m_nz;
					Vector3f pos(m_center[0]-(l/2)+(m_dx/2)+ix*m_dx,m_center[1]-(w/2)+(m_dy/2)+iy*m_dy,
					             m_center[2]-(d/2)+(m_dz/2)+iz*m_dz);
					m_pos[indexCell] = pos;
				}
			}
			else {
					Vector3f pos(m_center[0]-(l/2)+(m_dx/2)+ix*m_dx, m_center[1],
					         m_center[2]-(d/2)+(m_dz/2)+iz*m_dz);
					int indexCell = ix + iz*m_nx;
					m_pos[indexCell] = pos;
				}
		}
	}
}
/****************************************************************************/
/****************************************************************************/
void Grid::merge(Grid* other)
{
	Vector3f Min, Max;
	Min[0] = (float)fmin((double)m_min[0],(double)other->m_min[0]);
	Min[2] = (float)fmin((double)m_min[2],(double)other->m_min[2]);

	Max[0] = (double)fmax((double)m_max[0],(double)other->m_max[0]);
	Max[2] = (double)fmax((double)m_max[2],(double)other->m_max[2]);

	if(m_dy!=0){
		Min[1] = (double)fmin((double)m_min[1],(double)other->m_min[1]);
		Max[1] = (double)fmax((double)m_max[1],(double)other->m_max[1]);
		m_dy = fmin(m_dy,other->m_dy);
	}
	else {
		Min[1] = 0; Max[1] = 0;
	}
	this->m_min = Min; this->m_max = Max;
	this->m_center = (this->m_min + this->m_max)/2;

	if(typeid(*other)==typeid(GridSPH))
		this->m_center[0] = this->m_center[0] + (this->m_max[0]-this->m_min[0])/4;

	m_dx = fmin(m_dx,other->m_dx);
	m_dz = fmin(m_dz,other->m_dz);

	createGrid();
}

/****************************************************************************/
/****************************************************************************/
void Grid::exportOBJ(const char* filename)
{
	if(m_ny==0){
		FILE *f = fopen(filename,"w");
		if(f!=NULL){
			// Export vertices
			for(int ix=0; ix<m_nx; ix++){
				for(int iz=0; iz<m_nz; iz++){
		 			int indexCell = ix + iz*m_nx;
					fprintf(f,"v %f %f %f\n",m_pos[indexCell][0],m_pos[indexCell][1],m_pos[indexCell][2]);
				}
			}
			// Export Quads
			for(int i=0;i<m_nx-1;i++){
				for(int j=0;j<m_nz-1;j++){
					int indexs[4];
					indexs[0] = i + j*m_nx;
					indexs[1] = i + (j+1)*m_nx;
					indexs[2] = (i+1) + (j+1)*m_nx;
					indexs[3] = (i+1) + j*m_nx;
					fprintf(f,"f %d// %d// %d// %d//\n",indexs[0]+1,indexs[1]+1,indexs[2]+1,indexs[3]+1);

				}
			}
			fclose(f);
		}
		else
			std::cout << "Pb Ecriture !" << std::endl;
	}
	else
		std::cout << "Export OBJ Impossible : Grille volumique !" << std::endl;
}
/****************************************************************************/
/****************************************************************************/
void Grid::display()
{
	glColor4f((float)color[0],(float)color[1],(float)color[2],(float)color[3]);
	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
	for(int i=0;i<m_nx-1;i++){
		for(int j=0;j<m_nz-1;j++){

			// Cas de la grille surfacique
			if(m_ny<=0){
				int indexs[4];
				indexs[0] = i + j*m_nx;
				indexs[1] = i + (j+1)*m_nx;
				indexs[2] = (i+1) + (j+1)*m_nx;
				indexs[3] = (i+1) + j*m_nx;

				Vector3f pos1 = m_pos[indexs[0]];
				Vector3f pos2 = m_pos[indexs[1]];
				Vector3f pos3 = m_pos[indexs[2]];
				Vector3f pos4 = m_pos[indexs[3]];

				glBegin(GL_QUADS);
				glVertex3f(pos1[0],pos1[1],pos1[2]);
				glVertex3f(pos2[0],pos2[1],pos2[2]);
				glVertex3f(pos3[0],pos3[1],pos3[2]);
				glVertex3f(pos4[0],pos4[1],pos4[2]);
				glEnd();
				//printf("i:%d pos:%f %f %f\n",indexs[0], m_pos[indexs[0]][0],m_pos[indexs[0]][1],m_pos[indexs[0]][2]);

			}
			// Cas de la grille volumique
			else {
				for(int k=0;k<m_ny-1;k++){
					int indexs[8];
					indexs[0] = i + j*m_nx + k*m_nx*m_nz;
					indexs[1] = i + (j+1)*m_nx + k*m_nx*m_nz;
					indexs[2] = (i+1) + (j+1)*m_nx + k*m_nx*m_nz;
					indexs[3] = (i+1) + j*m_nx + k*m_nx*m_nz;

					indexs[4] = i + j*m_nx + (k+1)*m_nx*m_nz;
					indexs[5] = i + (j+1)*m_nx + (k+1)*m_nx*m_nz;
					indexs[6] = (i+1) + (j+1)*m_nx + (k+1)*m_nx*m_nz;
					indexs[7] = (i+1) + j*m_nx + (k+1)*m_nx*m_nz;

					glBegin(GL_QUADS);
					glVertex3f(m_pos[indexs[0]][0],m_pos[indexs[0]][1],m_pos[indexs[0]][2]);
					glVertex3f(m_pos[indexs[1]][0],m_pos[indexs[1]][1],m_pos[indexs[1]][2]);
					glVertex3f(m_pos[indexs[2]][0],m_pos[indexs[2]][1],m_pos[indexs[2]][2]);
					glVertex3f(m_pos[indexs[3]][0],m_pos[indexs[3]][1],m_pos[indexs[3]][2]);
					glEnd();

					glBegin(GL_QUADS);
					glVertex3f(m_pos[indexs[4]][0],m_pos[indexs[4]][1],m_pos[indexs[4]][2]);
					glVertex3f(m_pos[indexs[5]][0],m_pos[indexs[5]][1],m_pos[indexs[5]][2]);
					glVertex3f(m_pos[indexs[6]][0],m_pos[indexs[6]][1],m_pos[indexs[6]][2]);
					glVertex3f(m_pos[indexs[7]][0],m_pos[indexs[7]][1],m_pos[indexs[7]][2]);
					glEnd();

					glBegin(GL_QUADS);
					glVertex3f(m_pos[indexs[0]][0],m_pos[indexs[0]][1],m_pos[indexs[0]][2]);
					glVertex3f(m_pos[indexs[1]][0],m_pos[indexs[1]][1],m_pos[indexs[1]][2]);
					glVertex3f(m_pos[indexs[5]][0],m_pos[indexs[5]][1],m_pos[indexs[5]][2]);
					glVertex3f(m_pos[indexs[4]][0],m_pos[indexs[4]][1],m_pos[indexs[4]][2]);
					glEnd();

					glBegin(GL_QUADS);
					glVertex3f(m_pos[indexs[3]][0],m_pos[indexs[3]][1],m_pos[indexs[3]][2]);
					glVertex3f(m_pos[indexs[2]][0],m_pos[indexs[2]][1],m_pos[indexs[2]][2]);
					glVertex3f(m_pos[indexs[6]][0],m_pos[indexs[6]][1],m_pos[indexs[6]][2]);
					glVertex3f(m_pos[indexs[7]][0],m_pos[indexs[7]][1],m_pos[indexs[7]][2]);
					glEnd();

					glBegin(GL_QUADS);
					glVertex3f(m_pos[indexs[0]][0],m_pos[indexs[0]][1],m_pos[indexs[0]][2]);
					glVertex3f(m_pos[indexs[3]][0],m_pos[indexs[3]][1],m_pos[indexs[3]][2]);
					glVertex3f(m_pos[indexs[7]][0],m_pos[indexs[7]][1],m_pos[indexs[7]][2]);
					glVertex3f(m_pos[indexs[4]][0],m_pos[indexs[4]][1],m_pos[indexs[4]][2]);
					glEnd();

					glBegin(GL_QUADS);
					glVertex3f(m_pos[indexs[1]][0],m_pos[indexs[1]][1],m_pos[indexs[1]][2]);
					glVertex3f(m_pos[indexs[2]][0],m_pos[indexs[2]][1],m_pos[indexs[2]][2]);
					glVertex3f(m_pos[indexs[6]][0],m_pos[indexs[6]][1],m_pos[indexs[6]][2]);
					glVertex3f(m_pos[indexs[5]][0],m_pos[indexs[5]][1],m_pos[indexs[5]][2]);
					glEnd();
				}
			}
		}
	}
	glColor3f(1,0,0);
	glPointSize(5.0);
	glBegin(GL_POINTS);
	glVertex3f(m_center[0],m_center[1],m_center[2]);
	glEnd();

	
}
/****************************************************************************/
/****************************************************************************/
