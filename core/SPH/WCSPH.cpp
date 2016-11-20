#include <WCSPH.h>
#include <iostream>
/****************************************************************************/
/****************************************************************************/
WCSPH::WCSPH(Vector3f origin, Vector3f min, Vector3f max)
      :SESPH(origin,min,max,1119.0)
{
}
/****************************************************************************/
WCSPH::~WCSPH()
{
}
/****************************************************************************/
/****************************************************************************/
void WCSPH::computeRhoP()
{
	for(unsigned int i=0;i<particles.size();i++)
	{
		float dens = 0;
		Vector3f pos1 = particles[i]->getPos(); 
		float h1 = particles[i]->getRadius();

		for(unsigned int j=0;j<particles[i]->getNbVois();j++){
			int index = particles[i]->getVois(j);
			Vector3f pos2 = particles[index]->getPos(); 
			float m2 = particles[index]->getMass();
			float h2 = particles[index]->getRadius();
            		float h = (h1+h2)/2;
			float d = (pos1-pos2).norm();
			dens += m2*315*powf((h*h)-(d*d),3)/(64*M_PI*powf(h,9));
		}
		particles[i]->setRho(dens);
		// Calcul de la pression en utilisation l'équation des gaz parfaits
		particles[i]->setP(1119.0*(powf(dens/rho0,7)-1));
	}	
}
/****************************************************************************/
/****************************************************************************/
