#include <SprayParticle.h>
#include <utils.h>
/****************************************************************************/
/****************************************************************************/
SprayParticle::SprayParticle():Particle()
{
	lifetime = LIFETIME_MAX;
	lifetimeInit = lifetime;
	color = Vector3f(1,1,1);
}
/****************************************************************************/
SprayParticle::SprayParticle(Vector3f pos, Vector3f vel, float mass):Particle(pos,vel,mass)
{
	color = Vector3f(1,1,1);
	this->lifetime = (rand()/(double)RAND_MAX)*LIFETIME_MAX;
	this->lifetimeInit = this->lifetime;
}
/****************************************************************************/
SprayParticle::SprayParticle(const SprayParticle &P)
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
void 	 SprayParticle::setLifeTime(float lifetime)
{
	this->lifetime = lifetime;
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