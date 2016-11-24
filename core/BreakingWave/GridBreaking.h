/****************************************************************************/
/****************************************************************************/
#ifndef _GRID_BREAKING_WAVE_H__
#define _GRID_BREAKING_WAVE_H__

#include <GridOcean.h>
#include <vector>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;


class GridBreaking : public GridOcean
{
/****************************************************************************/
	public : 

		GridBreaking(Vector3f center, vector<WaveGroup*> wg, vector<bool> wg_active, float tx, float tz);
		~GridBreaking();

		void init();
		void createGrid(vector<WaveGroup*> wg, vector<bool> wg_active);

		void update(vector<WaveGroup*> wg, float dt);

		void checkActivePts(vector<WaveGroup*> wgs, vector<bool> wg_active);
		void checkActiveWgs(vector<WaveGroup*> wgs, vector<bool> *wg_active);

		int         getActivePt(int i);
		void        addActivePt(int index);
		int         getNbActivePts();
		float 	    getMaxLambda();
		float       getMaxExtens();
		int         getIndexMaxLambda();
		Vector3f    getMinAct();
		Vector3f    getMaxAct();

		Vector3f calculScale();
		Vector3f calculTranslation();
		float    calculRotation();

		void scale(Vector3f s);
		void translate(Vector3f t);
		void rotate(float angle);

		void  merge(GridBreaking *other);

		void  display();

	private :
		vector<int> activePts;

		Vector3f barycenter;
		int indexMax; int indexMaxLambda;
		float maxLambda, maxExtens;

		Vector3f minAct, maxAct;
/****************************************************************************/
};
#endif
/****************************************************************************/
/****************************************************************************/
