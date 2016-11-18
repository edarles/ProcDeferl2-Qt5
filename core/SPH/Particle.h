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
		Particle(Particle &P);
		~Particle();

		Vector3f pos, vel, forces;
		float mass, radius, rho, p;
		vector<short> vois;
};
#endif
