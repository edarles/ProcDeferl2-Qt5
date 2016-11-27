/****************************************************************************/
/****************************************************************************/
#ifndef _GRID_BREAKING_WAVE_H__
#define _GRID_BREAKING_WAVE_H__

#include <GridOcean.h>
#include <vector>
#include <Eigen/Dense>

using namespace Eigen;
using namespace std;


class GridBreaking : public Grid
{
/****************************************************************************/
	public : 

		/************** CONSTRUCTEUR ET DESTRUCTEUR  **************/
		GridBreaking(Vector3f center, vector<WaveGroup*> wg, vector<bool> wg_active, float tx, float tz);
		~GridBreaking();

		/*************** ROUTINE DE CREATION *****************/
		void init();
		void createGrid(vector<WaveGroup*> wg, vector<bool> wg_active);

		/*************** ROUTINE DE MISE A JOUR **************/
		void update(vector<WaveGroup*> wg, float dt);

		/*** ROUTINE POUR TROUVER LES POINTS ET LES GROUPES DE VAGUES ACTIFS ***/

		void checkActivePts(vector<WaveGroup*> wgs, vector<bool> wg_active);
		void checkActiveWgs(vector<WaveGroup*> wgs, vector<bool> *wg_active);

		/**************** GETTERS ***************************/
		int         getActivePt(int i);
		void        addActivePt(int index);
		int         getNbActivePts();
		float 	    getMaxLambda();
		float       getMaxExtens();
		int         getIndexMaxLambda();
		Vector3f    getMinAct();
		Vector3f    getMaxAct();
		Vector3f    getVel(int indexCell);
		float       getT();

		/**************** SETTERS ***************************/
		void        setT(float);

		/**************** ROUTINE DE CALCUL ET DE TRANSFORMATION ******/
		Vector3f calculScale();
		Vector3f calculTranslation();
		float    calculRotation();

		void 	scale(Vector3f s);
		void 	translate(Vector3f t);
		void 	rotate(float angle);

		/*************** ROUTINE DE MERGING *****************/
		void  	merge(GridBreaking *other);

		/*************** ROUTINE D'AFFICHAGE ****************/
		void  display();

	private :
		/****** TABLEAU D'INDICES DES POINTS ACTIFS ******/
		vector<int> activePts;

		/****** TABLEAUX DES POSITIONS COURANTES ET VITESSE ******/
		vector<Vector3f> m_initPos;
		vector<Vector3f> m_vel;

		/****** TEMPS COURANT *****/
		float m_t;

		Vector3f barycenter;
		int indexMax; int indexMaxLambda;
		float maxLambda, maxExtens;

		Vector3f minAct, maxAct;
		float tx, tz;

/****************************************************************************/
};
#endif
/****************************************************************************/
/****************************************************************************/
