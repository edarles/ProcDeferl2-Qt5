/****************************************************************************/
/****************************************************************************/
#ifndef _SPH_PARTICLE_H__
#define _SPH_PARTICLE_H__

#include <Particle.h>

class SPHParticle : public Particle
{
	public:

		SPHParticle();
		SPHParticle(Vector3f pos, Vector3f vel, float mass, float radius);
		SPHParticle(const SPHParticle &P);
		~SPHParticle();

		float    getRadius();
		float    getRho();
		float    getP();
		short    getVois(unsigned int index);
		unsigned int getNbVois();

		void     setRadius(float);
		void     setRho(float);
		void     setP(float);
		void     setVois(short);
		void     clearVois();

		/*************** INTEGRATE *************************/
		virtual void  integrate(float dt);

	protected:

		float radius, rho, p;
		vector<short> vois;
};
#endif
