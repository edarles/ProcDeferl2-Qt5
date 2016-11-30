#include <utils.h>
#include <HybridOcean.h>
#include <GL/gl.h>
#include <shaders.h>

GLuint HybridOcean::m_program = 0;
float* HybridOcean::posP = new float[4*MAX_PARTICLES];
float* HybridOcean::colorsP = new float[4*MAX_PARTICLES];
int    HybridOcean::nP = 0;

/****************************************************************************/
/****************************************************************************/
HybridOcean::HybridOcean()
{
	if(m_program==0) m_program = _compileProgram(sphereVertexShader, spherePixelShader);

	colorOcean << 0,0,1,1;
	colorBreakingWaves << 1,1,1,1;

	this->m_waveGroups.clear();
	//m_visuGrid = new GridOcean(Vector3f(-100,0,-50), Vector3f(100,0,50),0.1,0.1);
	//m_visuGrid = new GridOcean(Vector3f(-70,0,-50), Vector3f(-20,0,50),1,1);
	m_visuGrid = new GridOcean(Vector3f(-50,0,-50), Vector3f(50,0,50),1,1);
	m_visuGrid->setColor(colorOcean);	

	dt = 0.01;

	frame = 0;
}
/****************************************************************************/
HybridOcean::HybridOcean(vector<WaveGroup*> waveGroups, float dt)
{
	if(m_program==0) m_program = _compileProgram(sphereVertexShader, spherePixelShader);

	colorOcean << 0,0,1,1;
	colorBreakingWaves << 1,1,1,1;

	this->m_waveGroups.clear();
	for(unsigned int i=0; i<waveGroups.size(); i++)
		addWaveGroup(waveGroups[i]);

	//m_visuGrid = new GridOcean(Vector3f(-100,0,-50), Vector3f(100,0,50),0.1,0.1);
	//m_visuGrid = new GridOcean(Vector3f(-70,0,-50), Vector3f(-20,0,50),0.5,0.5);
	m_visuGrid = new GridOcean(Vector3f(-50,0,-50), Vector3f(50,0,50),1,1);
	m_visuGrid->setColor(colorOcean);
	
	this->dt = dt;

	frame = 0;
}

