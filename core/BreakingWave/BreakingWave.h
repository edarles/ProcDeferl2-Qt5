/****************************************************************************/
/****************************************************************************/
#ifndef _BREAKING_WAVE_H__
#define _BREAKING_WAVE_H__

#include <WaveGroup.h>
#include <WCSPH.h>
#include <GridBreaking.h>

#include <vector>

using namespace std;

class BreakingWave
{
/****************************************************************************/
	public : 

		BreakingWave(Vector3f center, vector<WaveGroup*> wg_acting, vector<bool> wg_active, float t);
		virtual ~BreakingWave();

		/**************** GETTERS ***************************/
		GridBreaking* getGridBreaking();
		WaveGroup* getWgActing(int index);
		int 	   getNbWgActing();
		bool getWgActive(int index);
		SPH* getSolver();
		bool getIsActive();

		/**************** SETTERS ***************************/
		void setGridBreaking(GridBreaking* gridBreaking);

		int addWgActing(WaveGroup* wg);
		int wgActingContains(WaveGroup* wg);

		void addWgActive(bool active);
		void setWgActive(int index, bool active);

		void setSolver(SPH*);

		void setIsActive(bool active);

		/**************** ROUTINE DE VERIFICATIONS **********/
		void checkActivePts();
		void checkActiveWgs();
		bool checkIfActive();

		/**************** ROUTINE DE TRANSFORMATION *********/
		void scale();
		void resetting();
		void rotation();
		void transform();

		/**************** ROUTINE DE MERGING ****************/
		void merge(BreakingWave *br, float dt);

		/**************** ROUTINE DE GENERATION DES PARTICULES ******/
		void generateParticles(float dt);

		/**************** ROUTINE DE MAJ ********************/
		void update(float dt);

		/**************** ROUTINE D'AFFICHAGE ****************/
		void display();

		void print();
/****************************************************************************/
	private:

		 Vector3f center;

		 GridBreaking* gridBreaking;

		 // Liste des WaveGroups qui agissent localement dans la zone
		 vector<WaveGroup*> wg_acting;

		 // Liste des WaveGroups qui sont actifs au déferlement
		 vector<bool> wg_active;

		 // Solveur SPH
		 SPH *solver;

		 bool isActive;

};
#endif
/****************************************************************************/
/****************************************************************************/
