/****************************************************************************/
/****************************************************************************/
#ifndef _SPRAYS_H__
#define _SPRAYS_H__

#include <SprayParticle.h>
#include <vector>
using namespace std;

class Sprays
{
/****************************************************************************/
	public : 

		Sprays();
		virtual ~Sprays();

		/**************** GETTERS ***************************/		
		SprayParticle* 	   getParticle(int index) const;
		vector<SprayParticle*>  getParticles() const;
		int        	   getNbParticles() const;

		/**************** ROUTINE D'AJOUT / SUPRESSION D'UNE PARTICULE *****/
		virtual void addParticle(Vector3f pos, Vector3f vel, float mass);
		virtual void addParticle(SprayParticle *p);
		virtual void deleteParticle(int index);

		/**************** ROUTINE DE GENERATION D'UNE PARTICULE *****/
		virtual void generate(Vector3f pos, Vector3f vel, float mass, float h, int nb);

		/**************** ROUTINE DE MERGING ****************/
		virtual void merge(Sprays* other);

		/**************** ROUTINES DU SOLVER ****************/
		virtual void update(float dt);

		/**************** ROUTINE D'AFFICHAGE ***************/
		virtual void display();

		/***************  ROUTINE D'EXPORT DES POSITIONS POUR MESHER LA SURFACE ******/
		//virtual void exportParticlesHoudini(const char* filename);

/****************************************************************************/
	protected : 
		vector<SprayParticle*> particles;
};
#endif
/****************************************************************************/
/****************************************************************************/
