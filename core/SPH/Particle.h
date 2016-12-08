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

		Vector3f getPos(); // repère local de la grille. Pour coord globales :  gridSPH->getLocalRotated(particle->getPos());
		Vector3f getVel();; // repère global
		Vector3f getForces();
		float 	 getMass();
		float    getRadius();
		float    getRho();
		float    getP();
		Vector3f getColor();
		short    getVois(unsigned int index);
		unsigned int getNbVois();

		void     setPos(Vector3f);
		void     setVel(Vector3f);
		void     setForces(Vector3f);
		void 	 setMass(float);
		void     setRadius(float);
		void     setRho(float);
		void     setP(float);
		void     setColor(Vector3f);
		void     setVois(short);
		void     clearVois();

	private :

		Vector3f pos, vel, forces, color;
		float mass, radius, rho, p;
		vector<short> vois;
};
#endif
