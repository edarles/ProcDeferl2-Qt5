/****************************************************************************/
/****************************************************************************/
#ifndef _SESPH_H__
#define _SESPH_H__

#include <SPH.h>

// SPH Solver with state equation to compute pressure
// No incompressible -> loi des gaz parfaits utilisée pour calculer la pression

class SESPH : public SPH
{
/****************************************************************************/
	public : 

		SESPH(Vector3f origin, Vector3f min, Vector3f max, float k);
		virtual ~SESPH();

		/**************** GETTERS / SETTERS ***************************/
		float   getK();
		void    setK(float k);
		

/****************************************************************************/
	protected : 

		float k;

		virtual void computeRhoP();
};
#endif
/****************************************************************************/
/****************************************************************************/
