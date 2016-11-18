#include <GridSPH.h>
#include <iostream>
/****************************************************************************/
/****************************************************************************/
GridSPH::GridSPH():Grid(Vector3f(0,0,0),Vector3f(-1,-1,-1),Vector3f(1,1,1),0.05,0.05,0.05)
{
	init();
	color = Vector4f(rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,1);
}
/****************************************************************************/
GridSPH::GridSPH(Vector3f center, Vector3f min, Vector3f max, float dx, float dy, float dz):Grid(center,min,max,dx,dy,dz)
{
	init();
	color = Vector4f(rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,rand()/(float)RAND_MAX,1);
}
/****************************************************************************/
GridSPH::~GridSPH()
{
}
/*************************************************/
/*************************************************/
void GridSPH::computeNeighborhood(vector<Particle*> particles)
{
	vector< vector<short> > indexs;
	int nbMaxPart = 0;
	// Store particles into cells
	for(int ix=0; ix<m_nx; ix++){
		 for(int iz=0; iz<m_nz; iz++){
			for(int iy=0; iy<m_ny; iy++){
				int indexCell = ix + iz*m_nx + iy*m_nx*m_nz;
				indexs.push_back(vector<short>());
			}
		}
	}
	for(int i=0;i<particles.size();i++){
		Vector3f pos = getLocalRotated(particles[i]->pos);
	  	int iC = floor((pos[0]-m_min[0])/m_dx);
	  	int jC = floor((pos[1]-m_min[1])/m_dy);
	  	int kC = floor((pos[2]-m_min[2])/m_dz);
	  	int indexCell = iC + kC*m_nx + jC*m_nx*m_nz;
		if(indexCell<m_n)
			indexs[indexCell].push_back(i);
	}
	// Compute neighboorhood for each particle
	for(int iP=0;iP<particles.size();iP++){

		particles[iP]->vois.clear();
		
		Vector3f pos = getLocalRotated(particles[iP]->pos);
		float h = particles[iP]->radius;
		float x = pos[0]; 
		float y = pos[1];
		float z = pos[2];

		int ix = floor((x-m_min[0])/m_dx);
		int iy = floor((y-m_min[1])/m_dy);
		int iz = floor((z-m_min[2])/m_dz);
		//cout << "ix: " << ix << " iy: " << iy << " iz: " << iz << endl;
	        int nbcx = floor(h/m_dx);
	        int nbcy = floor(h/m_dy);
	        int nbcz = floor(h/m_dz);
	        int nbP = 0;
		//cout << "cx: " << nbcx << " cy: " << nbcy << " cz: " << nbcz << endl;
		if(ix>=0 && iy>=0 && iz>=0 && ix < m_nx && iy < m_ny && iz < m_nz){

			for(int i=ix-nbcx; i<=ix+nbcx; i++){
				for(int j=iz-nbcz; j<=iz+nbcz; j++){
					for(int k=iy-nbcy; k<=iy+nbcy; k++){
						 if(i>=0 && j>=0 && k>=0 && i<m_nx && j<m_nz && k<m_ny){

						 	int indexCell = i + j*m_nx + k*m_nx*m_nz;
						 	if(indexCell>=0 && indexCell<m_n){
						 		for(int n=0;n<indexs[indexCell].size();n++)
						 		{
									short iP2 = indexs[indexCell][n];
									Vector3f pos2 = getLocalRotated(particles[iP2]->pos);
									Vector3f PP = pos - pos2;
									float h2 = particles[iP2]->radius;
									float r = (h+h2)/2;//max(h,h2);
									if(PP.norm()<=r)
											particles[iP]->vois.push_back(iP2);
								  		
								}
							}
						}
					}
				}
			}
		}
	}
	for(int i=0;i<indexs.size();i++){
		indexs[i].clear();
		indexs[i].shrink_to_fit();
		indexs[i].~vector();
	}
	indexs.clear();
	indexs.shrink_to_fit();
	indexs.~vector();
	//cout << "Max NEIGH : " << nbMaxPart << endl;
}
/****************************************************************************/
/****************************************************************************/
