/****************************************************************************/
/****************************************************************************/
#ifndef _HYBRID_OCEAN_H__
#define _HYBRID_OCEAN_H__

#include <stdio.h>
#include <cassert>

#include <WaveGroup.h>
#include <WaveGroupMP.h>
#include <GridOcean.h>
#include <BreakingWave.h>
#include <vector>
#include <iostream>

#include <QGLViewer/camera.h>
#include <tinyxml.h>

using namespace std;

class HybridOcean
{
/****************************************************************************/
	public : 

		HybridOcean();
		HybridOcean(vector<WaveGroup*> waveGroups, float dt);
		HybridOcean(GridOcean* gridOcean, vector<WaveGroup*> waveGroups, float dt);
		virtual ~HybridOcean();

	/**************** GETTERS AND SETTERS ***************************/
		WaveGroup*  getWaveGroup(int index);
		vector<WaveGroup*> getWavesGroups();
		int        getNWaveGroups();

		Grid*      getVisuGrid();
		int        getFrame();
		vector<BreakingWave*> getBreakingWaves();

	/**************** METHODES D'AJOUT DE WAVEGROUP ***************************/
		void addWaveGroup(WaveGroup *waveGroup);
		void addWaveGroups(vector<WaveGroup*> waveGroups);
		void deleteWaveGroup(int index);

		void loadSpectrum(const char* filename);
		void saveSpectrum(const char* filename);

	/***************** METHODES D'AJOUT DE BREAKING WAVE *************/
		void addBreakingWave(BreakingWave* breakingWave);
		void deleteBreakingWave(int index);

		void mergeBreakingWaves();

	/***************** SETTER DE LA GRILLE DE VISU ******************/
		void setVisuGrid(GridOcean *grid);

	/**************** METHODES D'ANIMATION ***************************/
		virtual void animate();

	/**************** METHODES D'AFFICHAGE ***************************/
		virtual void display();

	/**************** METHODES D'EXPORT POUR RENDU ******************/
		void exportData(const char* rep, char filenameOBJ[100], char filenameP[100]);
		void exportMitsuba(qglviewer::Camera *c);

	protected :

		vector<WaveGroup*> m_waveGroups;
		GridOcean* m_visuGrid;
		vector<BreakingWave*> m_breakingWaves;

		Vector4f colorOcean, colorBreakingWaves;

		float dt;

		int frame;
};
#endif
/****************************************************************************/
/****************************************************************************/
