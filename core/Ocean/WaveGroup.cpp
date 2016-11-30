#include <math.h>
#include <time.h> 
#include <WaveGroup.h> 

using namespace Eigen;

/****************************************************************************/
/****************************************************************************/
WaveGroup::WaveGroup()
{
  	// onde
  	k = 2*M_PI/13.0; // nombre d'onde : 2pi/lambda
	lambda = 2*M_PI/k;
  	theta = 0.0; // direction du vecteur d'onde kx=k.cos(theta), ky=k.sin(theta)
  	r = 1.1/k; // rayon des plus grosses vagues au centre du groupe (r.k==1 est la cambrure limite du déferlement)
  	phi0 = 0; // phase initiale de l'onde
  	zeta = M_PI/3.0; // phase instantannée maximale (-zeta<dphiw<zeta)
  	n = 5; // nombre de vagues sous l'enveloppe

	initOtherParams();

}
/****************************************************************************/
WaveGroup::WaveGroup(const WaveGroup& wg)
{
	this->n = wg.n;
	this->k = wg.k;
	this->lambda = 2*M_PI/this->k;
	this->theta = wg.theta;
	this->r = wg.r;
	this->phi0 = wg.phi0;
	this->zeta = wg.zeta;
	initOtherParams();
}
/****************************************************************************/
WaveGroup::WaveGroup(int n, float k, float theta, float r, float phi0, float zeta)
{
	this->n = n;
	this->k = k;
	this->lambda = 2*M_PI/this->k;
	this->theta = theta;
	this->r = r;
	this->phi0 = phi0;
	this->zeta = zeta;

	initOtherParams();
}
/****************************************************************************/
WaveGroup::~WaveGroup()
{
}
/****************************************************************************/
/****************************************************************************/
void WaveGroup::initOtherParams()
{
	cosTheta = cos(theta);
	sinTheta = sin(theta);

	acosTheta = acos(theta);
	asinTheta = asin(theta);

	// update others
	this->w = sqrt(9.81*k);

	// g -> position initiale du groupe
	this->g[0] = 0;// -50*cosTheta;
	this->g[1] = 0;
	this->g[2] = 0;// -50*sinTheta;

	this->ps = w/k;   //vitesse de phase
	this->fs = w/(2*k); // vitesse de l'enveloppe
	this->extens = 0.5*M_PI*powf(n/k,2);
}
/****************************************************************************/
/****************************************************************************/
float WaveGroup::getK()
{
	return this->k;
}
/****************************************************************************/
float WaveGroup::getLambda()
{
	return this->lambda;
}
/****************************************************************************/
float WaveGroup::getTheta()
{
	return this->theta;
}
/****************************************************************************/
float WaveGroup::getCosTheta()
{
	return this->cosTheta;
}
/****************************************************************************/
float WaveGroup::getSinTheta()
{
	return this->sinTheta;
}
/****************************************************************************/
float WaveGroup::getACosTheta()
{
	return this->acosTheta;
}
/****************************************************************************/
float WaveGroup::getASinTheta()
{
	return this->asinTheta;
}
/****************************************************************************/
float WaveGroup::getR()
{
	return this->r;
}
/****************************************************************************/
float WaveGroup::getPhi0()
{
	return this->phi0;
}
/****************************************************************************/
float WaveGroup::getZeta()
{
	return this->zeta;
}
/****************************************************************************/
float WaveGroup::getExtens()
{
	return this->extens;
}
/****************************************************************************/
float WaveGroup::getPs()
{
	return this->ps;
}
/****************************************************************************/
Vector3f WaveGroup::getG()
{
	return this->g;
}
/****************************************************************************/
int WaveGroup::getN()
{
	return this->n;
}
/****************************************************************************/
/****************************************************************************/		
void WaveGroup::setK(float k)
{
	this->k = k;
}
/****************************************************************************/
void WaveGroup::setTheta(float theta)
{
	this->theta = theta;
}
/****************************************************************************/
void WaveGroup::setR(float r)
{
	this->r = r;
}
/****************************************************************************/
void WaveGroup::setPhi0(float phi0)
{
	this->phi0 = phi0;
}
/****************************************************************************/
void WaveGroup::setZeta(float zeta)
{
	this->zeta = zeta;
}
/****************************************************************************/
void WaveGroup::setN(int n)
{
	this->n = n;
}
/****************************************************************************/
/****************************************************************************/
void WaveGroup::print()
{
	printf("%d %f %f %f %f %f\n",getN(), getK(), getTheta(), getR(), getPhi0(), getZeta());
}
/****************************************************************************/
/****************************************************************************/
void WaveGroup::load(FILE *f)
{
	fscanf(f,"%d %f %f %f %f %f\n",&n,&k,&theta,&r,&phi0,&zeta);
	initOtherParams();
}
/****************************************************************************/
void WaveGroup::save(FILE *f)
{
	fprintf(f,"%d %f %f %f %f %f\n",getN(), getK(), getTheta(), getR(), getPhi0(), getZeta());
}
/****************************************************************************/
/****************************************************************************/
void WaveGroup::update()
{
	// RIEN A FAIRE ICI
}
/****************************************************************************/
/****************************************************************************/

