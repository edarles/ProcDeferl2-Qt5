/****************************************************************************/
/****************************************************************************/
#ifndef _GRID_OCEAN_H__
#define _GRID_OCEAN_H__

#include <Grid.h>
#include <WaveGroup.h>
#include <vector>
using namespace std;

class BreakingWave;

class GridOcean : public Grid
{
/****************************************************************************/
	public : 

		GridOcean();
		GridOcean(Vector3f center, Vector3f min, Vector3f max, float dx, float dz);
		virtual ~GridOcean();

		/**************** GETTERS ***************************/
		Vector3f    getInitPos(int ix, int iz);
		Vector3f    getInitPos(int indexCell);
		Vector3f    getVel(int ix, int iz);
		Vector3f    getVel(int indexCell);
		Vector3f    getDVel(int ix, int iz);
		Vector3f    getDVel(int indexCell);

		float       getT();

		/**************** SETTERS ***************************/
		void setInitPos(int ix, int iz, Vector3f initPos);
		void setVel(int ix, int iz, Vector3f vel);
		void setDVel(int ix, int iz, Vector3f dVel);

		void setT(float t);

		/**************** ROUTINE DE REINITIALISATION *******/
		void init();
		void reinitPos();

		/**************** ROUTINE DE MAJ ********************/
		void update(vector<WaveGroup*> waveGroups, float dt);

		/**************** ROUTINE DE GENERATION DE VAGUES DEFERLANTES ******/
		void generateBreakingWaves(vector<WaveGroup*> waveGroups, vector<BreakingWave*> *breakingWaves, float dt);

/****************************************************************************/
	protected : 

		 vector<Vector3f> m_initPos;
		 vector<Vector3f> m_vel;
		 vector<Vector3f> m_dVel;

		 float m_t;
};
#endif
/****************************************************************************/
/****************************************************************************/
