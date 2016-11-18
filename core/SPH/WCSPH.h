/****************************************************************************/
/****************************************************************************/
#ifndef _WCSPH_H__
#define _WCSPH_H__

#include <SESPH.h>

// Weakly Compressible (WC) SPH Solver with Tait's equation to compute pressure
// Low compressible effect

class WCSPH : public SESPH
{
/****************************************************************************/
	public : 

		WCSPH(Vector3f origin, Vector3f min, Vector3f max);
		virtual ~WCSPH();


/****************************************************************************/
	protected : 

		virtual void computeRhoP();
};
#endif
/****************************************************************************/
/****************************************************************************/
