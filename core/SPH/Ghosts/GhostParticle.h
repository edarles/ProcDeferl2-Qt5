/****************************************************************************/
/****************************************************************************/
#ifndef _GHOST_PARTICLE_H__
#define _GHOST_PARTICLE_H__

#include <SPHParticle.h>

class GhostParticle : public SPHParticle
{
/****************************************************************************/
	    GhostParticle();
		GhostParticle(Vector3f pos, float mass, float radius);
		GhostParticle(const GhostParticle &P);
		~GhostParticle();

		/*************** INTEGRATE *************************/
		virtual void  integrate(float dt);
};
#endif
/****************************************************************************/
/****************************************************************************/
