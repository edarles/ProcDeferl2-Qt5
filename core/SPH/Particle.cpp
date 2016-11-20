#include <Particle.h>
/****************************************************************************/
/****************************************************************************/
Particle::Particle()
{
	pos = vel = forces = Vector3f(0,0,0);
	rho = p = mass = radius = 0;
}
/****************************************************************************/
Particle::Particle(Vector3f pos, Vector3f vel, float mass, float radius)
{
	this->pos = pos;
	this->vel = vel;
	this->mass = mass;
	this->radius = radius;
	this->rho = 0;
	this->p = 0;
	vois.clear();
}
/****************************************************************************/
Particle::Particle(Particle &P)
{
	pos = P.pos;
	vel = P.vel;
	forces =  P.forces;
	mass = P.mass;
	radius = P.radius;
	rho = P.rho;
	p = P.p;
	vois.clear();
	for(unsigned int i=0;i<P.vois.size();i++)
		vois.push_back(P.vois[i]);
}
/****************************************************************************/
Particle::~Particle()
{
	vois.clear();
	vois.shrink_to_fit();
	//vois.~vector();
}
/****************************************************************************/
/****************************************************************************/
Vector3f Particle::getPos()
{
	return pos;
}
/****************************************************************************/
Vector3f Particle::getVel()
{
	return vel;
}
/****************************************************************************/
Vector3f Particle::getForces()
{
	return forces;
}
/****************************************************************************/
float Particle::getMass()
{
	return mass;
}
/****************************************************************************/
float Particle::getRadius()
{
	return radius;
}
/****************************************************************************/
float Particle::getRho()
{
	return rho;
}
/****************************************************************************/
float Particle::getP()
{
	return p;
}
/****************************************************************************/
short Particle::getVois(unsigned int index)
{
	return vois[index];
}
/****************************************************************************/
unsigned int Particle::getNbVois()
{
	return vois.size();
}
/****************************************************************************/
/****************************************************************************/
void Particle::setPos(Vector3f pos)
{
	this->pos = pos;
}
/****************************************************************************/
void Particle::setVel(Vector3f vel)
{
	this->vel = vel;
}
/****************************************************************************/
void Particle::setForces(Vector3f forces)
{
	this->forces = forces;
}
/****************************************************************************/
void Particle::setMass(float mass)
{
	this->mass = mass;
}
/****************************************************************************/
void Particle::setRadius(float radius)
{
	this->radius = radius;
}
/****************************************************************************/
void Particle::setRho(float rho)
{
	this->rho = rho;
}
/****************************************************************************/
void Particle::setP(float p)
{
	this->p = p;
}
/****************************************************************************/
void Particle::setVois(short vois)
{
	this->vois.push_back(vois);
}
/****************************************************************************/
void Particle::clearVois()
{
	this->vois.clear();
}
/****************************************************************************/
/****************************************************************************/
