/****************************************************************************/
/****************************************************************************/
#ifndef _SPH_H__
#define _SPH_H__

#include <Particle.h>
#include <GridSPH.h>
#include <shaders.h>
#include <GL/gl.h>

#include <WaveGroup.h>

class SPH
{
/****************************************************************************/
	public : 

		SPH(Vector3f origin, Vector3f min, Vector3f max);
		virtual ~SPH();

		/**************** GETTERS ***************************/		
		Particle* 	   getParticle(int index);
		vector<Particle*>  getParticles();
		int        	   getNbParticles();
		GridSPH*    	   getGrid() const;
		float 	   	   getRho0();
		float 	   	   getMu();
		float 	   	   getTS();
		float 	   	   getLTS(); 

		/**************** ROUTINE D'AJOUT / SUPRESSION D'UNE PARTICULE *****/
		virtual void addParticle(Vector3f pos, Vector3f vel, float mass, float radius);
		virtual void addParticle(Particle *p);
		virtual void deleteParticle(int index);

		/**************** ROUTINE DE GENERATION D'UNE PARTICULE *****/
		virtual void generateParticle(Vector3f pos, Vector3f vel, float mass);

		/**************** ROUTINE DE MERGING ****************/
		virtual void merge(SPH* other);

		/**************** ROUTINES DU SOLVER ****************/
		virtual void update(vector<WaveGroup*> waveGroups, float time, float dt);

		/****************************************************/
		void constraintGridSPH();

		/**************** ROUTINE D'AFFICHAGE ***************/
		virtual void display();
		void         displayParticlesByField(int field);
		Vector3d     convertHsvToRgb(Vector3d Hsv);

		/***************  ROUTINE D'EXPORT DES POSITIONS POUR MESHER LA SURFACE ******/
		virtual void exportParticlesHoudini(const char* filename);

		/**************** GETTERS ***************************/
		

/****************************************************************************/
	protected : 

		// Grille de voisinage
		GridSPH *gridSPH;

		// Statics attributes
		static GLuint m_program;
		static const Vector3f gravity;
		static const float rho0;
		static const float mu;
		static const float tS;
		static const float ltS;
		static const float massMaxi;

		vector<Particle*> particles;

		//Arrays for GLSL shader 
		float* posP, *colors;

		// Protected Methods
		virtual bool computeNeighborhood();
		virtual void computeRhoP() = 0;
		virtual void computeForces();
		virtual void integrate(float dt);
		virtual void collideExterior(vector<WaveGroup*> waveGroups, float time);
};
#endif
/****************************************************************************/
/****************************************************************************/
