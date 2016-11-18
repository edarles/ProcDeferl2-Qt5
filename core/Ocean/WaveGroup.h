/****************************************************************************/
/****************************************************************************/
#ifndef _WAVE_GROUP_H__
#define _WAVE_GROUP_H__

#include <stdio.h>

#include <Eigen/Dense>
using namespace Eigen;

class WaveGroup
{
/****************************************************************************/
	public : 

		WaveGroup();
		WaveGroup(const WaveGroup& wg);
		WaveGroup(int n, float k, float theta, float r, float phi0, float zeta);
		virtual ~WaveGroup();

		void initOtherParams();

	/**************** GETTERS AND SETTERS ***************************/
		float getK();
		float getLambda();
		float getTheta();
		float getCosTheta();
		float getSinTheta();
		float getACosTheta();
		float getASinTheta();
		float getR();
		float getPhi0();
		float getZeta();
		float getExtens();
		float getPs();
		Vector3f getG();
		int   getN();
		
		void setK(float k);
		void setTheta(float theta);
		void setR(float r);
		void setPhi0(float phi0);
		void setZeta(float zeta);
		void setN(int n);
		
	/**************** CHARGEMENT ET SAUVEGARDE DEPUIS UN FICHIER ****************/
		virtual  void load(FILE *f);
		virtual	 void save(FILE *f);

	/**************** METHODE DE CALCUL DU MOUVEMENT ***************************/
		virtual void computeMovement(Vector3f pos, float t, 
                                     Vector3f *dPos, // delta position
                                     Vector3f *vel, // vitesse fluide
                                     Vector3f *dVel);  // pentes surface isobarique)

		virtual void update();

		void print();

	protected:

		float k, lambda, w, theta, ps, r, phi0, zeta, fs, extens;
		Vector3f g;
		int n;

		float cosTheta, sinTheta;
		float acosTheta, asinTheta;
};
#endif
/****************************************************************************/
/****************************************************************************/
