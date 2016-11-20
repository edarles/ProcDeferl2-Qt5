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
	public :

		Particle();
		Particle(Vector3f pos, Vector3f vel, float mass, float radius);
		Particle(Particle &P);
		~Particle();

		Vector3f getPos();
		Vector3f getVel();
		Vector3f getForces();
		float 	 getMass();
		float    getRadius();
		float    getRho();
		float    getP();
		short    getVois(unsigned int index);
		unsigned int getNbVois();

		void     setPos(Vector3f);
		void     setVel(Vector3f);
		void     setForces(Vector3f);
		void 	 setMass(float);
		void     setRadius(float);
		void     setRho(float);
		void     setP(float);
		void     setVois(short);
		void     clearVois();

	private :

		Vector3f pos, vel, forces;
		float mass, radius, rho, p;
		vector<short> vois;
};
#endif
