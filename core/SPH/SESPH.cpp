#include <SESPH.h>
/****************************************************************************/
/****************************************************************************/
SESPH::SESPH(Vector3f origin, Vector3f min, Vector3f max, float k):SPH(origin,min,max)
{
	this->k = k;
}
/****************************************************************************/
SESPH::~SESPH()
{
}
/****************************************************************************/
/****************************************************************************/
float SESPH::getK()
{
	return k;
}
/****************************************************************************/
/****************************************************************************/
void SESPH::setK(float k)
{
	this->k = k;
}
/****************************************************************************/
/****************************************************************************/
void SESPH::computeRhoP()
{
	for(int i=0;i<particles.size();i++)
	{
		float dens = 0;
		Vector3f pos1 = particles[i]->pos; 
		float h1 = particles[i]->radius;

		for(int j=0;j<particles[i]->vois.size();j++){
			int index = particles[i]->vois[j];
			Vector3f pos2 = particles[index]->pos; 
			float m2 = particles[index]->mass;
			float h2 = particles[index]->radius;
            		float h = (h1+h2)/2;
			float d = (pos1-pos2).norm();
			dens += m2*315*powf((h*h)-(d*d),3)/(64*M_PI*powf(h,9));
		}
		particles[i]->rho = dens;
		// Calcul de la pression en utilisation l'équation des gaz parfaits
		particles[i]->p = k*(rho0-particles[i]->rho);
	}	
}
/****************************************************************************/
/****************************************************************************/
