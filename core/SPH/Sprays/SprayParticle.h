/****************************************************************************/
/****************************************************************************/
#ifndef _SPRAY_PARTICLE_H__
#define _SPRAY_PARTICLE_H__

#include <Particle.h>

class SprayParticle : public Particle
{
/****************************************************************************/
	public:

/****************************************************************************/
		SprayParticle();
		SprayParticle(Vector3f pos, Vector3f vel, float mass);
		SprayParticle(const SprayParticle &P);
		~SprayParticle();

	/**************** GETTERS ***************************/		
		float 	 getLifeTime();
		float 	 getLifeTimeInit();

	/**************** SETTERS ***************************/		
		void     setLifeTime(float);

	/*************** Méthode integrate *****************/
		void     integrate(float dt);

/****************************************************************************/
	private:

		float lifetime, lifetimeInit;
};
#endif
/****************************************************************************/
/****************************************************************************/
