#include <SprayParticle.h>
/****************************************************************************/
/****************************************************************************/
SprayParticle::SprayParticle()
{
	pos = vel = forces = Vector3f(0,0,0);
	mass =  0;
	lifetime = 100;
	lifetimeInit = lifetime;
}
/****************************************************************************/
SprayParticle::SprayParticle(Vector3f pos, Vector3f vel, float mass)
{
	this->pos = pos;
	this->vel = vel;
	this->forces = Vector3f(0,0,0);
	this->mass =  mass;
	this->lifetime = (rand()/(double)RAND_MAX)*100;
	this->lifetimeInit = this->lifetime;
}
/****************************************************************************/
SprayParticle::SprayParticle(SprayParticle &P)
{
	this->pos = P.pos;
	this->vel = P.vel;
	this->forces = P.forces;
	this->mass = P.mass;
	this->lifetime = P.lifetime;
	this->lifetimeInit = P.lifetimeInit;
}
/****************************************************************************/
SprayParticle::~SprayParticle()
{
}
/****************************************************************************/
/****************************************************************************/
void SprayParticle::integrate(float dt)
{
	this->vel += (this->forces/mass)*dt;
	this->pos += this->vel*dt;
	this->lifetime--;
}
/****************************************************************************/
/****************************************************************************/
Vector3f SprayParticle::getPos()
{
	return pos;
}
/****************************************************************************/
Vector3f SprayParticle::getVel()
{
	return vel;
}
/****************************************************************************/
Vector3f SprayParticle::getForces()
{
	return forces;
}
/****************************************************************************/
float 	 SprayParticle::getMass()
{
	return mass;
}
/****************************************************************************/
float 	 SprayParticle::getLifeTime()
{
	return lifetime;
}
/****************************************************************************/
float 	 SprayParticle::getLifeTimeInit()
{
	return lifetimeInit;
}
/****************************************************************************/
/****************************************************************************/		
void     SprayParticle::setPos(Vector3f pos)
{
	this->pos = pos;
}
/****************************************************************************/
void     SprayParticle::setVel(Vector3f vel)
{
	this->vel = vel;
}
/****************************************************************************/
void     SprayParticle::setForces(Vector3f forces)
{
	this->forces = forces;
}
/****************************************************************************/
void 	 SprayParticle::setMass(float mass)
{
	this->mass = mass;
}
/****************************************************************************/
void 	 SprayParticle::setLifeTime(float lifetime)
{
	this->lifetime = lifetime;
}
/****************************************************************************/
void     SprayParticle::addForce(Vector3f force)
{
	this->forces += force;
}
/****************************************************************************/
/****************************************************************************/
