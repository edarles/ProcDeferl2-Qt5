/****************************************************************************/
/****************************************************************************/
#ifndef _WAVE_GROUP_MP_H__
#define _WAVE_GROUP_MP_H__

#include <WaveGroup.h>
using namespace Eigen;

class WaveGroupMP : public WaveGroup
{
/****************************************************************************/
	public : 

		WaveGroupMP();
		WaveGroupMP(int n, float k, float theta, float r, float phi0, float zeta);
		~WaveGroupMP();

		void computeMovement(Vector3f pos, float t, Vector3f *dPos, Vector3f *vel, Vector3f *dVel);
};
#endif
/****************************************************************************/
/****************************************************************************/
