#include <utils.h>
#include <Sprays.h>
#include <iostream>

/****************************************************************************/
/****************************************************************************/
Sprays::Sprays()
{
	particles.clear();
	srand(time(NULL));
}
/****************************************************************************/
Sprays::~Sprays()
{
	for(unsigned int i=0;i<particles.size();i++)
		delete particles[i];

	particles.clear();
	vector<SprayParticle*>().swap(particles);
	particles.shrink_to_fit();
}
/****************************************************************************/
/****************************************************************************/	
SprayParticle* Sprays::getParticle(int index) const
{
	assert(index < (int)particles.size());
	return particles[index];
}
/****************************************************************************/
vector<SprayParticle*>  Sprays::getParticles() const
{
	return particles;
}
/****************************************************************************/
int Sprays::getNbParticles() const
{
	return particles.size();
}
/****************************************************************************/
/****************************************************************************/
void Sprays::addParticle(Vector3f pos, Vector3f vel, float mass)
{
	SprayParticle *P = new SprayParticle(pos,vel,mass);
	particles.push_back(P);
}
/****************************************************************************/
void Sprays::addParticle(SprayParticle *p)
{
	SprayParticle *P = new SprayParticle((*p));
	particles.push_back(P);
}
/****************************************************************************/
void Sprays::deleteParticle(int index)
{
	delete particles[index];
	particles.erase(particles.begin()+index);
	//particles.shrink_to_fit();
}
/****************************************************************************/
/****************************************************************************/
void Sprays::generate(Vector3f pos, Vector3f vel, float mass, float h, int nb)
{
	Vector3f SPos, SVel;
	float hmin = -10*h; float hmax = 10*h;
	for(int i=0;i<nb;i++){
		SPos[0] = pos[0]+(rand()/(double)RAND_MAX)*(hmax-hmin)+hmin;
		SPos[1] = pos[1];//+(rand()/(double)RAND_MAX)*(hmax-hmin)+hmin;
		SPos[2] = pos[2]+(rand()/(double)RAND_MAX)*(hmax-hmin)+hmin;
		SVel[0] = vel[0];
		SVel[1] = vel[1];
		SVel[2] = vel[2];
		addParticle(SPos, SVel, mass);
	}
}
/****************************************************************************/
/****************************************************************************/
void Sprays::merge(Sprays* other)
{
	for(int i=0; other->particles.size();i++){
		addParticle(other->particles[i]);
		other->deleteParticle(i);
	}
}
/****************************************************************************/
/****************************************************************************/
void Sprays::update(float dt)
{
	Vector3f wind;
	float Amin = -100; float Amax = 100;
	// Simple gravity action -> rajouter un vent aléatoire
	for(unsigned int i=0; i<particles.size(); i++){
		
		wind = Vector3f((rand()/(double)RAND_MAX)*(Amax-Amin)+Amin,(rand()/(double)RAND_MAX)*(Amax-Amin)+Amin,(rand()/(double)RAND_MAX)*(Amax-Amin)+Amin);
		//cout << "Wind " << wind << endl;
		particles[i]->setForces(Vector3f(0,0,0));
		particles[i]->addForce(gravity*particles[i]->getMass());
		particles[i]->addForce(wind*particles[i]->getMass());
		particles[i]->integrate(dt);

		// Suppression de la particule de sprays si sa vitesse horizontale devient négative 
		// ou si durée de vie est inférieure ou égal à 0
		if(particles[i]->getVel()[1]<0 || particles[i]->getLifeTime()<=0)
			deleteParticle(i);
	}
}
/****************************************************************************/
/****************************************************************************/
void Sprays::display()
{
        glDisable(GL_LIGHTING);
	glPointSize(2.0);
	glBegin(GL_POINTS);
	for(unsigned int i=0; i<particles.size(); i++){
		float opacity = 1-((particles[i]->getLifeTimeInit()-particles[i]->getLifeTime())/particles[i]->getLifeTimeInit());
		glColor4f(1.0,1.0,1.0,opacity);
		Vector3f pos = particles[i]->getPos();
		glVertex3f(pos[0],pos[1],pos[2]);
	}
	glEnd();
	glPointSize(1.0);
}
/****************************************************************************/
/****************************************************************************/
//void Sprays::exportParticlesHoudini(const char* filename)
//{
	// Todo
//}
/****************************************************************************/
/****************************************************************************/
