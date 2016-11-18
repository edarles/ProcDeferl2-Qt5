#include <utils.h>
#include <SPH.h>
#include <iostream>
/****************************************************************************/
/****************************************************************************/
GLuint SPH::m_program = 0;
const Vector3f SPH::gravity = Vector3f(0,-9.81,0);
const float SPH::massMaxi = 1000.0;
const float SPH::rho0 = 998.29;
const float SPH::mu = 3.5;
const float SPH::tS = 0.0728;
const float SPH::ltS = 7.065;

/****************************************************************************/
SPH::SPH(Vector3f origin, Vector3f min, Vector3f max)
{
	if(m_program==0) m_program = _compileProgram(vertexShader, spherePixelShader);

	gridSPH = new GridSPH(origin,min,max,0.5,0.5,0.5); 

	posP = new float[4*MAX_PARTICLES];
	colors = new float[4*MAX_PARTICLES];
	particles.clear();
}
/****************************************************************************/
SPH::~SPH()
{
	delete[] posP;
	delete[] colors;
	delete gridSPH; 
	for(int i=0;i<particles.size();i++)
		delete particles[i];

	particles.clear();
	particles.shrink_to_fit();
	particles.~vector();
}

/****************************************************************************/
Particle* SPH::getParticle(int index) const
{
	assert (index < particles.size());
	return particles[index];
}
/****************************************************************************/
vector<Particle*> SPH::getParticles() const
{
	return particles;
}
/****************************************************************************/
void SPH::addParticle(Vector3f pos, Vector3f vel, float mass, float radius)
{
	if(particles.size()<MAX_PARTICLES){
		Particle* P = new Particle();
		P->pos = pos;
		P->vel = vel;
		P->forces = Vector3f(0,0,0);
		P->mass = mass;
		P->radius = radius;
		P->rho = P->p = 0;
		particles.push_back(P);
	}
}
/****************************************************************************/
void SPH::addParticle(Particle *P2)
{
	if(particles.size()<MAX_PARTICLES){
		Particle* P = new Particle();
		P->pos = P2->pos;
		P->vel = P2->vel;
		P->forces = P2->forces;
		P->mass = P2->mass;
		P->radius = P2->radius;
		P->rho = P->p = 0;
		particles.push_back(P);
	}
}
/****************************************************************************/
void SPH::deleteParticle(int index)
{
	delete particles[index];
	particles.erase(particles.begin()+index);
}
/****************************************************************************/
void SPH::generateParticle(Vector3f pos, Vector3f vel, Vector3f dVel, float mass)
{
	//On regarde si on a pas déjà généré une particule dans un rayon h 
	bool trouve = false;
	int i=particles.size()-1;
	float h = powf((60*mass)/(4*M_PI*rho0),0.333);
	Vector3f velP = vel; 
	velP[0] = 2*powf(vel[0]*0.5,2); velP[1] = -powf(vel[1]*0.5,2); velP[2] = powf(vel[2]*0.5,2);
	while(i>=0 && !trouve)
	{
		Vector3f pI_pos = this->particles[i]->pos - pos;
		if(pI_pos.norm()<=h)
			trouve = true;
		else i--;
	}
	// Si on a pas trouvé une particule alors on la génère
	if(!trouve)
		addParticle(pos, velP, mass, h);
	/****************
	Si on a trouvé une particule
	****************/
	else {
		float newMass = this->particles[i]->mass+mass;
		// Si on ne dépasse pas la masse maxi, on ajoute à la particule déjà existante le petit paquet d'eau supplémentaire
		// -> conservation de la quantité de mouvement
		if(newMass<=massMaxi){ 
	  		float invnewmass=1.f/newMass;
	  		this->particles[i]->vel[0] = (this->particles[i]->mass*this->particles[i]->vel[0]+mass*velP[0])*invnewmass;
	  		this->particles[i]->vel[1] = (this->particles[i]->mass*this->particles[i]->vel[1]+mass*velP[1])*invnewmass;
	  		this->particles[i]->vel[2] = (this->particles[i]->mass*this->particles[i]->vel[2]+mass*velP[2])*invnewmass;
	  		this->particles[i]->mass = newMass;
	  		this->particles[i]->radius = powf((60*this->particles[i]->mass)/(4*M_PI*rho0),0.333);
	  	}
		// Sinon on réparti la masse entre les particules existantes
	 	else {
	 		// distribue la masse en nombre de particules de masse au plus massMaxi
		        int nbCreate = ceil(mass/massMaxi);
			float massReparti = mass/nbCreate;
			int j = 0;
			float massToCreate = mass;
			if(particles[i]->vois.size()>0)
			{ 
			   	while(j<particles[i]->vois.size() && j<nbCreate && massToCreate>0){
					int index = particles[i]->vois[j];
					float newMass = this->particles[index]->mass+massReparti;
					massToCreate-=massReparti;
					float invnewmass=1.f/newMass;
	  				//this->particles[index]->vel[0] = (this->particles[index]->mass*this->particles[index]->vel[0]+mass*velP[0])*invnewmass;
	  				//this->particles[index]->vel[1] = (this->particles[index]->mass*this->particles[index]->vel[1]+mass*velP[1])*invnewmass;
	  				//this->particles[index]->vel[2] = (this->particles[index]->mass*this->particles[index]->vel[2]+mass*velP[2])*invnewmass;
	  				this->particles[index]->mass = newMass;
	  				this->particles[index]->radius = powf((60*this->particles[index]->mass)/(4*M_PI*rho0),0.333);
					j++;
			    	}
			}
			// Si on a pas pu distribuer la masse sur les particules existantes
			// On créer une nouvelle particule de masse restante
			if(massToCreate>0){ 
				//cout << "Mass To Create: " << massToCreate << endl;
		        	float hR = powf((60*massToCreate)/(4*M_PI*rho0),0.333);
				float rndcoeff=hR/(RAND_MAX+1.0);
        			float invnv = Q_rsqrt(velP[0]*velP[0]+velP[1]*velP[1]+velP[2]*velP[2]);
	 	 		pos[0] += (rand()*rndcoeff)*vel[0]*invnv;
	  	 		pos[1] += (rand()*rndcoeff)*vel[1]*invnv;
	  	 		pos[2] += (rand()*rndcoeff)*vel[2]*invnv;
				//cout << "Pos " << pos << endl;
	  	 		addParticle(pos, velP, massToCreate, hR);
			}
		}
	}
}
/****************************************************************************/
/****************************************************************************/
void SPH::constraintGridSPH()
{
	if(particles.size()>0){
	Vector3f min = Vector3f(INF,INF,INF); 
	Vector3f max = Vector3f(-INF,-INF,-INF);
	Vector3f min0 = gridSPH->getMin();
	Vector3f max0 = gridSPH->getMax();
	Vector3f s = Vector3f(1,1,1);
	float rMax = -INF;
	Vector3f barycenter(0,0,0);
	for(int i=0;i<particles.size();i++)
	{
		Vector3f pt = gridSPH->getLocalRotated(particles[i]->pos);
		barycenter+= pt;
		rMax = fmax(rMax,particles[i]->radius);
	}
	barycenter/=particles.size();
	gridSPH->translate(barycenter-gridSPH->getCenter());

	for(int i=0;i<particles.size();i++)
	{
		Vector3f pt = gridSPH->getLocalRotated(particles[i]->pos);
		
		min[0] = fmin(min[0],pt[0]-rMax);
		min[1] = fmin(min[1],pt[1]-rMax);
		min[2] = fmin(min[2],pt[2]-rMax);

		max[0] = fmax(max[0],pt[0]+rMax);
		max[1] = fmax(max[1],pt[1]+rMax);
		max[2] = fmax(max[2],pt[2]+rMax);
	}
	if(max0[0]-min0[0]!=0) s[0] = fabs((max[0]-min[0])/(max0[0]-min0[0]));
	if(max0[1]-min0[1]!=0) s[1] = fabs((max[1]-min[1])/(max0[1]-min0[1]));
	if(max0[2]-min0[2]!=0) s[2] = fabs((max[2]-min[2])/(max0[2]-min0[2]));
	gridSPH->scale(s);

	}	
}
/****************************************************************************/
/****************************************************************************/
void SPH::merge(SPH* other)
{
	//std::cout << "debut merge " << std::endl;
	// On ajoute les particules de l'autre solveur dans le solveur
	for(int i=0;i<other->particles.size();i++){
		Vector3f posP = other->particles[i]->pos;
		float h = other->particles[i]->radius;
		int j = particles.size()-1;
		bool trouve = false;
		while(j>=0 && !trouve)
		{
			Vector3f pI_pos = this->particles[j]->pos - posP;
			if(pI_pos.norm()==0)
				trouve = true;
			else j--;
		}
		// Si on a pas trouvé une particule alors on peut l'ajouter
		if(!trouve)
			addParticle(other->particles[i]);
	}
	// On merge la grille de voisinage avec la grille de voisinage de l'autre solveur
	gridSPH->merge(other->gridSPH);
	//std::cout << "Fin Merge solver" << std::endl;	
}
/****************************************************************************/
/****************************************************************************/
void SPH::update(vector<WaveGroup*> waveGroups, float time, float dt)
{
	//std::cout << "debut update " << std::endl;
	if(particles.size()>0){
		// On contraint la grille pour que toutes les particules puissent y etre présente
		constraintGridSPH();

		// Boucle de simulation
	//	std::cout << "neigh " << std::endl;
		if(computeNeighborhood()){
	//	std::cout << "dens P " << std::endl;
			computeRhoP();
	//	std::cout << "forces " << std::endl;
			computeForces();
	//	std::cout << "integrate " << std::endl;
			integrate(dt);
	//	std::cout << "collide " << std::endl;
			collideExterior(waveGroups,time);
		}
	}
	//std::cout << "fin update " << std::endl;
}
/****************************************************************************/
bool SPH::computeNeighborhood()
{
	if(gridSPH)
	{
		gridSPH->computeNeighborhood(particles);
		return true;
	}
	return false;
}
/****************************************************************************/
void SPH::computeForces()
{
	for(int i=0;i<particles.size();i++)
	{
		Vector3f fP(0,0,0),fV(0,0,0),N(0,0,0);
		float fS = 0;

		Vector3f pos1 = particles[i]->pos;
		Vector3f vel1 = particles[i]->vel;
		float rho1 = particles[i]->rho;
		float p1 = particles[i]->p;
		float h1 = particles[i]->radius;

		for(int j=0;j<particles[i]->vois.size();j++){
			int index = particles[i]->vois[j];
			Vector3f pos2 = particles[index]->pos;
			float d = (pos1-pos2).norm();

			if(d>0) {
				float rho2 = particles[index]->rho;
				float p2 = particles[index]->p;
				float h2 = particles[index]->radius;
				float m2 = particles[index]->mass;
			        Vector3f vel2 = particles[index]->vel;
				// pas bon...
                		float h = (h1+h2)/2;//max(h1,h2);///2;
                
				// Todo : faire plutot (grad_Wij(rij,hi) + grad_Wij(rij,hj))/2
				// Pressure force
				float WP = (-45*pow(h-d,2)/(M_PI*powf(h,6)))*((p1/(rho1*rho1))+(p2/(rho2*rho2)))*m2;
				fP += ((pos1-pos2)/d)*WP;
		
				// Todo : faire plutot (lapl_Wij(rij,hi) + lapl_Wij(rij,hj))/2
                		// Viscosity force
				float WV  = mu*(45*(h-d)/(M_PI*pow(h,6)))*m2/rho2;
				fV += (vel2-vel1)*WV;
			
				// Todo : idem
				// Surface tension force
				float WS = -(945/(32*M_PI*powf(h,9)))*((h*h)-(d*d))*((3*h*h)-(7*d*d));
				fS += WS*(m2/rho2);
	
				// Todo : idem
				// normale evaluation
				float WN = (-945/(32*M_PI*powf(h,9)))*(m2/rho2)*powf(h*h-d*d,2);
				N += (pos1-pos2)*WN;
			}
		}
		//cout << "fP: " << -fP[0]/rho1 << " " << -fP[1]/rho1 << " " << -fP[2]/rho1 << endl;
		//cout << "fV: " << fV[0] << " " << fV[1] << " " << fV[2] << endl;
		particles[i]->forces = (-fP/rho1) + fV + gravity*rho1; 
		float lN = N.norm();
		if(lN>=ltS)
			particles[i]->forces += -fS*tS*N/lN;
	}
}
/****************************************************************************/
void SPH::integrate(float dt)
{
	for(int i=0;i<particles.size();i++)
    	{
      		if(particles[i]->rho!=0)
		{
	  		// Integration numérique => Euler simplectique
	  		particles[i]->vel = particles[i]->vel+(particles[i]->forces/particles[i]->rho)*dt;
	  		particles[i]->pos = particles[i]->pos+particles[i]->vel*dt;
			posP[i*4] = particles[i]->pos[0]; posP[i*4+1] = particles[i]->pos[1]; posP[i*4+2] = particles[i]->pos[2]; posP[i*4+3] = particles[i]->radius;
		}
	}
}
/****************************************************************************/
void SPH::collideExterior(vector<WaveGroup*> waveGroups, float time)
{
	// On supprime les particules du solveur quand elles passent en dessous de la surface
	for(int i=0;i<particles.size();i++)
	{
		Vector3f pos2 = particles[i]->pos;
		Vector3f pos = Vector3f(pos2[0],0,pos2[2]);
		Vector3f dPos(0,0,0);
		for(int j=0;j<waveGroups.size();j++){
			Vector3f dP, vel, dVel;
			waveGroups[j]->computeMovement(pos, time, &dP, &vel, &dVel);
			dP[0]*=waveGroups[j]->getCosTheta(); dP[2]*=waveGroups[j]->getSinTheta(); 
			dPos+=dP;
		}
		if(pos2[1] + particles[i]->radius < dPos[1])	
			deleteParticle(i);
	}
}
/****************************************************************************/
void SPH::display()
{
	//if(particles.size()>0){
		// Affichage de la grille
		//gridSPH->display();

		// Affichage des particules
		displayParticlesByField(1);
		displaySpheres(m_program,posP,colors,particles.size());
	//}
}
/****************************************************************************/
/****************************************************************************/
void SPH::displayParticlesByField(int field)
{
    // DISPLAY BY MASS FIELD
    if(field==0){
    	
	float mmax, mmin;
	mmax = massMaxi; mmin = 0;
	/*for(uint i=0;i<particles.size();i++){
		float m = particles[i]->mass;
		//if(mmax<m) mmax = m;
		if(mmin>m) mmin = m;
	}*/
	#pragma omp parallel for
    	for(uint i=0;i<particles.size();i++){
		float m = particles[i]->mass;
		float hue = 240 * (mmax - fmin(m,mmax)) / (mmax-mmin);
		Vector3d Hsv(hue,1,1);
		Vector3d Rgb = convertHsvToRgb(Hsv);
		colors[i*4] = Rgb[0];
		colors[i*4+1] = Rgb[1];
		colors[i*4+2] = Rgb[2];
		colors[i*4+3] = 0.9;
    	}
   }
   //DISPLAY BY VELOCITY FIELD
   if(field==1){
	float vmax, vmin;
	vmax = 0; vmin = 1000000;
	for(uint i=0;i<particles.size();i++){
		float length = particles[i]->vel.norm();
		if(vmax<length) vmax = length;
		if(vmin>length) vmin = length;
	}
	#pragma omp parallel for
    	for(uint i=0;i<particles.size();i++){
		float length = particles[i]->vel.norm();
		float hue = 240 * (vmax - fmin(length,vmax)) / (vmax-vmin);
		Vector3d Hsv(hue,1,1);
		Vector3d Rgb = convertHsvToRgb(Hsv);
		colors[i*4] = Rgb[0];
		colors[i*4+1] = Rgb[1];
		colors[i*4+2] = Rgb[2];
		colors[i*4+3] = 0.9;
    	}
   }
}
/****************************************************************************/
/****************************************************************************/
Vector3d SPH::convertHsvToRgb(Vector3d Hsv)
{
   Vector3d result;
   int t = (int)(floor(Hsv[0]/60))%6;
   double f = (Hsv[0]/60)-t;
   double l = Hsv[2]*(1-Hsv[1]);
   double m = Hsv[2]*(1-f*Hsv[1]);
   double n = Hsv[2]*(1-(1-f)*Hsv[1]);
   if(t==0) {result[0] = Hsv[2]; result[1] = n; result[2] = l;};
   if(t==1) {result[0] = m; result[1] = Hsv[2]; result[2] = l;};
   if(t==2) {result[0] = l; result[1] = Hsv[2]; result[2] = n;};
   if(t==3) {result[0] = l; result[1] = m; result[2] = Hsv[2];};
   if(t==4) {result[0] = n; result[1] = l; result[2] = Hsv[2];};
   if(t==5) {result[0] = Hsv[2]; result[1] = l; result[2] = m;};
   return result;
}
/****************************************************************************/
void SPH::exportParticlesHoudini(const char* filename)
{
	FILE *f = fopen(filename,"a");
	if(f){	
		fprintf(f,"houdini\n");
		fprintf(f,"%lu\n",particles.size());
		fprintf(f,"32 32 32\n");
		for (int i=0; i<particles.size(); i++) 
			fprintf(f,"%d %f %f %f %f %f %f 0\n",i,particles[i]->pos[0],particles[i]->pos[1],particles[i]->pos[2],
			        particles[i]->vel[0],particles[i]->vel[1],particles[i]->vel[2]);
	}
}
/****************************************************************************/
/****************************************************************************/
const float SPH::getRho0()
{
	return rho0;
}
/****************************************************************************/
const float SPH::getMu()
{
	return mu;
}
/****************************************************************************/
const float SPH::getTS()
{
	return tS;
}
/****************************************************************************/
const float SPH::getLTS()
{
	return ltS;
}
/****************************************************************************/
int   SPH::getNbParticles() const
{
	return particles.size();
}
/****************************************************************************/
GridSPH* SPH::getGrid() const
{
	return gridSPH;
}
/****************************************************************************/
/****************************************************************************/