/****************************************************************************/
HybridOcean::HybridOcean(GridOcean* gridOcean, vector<WaveGroup*> waveGroups, float dt)
{
	if(m_program==0) m_program = _compileProgram(sphereVertexShader, spherePixelShader);

	colorOcean << 0,0,1,1;
	colorBreakingWaves << 1,1,1,1;
	this->m_waveGroups.clear();
	for(unsigned int i=0; i<waveGroups.size(); i++)
		addWaveGroup(waveGroups[i]);
	m_visuGrid = gridOcean;
	m_visuGrid->setColor(colorOcean);	

	this->dt = dt;

	frame = 0;
}
/****************************************************************************/
HybridOcean::~HybridOcean()
{
	for(unsigned int i=0; i<m_waveGroups.size(); i++)
		delete m_waveGroups[i];

	m_waveGroups.clear();
	vector<WaveGroup*>().swap(m_waveGroups);
	m_waveGroups.shrink_to_fit();
 
	delete(m_visuGrid);

	for(unsigned int i=0;i<m_breakingWaves.size();i++)
		delete m_breakingWaves[i];

	m_breakingWaves.clear();
	vector<BreakingWave*>().swap(m_breakingWaves);
	m_breakingWaves.shrink_to_fit();

	delete[] posP;
	delete[] colorsP;
}
/****************************************************************************/
/****************************************************************************/
WaveGroup* HybridOcean::getWaveGroup(int index)
{
	assert(index < (int)m_waveGroups.size());
	return m_waveGroups[index];
}
/****************************************************************************/
vector<WaveGroup*> HybridOcean::getWavesGroups()
{
	return m_waveGroups;
}
/****************************************************************************/
int HybridOcean::getNWaveGroups()
{
	return m_waveGroups.size();
}
/****************************************************************************/
Grid* HybridOcean::getVisuGrid()
{
	return m_visuGrid;
}
/****************************************************************************/
int HybridOcean::getFrame()
{
	return frame;
}
/****************************************************************************/
/****************************************************************************/
void HybridOcean::addWaveGroup(WaveGroup *waveGroup)
{
	m_waveGroups.push_back(waveGroup);
}
/****************************************************************************/
void HybridOcean::addWaveGroups(vector<WaveGroup*> waveGroups)
{
	for(unsigned int i=0; i<waveGroups.size(); i++)
		m_waveGroups.push_back(waveGroups[i]);
}
/****************************************************************************/
/****************************************************************************/
void HybridOcean::loadSpectrum(const char* filename)
{
	FILE * f = fopen(filename,"r");
	if(f!=NULL){
		for(unsigned int i=0; i<m_waveGroups.size(); i++)
			delete m_waveGroups[i];
		m_waveGroups.clear();
		int nbWaves;
		fscanf(f,"%d\n",&nbWaves);
		printf("nb:%d",nbWaves);
		for(int i=0; i<nbWaves;i++){
			WaveGroupMP *w = new WaveGroupMP();
			w->load(f);
			addWaveGroup(w);
		}
		fclose(f);
	}
}
/****************************************************************************/
void HybridOcean::saveSpectrum(const char* filename)
{
	FILE *f = fopen(filename, "w");
	if(f!=NULL){
		fprintf(f,"%lu\n",m_waveGroups.size());
		for(unsigned int i=0; i<m_waveGroups.size();i++)
			m_waveGroups[i]->save(f);
		fclose(f);
	}
}
/****************************************************************************/
/****************************************************************************/
vector<BreakingWave*> HybridOcean::getBreakingWaves()
{
	return m_breakingWaves;
}
/****************************************************************************/
void HybridOcean::addBreakingWave(BreakingWave* breakingWave)
{
	m_breakingWaves.push_back(breakingWave);
}
/****************************************************************************/
void HybridOcean::deleteBreakingWave(int index)
{
	assert(index<(int)m_breakingWaves.size());
	m_breakingWaves[index] = m_breakingWaves[m_breakingWaves.size()-1];
	m_breakingWaves.pop_back();
}
/****************************************************************************/
void HybridOcean::mergeBreakingWaves()
{
	bool merge = true;

	vector<BreakingWave*> toDelete;
	while(merge){

	merge = false;

	for(unsigned int i=0;i<m_breakingWaves.size();i++){

		for(unsigned int j=0;j<m_breakingWaves.size();j++){

			BreakingWave *bI = m_breakingWaves[i];
			if(bI!=NULL){
			GridBreaking *gBI = bI->getGridBreaking();
			Vector3f minI = gBI->getMin(); Vector3f maxI = gBI->getMax();
			Vector3f cI = gBI->getCenter(); 
			float lambdaMaxI = gBI->getMaxLambda();

			if(i!=j){
				
				BreakingWave *bJ = m_breakingWaves[j];
				if(bJ!=NULL){
				GridBreaking *gBJ = bJ->getGridBreaking();
				Vector3f cJ = gBJ->getCenter(); 

				// Test 1: Si cJ est compris entre minI et maxI
				if(cJ[0]>=minI[0] && cJ[2]>=minI[2] && cJ[0]<=maxI[0] && cJ[2]<=maxI[2]){

					// Critere 2 : distance centreI - centreJ < lambdaMax /8
					//printf("Trouve ");
					Vector2f d; d << cI[0] - cJ[0], cI[2] - cJ[2] ;
					//cout << "d: " << d.norm() << " " << "lM: " << lambdaMaxI << endl;

					// Critere 3 : si on trouve un pt actif de gBJ tel que distance(ptJ,cI)<distance(ptJ,cJ)
					bool trouve = false;
					int k = 0;
					while(!trouve && k<gBJ->getNbActivePts()){
						Vector3f pos = gBJ->getLocalRotated(gBJ->getPos(gBJ->getActivePt(k)));
						Vector2f ptCI; ptCI << pos[0] - cI[0], pos[2] - cI[2];
						Vector2f ptCJ; ptCJ << pos[0] - cJ[0], pos[2] - cJ[2];
						if(ptCI.norm()<ptCJ.norm())
							trouve = true;
						k++;
					}
					
					// Test 2 : Critere 2 OU Critere 3//
					//if((d.norm()<lambdaMaxI)||(trouve)) {
						// On merge bJ à bI
						//printf("**** Merging de %d et %d\n ***",i,j);
						//cout << "D: " << d.norm() << " lMax: " << lambdaMaxI << endl;
						bI->merge(bJ, dt);
						delete m_breakingWaves[j];
						m_breakingWaves.erase(m_breakingWaves.begin()+j);
						m_breakingWaves.shrink_to_fit();
						merge = true;
					//}
					//else
					//	cout << "Non Merge " << "D: " << d.norm() << " lMax: " << lambdaMaxI << endl;
				}
			}
			}
}
		}
	}
	}
}
/****************************************************************************/
/****************************************************************************/
void HybridOcean::setVisuGrid(GridOcean *grid)
{
	m_visuGrid = grid;
}
/****************************************************************************/
/****************************************************************************/
void HybridOcean::animate()
{
	//cout << "animate" << endl;
	// Update des groupes de vagues
	for(unsigned int i=0;i<m_waveGroups.size();i++)
		m_waveGroups[i]->update();

	// Update de grille de l'océan
	m_visuGrid->update(m_waveGroups, dt);

	// Génération des vagues déferlantes
	m_visuGrid->generateBreakingWaves(m_waveGroups, &m_breakingWaves, dt);

	mergeBreakingWaves();

	// Update des vagues déferlantes
	for(unsigned int i=0;i<m_breakingWaves.size();i++){
		m_breakingWaves[i]->checkActiveWgs();
		m_breakingWaves[i]->checkActivePts();
		m_breakingWaves[i]->transform();
		m_breakingWaves[i]->update(dt,m_visuGrid);
	}

	for(unsigned int i=0;i<m_breakingWaves.size();i++)
		m_breakingWaves[i]->generateParticles(dt);

	// Suppression des vagues déferlantes qui ne sont plus actifs
	for(unsigned int i=0;i<m_breakingWaves.size();i++){
		if(!m_breakingWaves[i]->checkIfActive()){
			delete m_breakingWaves[i]; 
			m_breakingWaves.erase(m_breakingWaves.begin()+i);
		}
	}
	m_breakingWaves.shrink_to_fit();

	//***** POUR AFFICHAGE DES PARTICULES AVEC GLSL ***********/
	nP = 0;
	for(unsigned int i=0;i<m_breakingWaves.size();i++){
		for(int j=0;j<m_breakingWaves[i]->getSolver()->getNbParticles();j++){
			assert(nP<MAX_PARTICLES);
			Vector3f pos = m_breakingWaves[i]->getSolver()->getParticle(j)->getPos();
			Vector3f c = m_breakingWaves[i]->getSolver()->getParticle(j)->getColor();
			float h = m_breakingWaves[i]->getSolver()->getParticle(j)->getRadius();
			posP[nP*4] = pos[0]; 
			posP[nP*4+1] = pos[1]; 
			posP[nP*4+2] = pos[2]; 
			posP[nP*4+3] = h;
			colorsP[nP*4] = c[0];
			colorsP[nP*4+1] = c[1];
			colorsP[nP*4+2] = c[2];
			colorsP[nP*4+3] = 0.9;
			nP++;
		}
	}
	//*****************************************************/

	frame++;
	
}
/****************************************************************************/
/****************************************************************************/
void HybridOcean::display()
{
	m_visuGrid->display();

	for(unsigned int i=0;i<m_breakingWaves.size();i++)
		m_breakingWaves[i]->display();
	displaySpheres(m_program,posP,colorsP,nP);
}
/****************************************************************************/
/****************************************************************************/
void HybridOcean::exportData(const char* rep, char filenameOBJ[100], char filenameP[100])
{
	char frameF[100];  sprintf(frameF,"%s","");;
	if(frame<10)  sprintf(frameF,"%s","000");
	if(frame>=10 && frame<100) sprintf(frameF,"%s","00");
	if(frame>=100 && frame<1000)  sprintf(frameF,"%s","0");
	
	// Export Grid Ocean to OBJ format
	sprintf(filenameOBJ, "%s/OBJ/ocean_%s%d.obj",rep,frameF,frame-1);
	m_visuGrid->exportOBJ(filenameOBJ);

	// Export particles positions for houdini mesher
	sprintf(filenameP, "%s/Particles/particles_%s%d.txt",rep,frameF,frame-1);
	for(unsigned int i=0;i<m_breakingWaves.size();i++)
		m_breakingWaves[i]->getSolver()->exportParticlesHoudini(filenameP);
}
/****************************************************************************/
/****************************************************************************/
void HybridOcean::exportMitsuba(qglviewer::Camera *cam)
{
	char frameF[100];  sprintf(frameF,"%s","");;
	if(frame<10)  sprintf(frameF,"%s","000");
	if(frame>=10 && frame<100) sprintf(frameF,"%s","00");
	if(frame>=100 && frame<1000)  sprintf(frameF,"%s","0");

	char filename[100]; sprintf(filename,"sortie/MTS/file_%s%d.xml",frameF,frame-1);
	char filenameMeshHoudini[100]; sprintf(filenameMeshHoudini,"../Houdini/file_%s%d.xml",frameF,frame-1);
	char filename2OBJ[100]; sprintf(filename2OBJ,"../OBJ/ocean_%s%d.obj",frameF,frame-1);

	char filenameOBJ[100], filenameP[100];
	char rep[100]="sortie";
	exportData(rep, filenameOBJ, filenameP);
	
	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );

	TiXmlElement * scene = new TiXmlElement( "scene" );
	scene->SetAttribute("version","0.5.0");
	doc.LinkEndChild(scene);

	TiXmlElement * integrator = new TiXmlElement( "integrator");
	integrator->SetAttribute("type","volpath");
	scene->LinkEndChild(integrator);

	// Add Ocean Mesh
	TiXmlElement * shapeObj = new TiXmlElement( "shape");
	shapeObj->SetAttribute("type","obj");
	TiXmlElement * file = new TiXmlElement( "string");
	file->SetAttribute("name","filename");
	file->SetAttribute("value",filename2OBJ);
	shapeObj->LinkEndChild(file);

	TiXmlElement * bsdfObj = new TiXmlElement( "bsdf");
	bsdfObj->SetAttribute("type","diffuse");
	TiXmlElement * rgbObj = new TiXmlElement( "rgb");
	rgbObj->SetAttribute("name","diffuseReflectance");
	rgbObj->SetAttribute("value",".1, .5, .4");
	bsdfObj->LinkEndChild(rgbObj);
	shapeObj->LinkEndChild(bsdfObj);

	TiXmlElement * bsdf2Obj = new TiXmlElement( "bsdf");
	bsdf2Obj->SetAttribute("type","dielectric");
	TiXmlElement * interior = new TiXmlElement( "float");
	interior->SetAttribute("name","intIOR");
	interior->SetAttribute("value","1.333");
	bsdf2Obj->LinkEndChild(interior);
	TiXmlElement * exterior = new TiXmlElement( "float");
	exterior->SetAttribute("name","extIOR");
	exterior->SetAttribute("value","1.0");
	bsdf2Obj->LinkEndChild(exterior);
	shapeObj->LinkEndChild(bsdf2Obj);

	scene->LinkEndChild(shapeObj);

	// Add Particle Mesh
	/*TiXmlElement * shapeObjP = new TiXmlElement( "shape");
	shapeObjP->SetAttribute("type","obj");
	TiXmlElement * fileP = new TiXmlElement( "string");
	fileP->SetAttribute("name","filename");
	fileP->SetAttribute("value",filenameMeshHoudini);
	shapeObjP->LinkEndChild(fileP);
	TiXmlElement * bsdfObjP = new TiXmlElement( "bsdf");
	bsdfObjP->SetAttribute("type","twosided");
	TiXmlElement * bsdf2ObjP = new TiXmlElement( "bsdf");
	bsdf2ObjP->SetAttribute("type","diffuse");
	TiXmlElement * rgbObjP = new TiXmlElement( "rgb");
	rgbObjP->SetAttribute("name","diffuseReflectance");
	rgbObjP->SetAttribute("value",".1, .1, .8");
	bsdf2ObjP->LinkEndChild(rgbObjP);
	bsdfObjP->LinkEndChild(bsdf2ObjP);
	shapeObjP->LinkEndChild(bsdfObjP);
	scene->LinkEndChild(shapeObjP);
	*/

	// Export Camera caracteristics
	TiXmlElement * sensor = new TiXmlElement( "sensor");
	sensor->SetAttribute("type","perspective");
	TiXmlElement * transformCamera = new TiXmlElement( "transform");
	transformCamera->SetAttribute("name","toWorld");

	TiXmlElement * lookat = new TiXmlElement( "lookat");
	QString target="";
	target= QString::number(cam->position().x);
	target +=  " ";
	target += QString::number(cam->position().y);
	target+= " ";
	target+= QString::number(cam->position().z);
	lookat->SetAttribute("target",target.toStdString());
	printf("\ntarget:%s\n",target.toStdString().c_str());
	
	QString pos="";
	pos = QString::number(cam->viewDirection().x);
	pos +=  " ";
	pos += QString::number(cam->viewDirection().y);
	pos += " ";
	pos += QString::number(cam->viewDirection().z);
	lookat->SetAttribute("origin",pos.toStdString());
	//printf("origin:%s\n",pos.toStdString().c_str());
	transformCamera->LinkEndChild(lookat);
	sensor->LinkEndChild(transformCamera);

	TiXmlElement * sampler = new TiXmlElement( "sampler");
	sampler->SetAttribute("type","ldsampler");

	TiXmlElement * samplerCount = new TiXmlElement( "integer");
	samplerCount->SetAttribute("name","sampleCount");
	samplerCount->SetAttribute("value","512");
	sampler->LinkEndChild(samplerCount);

	sensor->LinkEndChild(sampler);

	TiXmlElement * film = new TiXmlElement( "film");
	film->SetAttribute("type","ldrfilm");
	TiXmlElement * banner = new TiXmlElement( "boolean");
	banner->SetAttribute("name","banner");
	banner->SetAttribute("value","false");
	film->LinkEndChild(banner);
	TiXmlElement * height = new TiXmlElement( "integer");
	height->SetAttribute("name","height");
	height->SetAttribute("value","512");
	film->LinkEndChild(height);
	TiXmlElement * pixels = new TiXmlElement( "string");
	pixels->SetAttribute("name","pixelFormat");
	pixels->SetAttribute("value","rgb");
	film->LinkEndChild(pixels);
	TiXmlElement * width = new TiXmlElement( "integer");
	width->SetAttribute("name","width");
	width->SetAttribute("value","512");
	film->LinkEndChild(width);

	TiXmlElement * filter = new TiXmlElement( "rfilter");
	filter->SetAttribute("type","gaussian");
	film->LinkEndChild(filter);
	sensor->LinkEndChild(film);
	scene->LinkEndChild(sensor);

	TiXmlElement * emitter = new TiXmlElement( "emitter");
	emitter->SetAttribute("type","envmap");
	emitter->SetAttribute("id","Area_002-light");
	TiXmlElement * nameEnvMap = new TiXmlElement( "string");
	nameEnvMap->SetAttribute("name","filename");
	nameEnvMap->SetAttribute("value","sky.jpg");
	emitter->LinkEndChild(nameEnvMap);
	TiXmlElement * transformEnvMap = new TiXmlElement( "transform");
	transformEnvMap->SetAttribute("name","toWorld");
	TiXmlElement * rotateEnvMap = new TiXmlElement( "rotate");
	rotateEnvMap->SetAttribute("y","1");
	rotateEnvMap->SetAttribute("angle","-180");
	transformEnvMap->LinkEndChild(rotateEnvMap);
	TiXmlElement * matrixEnvMap = new TiXmlElement( "matrix");

	matrixEnvMap->SetAttribute("value","-0.224951 -0.000001 -0.974370 0.000000 -0.974370 0.000000 0.224951 0.000000 0.000000 1.000000 -0.000001 8.870000 0.000000 0.000000 0.000000 1.000000");
	transformEnvMap->LinkEndChild(matrixEnvMap);
	emitter->LinkEndChild(transformEnvMap);

        TiXmlElement * scaleEnvMap = new TiXmlElement( "float");
	scaleEnvMap->SetAttribute("name","scale");
	scaleEnvMap->SetAttribute("value","3");

	emitter->LinkEndChild(scaleEnvMap);

	scene->LinkEndChild(emitter);

	doc.SaveFile(filename);
}
