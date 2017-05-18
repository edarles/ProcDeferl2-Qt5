/****************************************************************************/
/****************************************************************************/
#ifndef _PARTICLE_H__
#define _PARTICLE_H__

#include <Eigen/Dense>
#include <vector>

using namespace Eigen;
using namespace std;

class Particle
{
	public:

		Particle();
		Particle(Vector3f pos, Vector3f vel, float mass);
		Particle(const Particle &P);
		~Particle();

		Vector3f getPos(); // repère local de la grille. Pour coord globales :  gridSPH->getLocalRotated(particle->getPos());
		Vector3f getVel();; // repère global
		Vector3f getForces();
        Vector3f getColor();
		float 	 getMass();

		void     setPos(Vector3f pos);
		void     setVel(Vector3f vel);
		void     setForces(Vector3f forces);
        void     setColor(Vector3f color);
		void 	 setMass(float mass);
        void     addForce(Vector3f);

         /*************** INTEGRATE *************************/
		virtual void  integrate(float dt) = 0;

	protected:

		Vector3f pos, vel, forces, color;
		float mass;

};
#endif
