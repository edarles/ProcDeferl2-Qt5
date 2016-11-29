#include <math.h>
#include <WaveGroupMP.h> 
using namespace Eigen;

/****************************************************************************/
/****************************************************************************/
WaveGroupMP::WaveGroupMP():WaveGroup()
{
}
/****************************************************************************/
WaveGroupMP::WaveGroupMP(int n, float k, float theta, float r, float phi0, float zeta):WaveGroup(n,k,theta,r,phi0,zeta)
{
}
/****************************************************************************/
WaveGroupMP::~WaveGroupMP()
{
}
/****************************************************************************/
/****************************************************************************/
void WaveGroupMP::computeMovement(Vector3f pos, float t, Vector3f *dPos, Vector3f *vel, Vector3f *dVel)
{
  	// phase
  	double phi = (cosTheta*pos[0]+sinTheta*pos[2])*k-w*t+phi0;

  	// groupe et profondeur
  	double ekman=1; 
  	double gx = cosTheta*fs*t+g[0];
  	double gz = sinTheta*fs*t+g[2];
  	double gauss = ekman*expf(-(powf((float)(pos[0]-g[0]),2.0)+powf((float)(pos[2]-g[2]),2.0))/(2*extens));

  	// phase instantannée
  	double f_phi = 0.5*powf(1-sin(phi),2)-1;
  	double d_phi = zeta*gauss*f_phi;
  	double d_phiPrim = zeta*gauss*0.5*cos(phi)*(1-sin(phi));
  	double phiTot=phi+d_phi;

  	// amplitude locale
  	double a=r*gauss;

  	(*dPos)[0] = cos(phiTot)*a;
	(*dPos)[1] = sin(phi)*a;
  	(*dPos)[2] = cos(phiTot)*a;

  	(*vel)[0] = w*a*sin(phiTot)*(1+0.5*cos(phiTot)*(1-sin(phiTot)));
  	(*vel)[1] = -w*a*cos(phi);
	(*vel)[2] = w*a*sin(phiTot)*(1+0.5*cos(phiTot)*(1-sin(phiTot)));

  	(*dVel)[0] = (1-k*a*sin(phiTot)*(1+0.5*cos(phiTot)*(1-sin(phiTot))));
  	(*dVel)[1] = (k*a*cos(phi));
	(*dVel)[2] = 0;//(1-k*a*sin(phiTot)*(1+0.5*cos(phiTot)*(1-sin(phiTot))));
}
/****************************************************************************/
/****************************************************************************/
