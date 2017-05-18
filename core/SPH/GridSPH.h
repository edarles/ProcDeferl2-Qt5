/****************************************************************************/
/****************************************************************************/
#ifndef _GRID_SPH_H__
#define _GRID_SPH_H__

#include <Grid.h>
#include <SPHParticle.h>

class GridSPH : public Grid
{
/****************************************************************************/
	public : 

		GridSPH();
		GridSPH(Vector3f min, Vector3f max, float dx, float dy, float dz);
		virtual ~GridSPH();

		/**************** ROUTINE DE CALCUL DE VOISINAGE *******/
		void computeNeighborhood(vector<SPHParticle*> particles);

};
#endif
/****************************************************************************/
/****************************************************************************/
