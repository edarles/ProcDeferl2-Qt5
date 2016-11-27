/****************************************************************************/
/****************************************************************************/
#ifndef _SPRAY_PARTICLE_H__
#define _SPRAY_PARTICLE_H__

#include <Eigen/Dense>
using namespace Eigen;

class SprayParticle
{
/****************************************************************************/
	public :

/****************************************************************************/
		SprayParticle();
		SprayParticle(Vector3f pos, Vector3f vel, float mass);
		SprayParticle(SprayParticle &P);
		~SprayParticle();

	/**************** GETTERS ***************************/		
		Vector3f getPos();
		Vector3f getVel();
		Vector3f getForces();
		float 	 getMass();
		float 	 getLifeTime();
		float 	 getLifeTimeInit();

	/**************** SETTERS ***************************/		
		void     setPos(Vector3f);
		void     setVel(Vector3f);
		void     setForces(Vector3f);
		void 	 setMass(float);
		void     setLifeTime(float);

		void     addForce(Vector3f);

	/*************** INTEGRATE *************************/
		void     integrate(float dt);

/****************************************************************************/
	private :

		Vector3f pos, vel, forces;
		float mass, lifetime, lifetimeInit;
};
#endif
/****************************************************************************/
/****************************************************************************/
