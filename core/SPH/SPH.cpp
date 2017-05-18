#include <utils.h>
#include <SPH.h>
#include <iostream>
/****************************************************************************/
/****************************************************************************/
const float SPH::massMaxi = 1000.0;
const float SPH::rho0 = 998.29;
const float SPH::mu = 3.5;
const float SPH::tS = 0.0728;
const float SPH::ltS = 7.065;

/****************************************************************************/
SPH::SPH(Vector3f origin, Vector3f min, Vector3f max)
{
	gridSPH = new GridSPH(min,max,0.5,0.5,0.5); 
	//gridSPH->setCenter(origin);
	totalMass = 0;
	particles.clear();
	srand(time(NULL));
}
/****************************************************************************/
SPH::~SPH()
{
	cout << "delete breaking waves" << endl;
	delete gridSPH; 
	for(unsigned int i=0;i<particles.size();i++)
		delete particles[i];
	particles.clear();
	vector<SPHParticle*>().swap(particles);
	particles.shrink_to_fit();
}
/****************************************************************************/
void SPH::addParticle(Vector3f pos, Vector3f vel, float mass, float radius)
{
	if(particles.size()<MAX_PARTICLES){
		SPHParticle *P = new SPHParticle(pos,vel,mass,radius);
		particles.push_back(P);
		totalMass += mass;
	}
}
/****************************************************************************/
void SPH::addParticle(SPHParticle *P2)
{
	if(particles.size()<MAX_PARTICLES){
		SPHParticle *P = new SPHParticle((*P2));
		particles.push_back(P);
		totalMass += P2->getMass();
	}
}
/****************************************************************************/
void SPH::deleteParticle(int index)
{
	totalMass -= particles[index]->getMass();
	delete particles[index];
	particles.erase(particles.begin()+index);
	particles.shrink_to_fit();
}
/****************************************************************************/
void SPH::generateParticle(Vector3f pos, Vector3f vel, float mass)
{
	float massToCreate = mass;

	//On regarde si on a pas déjà généré une particule dans un rayon h 
	bool trouve = false;
	int i= particles.size()-1;
	float h = powf((60*mass)/(4*M_PI*rho0),0.333);
	Vector3f velP = vel; 
	velP[0] = 1.0*powf(vel[0]*0.5,2); velP[1] = -powf(vel[1]*0.5,2); velP[2] = powf(vel[2]*0.5,2);
	while(i>=0 && !trouve)
	{
		Vector3f pI_pos = this->particles[i]->getPos() - pos;
		if(pI_pos.norm()<=h)
			trouve = true;
		else i--;
	}
	// Si on a pas trouvé une particule alors on la génère
	if(!trouve){
		addParticle(pos, velP, mass, h);
		massToCreate -= mass;
	}
	/****************
	Si on a trouvé une particule
	****************/
	else {
		//cout << "trouve i: " << i << " m: " << this->particles[i]->getMass() << endl;
		float newMass = this->particles[i]->getMass()+mass;
		// Si on ne dépasse pas la masse maxi, on ajoute à la particule déjà existante le petit paquet d'eau supplémentaire
		// -> conservation de la quantité de mouvement
		if(newMass<=massMaxi){ 
	  		float invnewmass=1.f/newMass;
			massToCreate -= mass;
			Vector3f vel((this->particles[i]->getMass()*this->particles[i]->getVel()[0]+mass*velP[0])*invnewmass,
				     (this->particles[i]->getMass()*this->particles[i]->getVel()[1]+mass*velP[1])*invnewmass,
				     (this->particles[i]->getMass()*this->particles[i]->getVel()[2]+mass*velP[2])*invnewmass);
	  		this->particles[i]->setVel(vel);
	  		this->particles[i]->setMass(newMass);
	  		this->particles[i]->setRadius(powf((60*this->particles[i]->getMass())/(4*M_PI*rho0),0.333));
			totalMass += mass;
	  	}
	}
	/****************
	 Si on a pas pu distribuer la masse sur les particules existantes
	 On créer une nouvelle particule de masse restante
	****************/
	if(massToCreate>0){ 
		//cout << "Mass To Create: " << massToCreate << endl;
		float hR = powf((60*massToCreate)/(4*M_PI*rho0),0.333);
		float rndcoeff=hR/(RAND_MAX+1.0);
        	float invnv = Q_rsqrt(velP[0]*velP[0]+velP[1]*velP[1]+velP[2]*velP[2]);
		Vector3f velR = gridSPH->getLocalRotated(vel);
	 	pos[0] += (rand()*rndcoeff)*velR[0]*invnv;
	  	pos[1] += (rand()*rndcoeff)*velR[1]*invnv;
	  	pos[2] += (rand()*rndcoeff)*velR[2]*invnv;
		//cout << "Pos " << pos << endl;
	  	addParticle(pos, velP, massToCreate, hR);
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
	int nb = 0;
	for(unsigned int i=0;i<particles.size();i++)
	{
		Vector3f pt = gridSPH->getLocalRotated(particles[i]->getPos());
		if(!isnan(pt[0]) && !isnan(pt[1]) && !isnan(pt[2])) {
			barycenter+= pt;
			rMax = fmax(rMax,particles[i]->getRadius());
			nb++;
		}
	}
	if(nb>0){
		barycenter/=nb;
		gridSPH->translate(barycenter-gridSPH->getCenter());
        rMax*=2;
	
		min0 = gridSPH->getMin();
		max0 = gridSPH->getMax();
		for(unsigned int i=0;i<particles.size();i++)
		{		
			Vector3f pt = gridSPH->getLocalRotated(particles[i]->getPos());
			if(!isnan(pt[0]) && !isnan(pt[1]) && !isnan(pt[2])) {
		
			min[0] = fmin(min[0],pt[0]-rMax);
			min[1] = fmin(min[1],pt[1]-rMax);
			min[2] = fmin(min[2],pt[2]-rMax);

			max[0] = fmax(max[0],pt[0]+rMax);
			max[1] = fmax(max[1],pt[1]+rMax);
			max[2] = fmax(max[2],pt[2]+rMax);
			}
		
		}
		if(max0[0]-min0[0]!=0) s[0] = (float)fabs((double)((max[0]-min[0])/(max0[0]-min0[0])));
		if(max0[1]-min0[1]!=0) s[1] = (float)fabs((double)((max[1]-min[1])/(max0[1]-min0[1])));
		if(max0[2]-min0[2]!=0) s[2] = (float)fabs((double)((max[2]-min[2])/(max0[2]-min0[2])));
		//cout << "scale: " << s[0] << " " << s[1] << " " << s[2] << endl;
		gridSPH->scale(s);
	}
  }	
}
/****************************************************************************/
/****************************************************************************/
void SPH::merge(SPH* other)
{
	// On ajoute les particules de l'autre solveur dans le solveur
	for(unsigned int i=0;i<other->particles.size();i++){
		Vector3f posP = other->particles[i]->getPos();
		float h = other->particles[i]->getRadius();
		int j = particles.size()-1;
		bool trouve = false;
		while(j>=0 && !trouve)
		{
			Vector3f pI_pos = this->particles[j]->getPos() - posP;
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
}
/****************************************************************************/
/****************************************************************************/
void SPH::update(vector<WaveGroup*> waveGroups, float time, float dt, GridOcean* ocean)
{
	if(particles.size()>0){
		// On contraint la grille pour que toutes les particules puissent y etre présente
		constraintGridSPH();

		// Boucle de simulation
		if(computeNeighborhood()){
			computeRhoP();
			computeForces();
			integrate(dt);
			generateBubblesSprays(waveGroups,time,ocean);
			displayParticlesByField(1);
		}
	}
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
	//#pragma omp parallel for
	for(unsigned int i=0;i<particles.size();i++)
	{
		Vector3f fP(0,0,0),fV(0,0,0),N(0,0,0);
		float fS = 0;

		Vector3f pos1 = particles[i]->getPos();
		Vector3f vel1 = particles[i]->getVel();
		float rho1 = particles[i]->getRho();
		float p1 = particles[i]->getP();
		float h1 = particles[i]->getRadius();
		float pV1 = p1/(rho1*rho1);

		for(unsigned int j=0;j<particles[i]->getNbVois();j++){
			int index = particles[i]->getVois(j);
			Vector3f pos2 = particles[index]->getPos();
			Vector3f P1P2 = pos1 - pos2;
			float d = P1P2.norm();

			if(d>0) {
				float rho2 = particles[index]->getRho();
				float p2 = particles[index]->getP();
				float h2 = particles[index]->getRadius();
				float m2 = particles[index]->getMass();
				float pV2 = p2/(rho2*rho2);
			        Vector3f vel2 = particles[index]->getVel();
				// pas bon...
                		float h = (h1+h2)/2;
                
				// Pressure force
				Vector3f WP = (-45*pow(h-d,2)/(M_PI*powf(h,6)))*(P1P2/d);
				fP += m2*(pV1+pV2)*WP;
		
                		// Viscosity force
				float WV  = mu*(45*(h-d)/(M_PI*pow(h,6)))*m2/rho2;
				fV += (vel2-vel1)*WV;
			
				// Surface tension force
				float WS = -(945/(32*M_PI*powf(h,9)))*((h*h)-(d*d))*((3*h*h)-(7*d*d));
				fS += WS*(m2/rho2);
	
				// normale evaluation
				float WN = (-945/(32*M_PI*powf(h,9)))*(m2/rho2)*powf(h*h-d*d,2);
				N += (pos1-pos2)*WN;
			}
		}
		Vector3f forces = (-fP/rho1) + fV + gravity*rho1; 
		float lN = N.norm();
		if(lN>=ltS)
			forces += -fS*tS*N/lN;
		particles[i]->setForces(forces);
		//particles[i]->clearVois();
	}
}
/****************************************************************************/
void SPH::integrate(float dt)
{
	for(unsigned int i=0;i<particles.size();i++)
			particles[i]->integrate(dt);
}
/****************************************************************************/
void SPH::generateBubblesSprays(vector<WaveGroup*> waveGroups, float time, GridOcean* ocean)
{
	// On gère ici l'interaction particule / surface libre
  
	float lifeTimeMax = 200;
	float lifeTimeMin = 100;
	Vector3f dP, v, dV;

	QImage tex = ocean->getTexBubbles();

	for(unsigned int i=0;i<particles.size();i++) {
	  Vector3f posPart = particles[i]->getPos(); // position de la particule repère global
	  Vector3f velPart = particles[i]->getVel(); // vitesse de la particule repère global
	  velPart[1]=0; velPart.normalize(); // on se place dans le plan horizontal

	  // recherche de la surface libre
	  // initialisation
	  Vector3f p1 = Vector3f(posPart[0],0,posPart[2]);
	  Vector3f p2,m1,m2;
	  Vector3f dSeaPos(0,0,0);
	  float A = 0;
	  for(unsigned int j=0;j<waveGroups.size();j++){
	    waveGroups[j]->computeMovement(p1, time, &dP, &v, &dV);
	    A+=waveGroups[j]->getR();
	    dP[2]=dP[0]; dP[0]*=waveGroups[j]->getCosTheta(); dP[2]*=waveGroups[j]->getSinTheta(); 
	    dSeaPos+= dP;	
	  }
	  m1=p1+dSeaPos;
	  p2=p1+A*velPart;
	  dSeaPos[0]=dSeaPos[1]=dSeaPos[2]=0;
	  for(unsigned int j=0;j<waveGroups.size();j++){
	    waveGroups[j]->computeMovement(p2, time, &dP, &v, &dV); // peut être plus simple car on a pas besoin de v ni dv
	    dP[2]=dP[0]; dP[0]*=waveGroups[j]->getCosTheta(); dP[2]*=waveGroups[j]->getSinTheta(); 
	    dSeaPos+= dP;
	  }
	  m2=p2+dSeaPos;
	  Vector3f dp=0.33*A*velPart; // le coeff est encore à travailler (0.33 semble être un bon compromis)
	  // itérations grossières pour initialiser la méthode de la sécante
	  int nbIter=0;
	  while (m2.dot(velPart)>posPart.dot(velPart)) {
	    p1=p2; m1=m2;
	    p2=p2-dp;
	    dSeaPos[0]=dSeaPos[1]=dSeaPos[2]=0;
	    for(unsigned int j=0;j<waveGroups.size();j++){
	      waveGroups[j]->computeMovement(p2, time, &dP, &v, &dV); // peut être plus simple car on a pas besoin de v ni dv
	      dP[2]=dP[0]; dP[0]*=waveGroups[j]->getCosTheta(); dP[2]*=waveGroups[j]->getSinTheta(); 
	      dSeaPos+= dP;
	    }
	    m2=p2+dSeaPos;
	    nbIter++;
	  }
	 // if (i==0) {
	 //   cout<<"nbIter (phase grossière)="<<nbIter<<" | ";
	 // }
	  // itérations plus fines (méthode de la sécante)
	  Vector3f delta=m2-posPart; delta[1]=0;
	  while (delta.squaredNorm()>0.01*A*A){ // au dixième de l'amplitude totale près
	    Vector3f p=p2-((p2-p1).norm()/(m2-m1).norm())*delta;
	    p1=p2; m1=m2; p2=p;
	    dSeaPos[0]=dSeaPos[1]=dSeaPos[2]=0;
	    for(unsigned int j=0;j<waveGroups.size();j++){
	      waveGroups[j]->computeMovement(p2, time, &dP, &v, &dV); // peut être plus simple car on a pas besoin de v ni dv
	      dP[2]=dP[0]; dP[0]*=waveGroups[j]->getCosTheta(); dP[2]*=waveGroups[j]->getSinTheta(); 
	      dSeaPos+= dP;
	    }
	    m2=p2+dSeaPos;
	    delta=m2-posPart; delta[1]=0;
	    nbIter++;
	  }
	//  if (i==0) {
	//    cout<<"nbIter (phase fine)="<<nbIter<<endl;
	//  }
	  p2-=((p2-p1).norm()/(m2-m1).norm())*delta;
	  // On a trouvé le point sous la particule (c'est p2 à 0.1*A près)
	  Vector3f seaSpeed(0,0,0);
	  float slope_x=0;
	  float slope_z=0;
	  dSeaPos[0]=dSeaPos[1]=dSeaPos[2]=0;
	  for(unsigned int j=0;j<waveGroups.size();j++){
	    waveGroups[j]->computeMovement(p2, time, &dP, &v, &dV);
	    dP[2]=dP[0]; dP[0]*=waveGroups[j]->getCosTheta(); dP[2]*=waveGroups[j]->getSinTheta(); 
	    v[2]=v[0]; v[0]*=waveGroups[j]->getCosTheta(); v[2]*=waveGroups[j]->getSinTheta();
	    dSeaPos+= dP;
	    seaSpeed+= v;
	    float tmp=dV[1]/dV[0];
	    slope_x+=waveGroups[j]->getCosTheta()*tmp;
	    slope_z+=waveGroups[j]->getSinTheta()*tmp;	
	  }
	  Vector3f seaNormal(-slope_x, 1, -slope_z);
	  seaNormal.normalize();
	  m2=p2+dSeaPos; // devrait être sous la particule
	
	  // surface libre sous la particule : position : m2, vitesse du fluide : seaSpeed, normale à la surface : seaNormal
	  if(posPart[1]  < dSeaPos[1]){ // if(posPart[1] + particles[i]->getRadius()< dSeaPos[1]){
	    // on évalue le rebond avec perte de masse et symétrisation de la vitesse
	    float mass = particles[i]->getMass();
	    Vector3f velPart = particles[i]->getVel();
	    velPart-=seaSpeed; // pour se placer dans le repère lié à l'eau (pour le rebond)
	    // symétrie de la vitesse relative à l'eau autour de la normale à la surface libre seaNormal
	    Vector3f vt, vn=seaNormal.dot(velPart)*seaNormal/seaNormal.squaredNorm(); // projection tangentielle et normale de velPart
	    vt=velPart-vn;
	    float incidence=vn.norm()/velPart.norm();
	    vt*=0.7; // friction avec 30% pertes
	    vn*=0.1; // rebond avec 90% pertes
	    velPart=vt-vn;
	    // on repasse dans le repère global
	    velPart+=seaSpeed;
	    // et on met à jour la vitesse
	    particles[i]->setVel(velPart);
	    particles[i]->setMass(0.8*(1-incidence)*mass); // la masse reste d'autant plus que la vitesse de la particule est tangentielle à la surface libre
			
	    // On augmente l'alpha dans la texture de la grille de visualisation
	    // Augmentation de la transparence en corrélation avec la masse de la particule et de la vitesse
			
	    //cout << "dAlpha " << dAlpha << endl;
	    int ix = (int)floor((double)(posPart[0]-ocean->getMin()[0])/ocean->getDx());
	    int iz = (int)floor((double)(posPart[2]-ocean->getMin()[2])/ocean->getDz());
	    
	    int x = (int) floor((ix/(float)(ocean->getNx()-1))*tex.width());
	    int y = (int) floor((iz/(float)(ocean->getNz()-1))*tex.height());

	    if(x>0 && y>0 && x<tex.width() && y<tex.height()){
	      QRgb rgb = tex.pixel(x,y);
	      int alpha = qAlpha(rgb);
	      // Ajout d'une durée de vie par texel
	      if(alpha==0){
		int indexC = ix + iz*ocean->getNx();
		float lifeTime = (rand()/(double)RAND_MAX) * (lifeTimeMax-lifeTimeMin) + lifeTimeMin;
		ocean->setLifeTimeBubbles(indexC,lifeTime);
		//cout << "lifeT: " << lifeTime << endl;				
	      }
	      int nbx = (int) ((rand()/(double)RAND_MAX)*2*powf(particles[i]->getRadius(),1/3.0)*tex.width()/ocean->getTx());// *(tex.width()/ocean->getNx());
	      int nby = (int) ((rand()/(double)RAND_MAX)*2*powf(particles[i]->getRadius(),1/3.0)*tex.height()/ocean->getTz());//(rand()/(double)RAND_MAX);// *(tex.height()/ocean->getNz());
	      //cout << "nbX: " << nbx << " nbY: " << nby << endl;
	      // Augmentation de la transparence pour chaque pixel du texel
	      for(int xM=x-nbx; xM<=x+nbx; xM++){
		    for(int yM=y-nby; yM<=y+nby; yM++){
		      if(xM>0 && yM>0 && xM <tex.width() && yM < tex.height()){
		    QRgb rgb = tex.pixel(xM,yM);
		    int alpha = qAlpha(rgb);
		    if(alpha<=255){
		      float dAlpha = (rand()/(double)RAND_MAX)*15;//powf((mass/totalMass)*particles[i]->getVel().norm(),1/3.0)*5;	
		      //cout << "noise " << dAlpha << endl;
		      alpha = fmin(alpha+dAlpha,255); 
		      tex.setPixel(xM,yM,qRgba(qRed(rgb),qGreen(rgb),qBlue(rgb),alpha));
		    }
		  }
		}
	      }
	    }
	    // Generation des sprays
	    if(SPRAYS == 1){
	      //if(seaSpeed[1]<0 || particles[i]->getMass()<1 || posPart[1] + particles[i]->getRadius()< dSeaPos[1]){
			if(particles[i]->getMass()<1 || posPart[1] < dSeaPos[1]){
				int nbSprays = particles[i]->getMass()*particles[i]->getVel().norm();
				cout << "nbSprays: " << nbSprays << endl;
				Vector3f velP = particles[i]->getVel();
				if(posPart[1] < dSeaPos[1] && velP[1]<0)
				{
					velP[1]=-velP[1];
					velP[1]*=2;
					velP[0] = 2*velP[0];
				}
				ocean->getSprays()->generate(gridSPH->getLocalRotated(particles[i]->getPos()),velP,particles[i]->getMass()/nbSprays,particles[i]->getRadius(),nbSprays);
	      	}
	    }
	    // On supprime la particule (éventuellement remplacer 1 pour la masse par un meilleur critère)
	    if (particles[i]->getMass()<1 || posPart[1] + particles[i]->getRadius()< dSeaPos[1] ) deleteParticle(i);
	  }
	}
	ocean->setTexBubbles(tex);
}
/****************************************************************************/
void SPH::display()
{
		// Affichage de la grille
		//gridSPH->display();

		// Affichage des particules
		displayParticlesByField(1);
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
    	for(unsigned i=0;i<particles.size();i++){
		SPHParticle *p = particles[i];
		float m = p->getMass();
		float hue = 240 * (mmax - fmin(m,mmax)) / (mmax-mmin);
		Vector3d Hsv(hue,1,1);
		
		p->setColor(convertHsvToRgb(Hsv));
    	}
   }
   //DISPLAY BY VELOCITY FIELD
   if(field==1){
	float vmax, vmin;
	vmax = 0; vmin = 1000000;
	for(unsigned i=0;i<particles.size();i++){
		float length = particles[i]->getVel().norm();
		if(vmax<length) vmax = length;
		if(vmin>length) vmin = length;
	}
	#pragma omp parallel for
    	for(unsigned i=0;i<particles.size();i++){
		SPHParticle *p = particles[i];
		float length = p->getVel().norm();
		float hue = 240 * (vmax - fmin(length,vmax)) / (vmax-vmin);
		Vector3d Hsv(hue,1,1);
		p->setColor(convertHsvToRgb(Hsv));
    	}
   }
}
/****************************************************************************/
/****************************************************************************/
Vector3f SPH::convertHsvToRgb(Vector3d Hsv)
{
   Vector3f result;
   int t = (int)(floor((double)(Hsv[0]/60)))%6;
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
		for (unsigned int i=0; i<particles.size(); i++){
			Vector3f pos = particles[i]->getPos();
			Vector3f vel = particles[i]->getVel();
			fprintf(f,"%d %f %f %f %f %f %f 0\n",i,pos[0],pos[1],pos[2],vel[0],vel[1],vel[2]);
		}
	}
}
/****************************************************************************/
/****************************************************************************/
float SPH::getRho0() const
{
	return rho0;
}
/****************************************************************************/
float SPH::getMu() const
{
	return mu;
}
/****************************************************************************/
float SPH::getTS() const
{
	return tS;
}
/****************************************************************************/
float SPH::getLTS() const
{
	return ltS;
}
/****************************************************************************/
int  SPH::getNbParticles() const
{
	return particles.size();
}
/****************************************************************************/
GridSPH* SPH::getGrid() const
{
	return gridSPH;
}
/****************************************************************************/
SPHParticle* SPH::getParticle(int index) const
{
	assert (index < (int)particles.size());
	return particles[index];
}
/****************************************************************************/
vector<SPHParticle*> SPH::getParticles() const
{
	return particles;
}
/****************************************************************************/
/****************************************************************************/





