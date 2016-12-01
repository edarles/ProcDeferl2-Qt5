#include <GL/glew.h>
#include <iostream>
#include <unistd.h>
#include <glWidget.h>
//******************************************************************************
//******************************************************************************
HybridOcean* GLWidget::getHybridOcean()
{
	return this->_hybridOcean;
}
//******************************************************************************
void GLWidget::setHybridOcean(HybridOcean* hybridOcean)
{
	this->_hybridOcean = hybridOcean;
}
//******************************************************************************
//******************************************************************************
QSize GLWidget::minimumSizeHint() const
{
	return QSize(640, 480);
}
//******************************************************************************
QSize GLWidget::sizeHint() const 
{
	return QSize(1024, 768);
}
//******************************************************************************
//******************************************************************************
void GLWidget::init() 
{
       //	setAxisIsDrawn(true);
	setTextIsEnabled (true);

	//	setGridIsDrawn();
	setShortcut(ANIMATION, Qt::CTRL + Qt::Key_A);
	camera()->setType(Camera::PERSPECTIVE);
	camera()->setSceneRadius(50.0);
	camera()->showEntireScene();
	camera()->setPosition(Vec(-38.644604,5.909318,25.212061));
	camera()->setViewDirection(Vec(-0.061944,-0.243702,-0.967870));

	initRender();
	setAnimationPeriod(1);
	setFPSIsDisplayed(true);

	_hybridOcean = NULL;
	test();
}

//******************************************************************************
//******************************************************************************
void GLWidget::initRender() 
{
	glewInit();

	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	srand ( time ( 0x0 ) );
	glClearColor( 0,0,0, 1.0 );
	glShadeModel( GL_SMOOTH );
	glEnable ( GL_COLOR_MATERIAL );
	glEnable (GL_DEPTH_TEST);
	glDepthMask ( 1 );

}
//******************************************************************************
//******************************************************************************
//void GLWidget::keyPressEvent( QKeyEvent *e )
//{ 
//}
//******************************************************************************
//******************************************************************************
void GLWidget::clean()
{
  	init();
}

//******************************************************************************
//******************************************************************************
void GLWidget::draw() 
{
	if(_hybridOcean){
		 int f = _hybridOcean->getFrame();
		 _hybridOcean->display();
	}
}
//******************************************************************************
//******************************************************************************
void GLWidget::animate()
{
	if(_hybridOcean){
		 _hybridOcean->animate();
		// _hybridOcean->exportMitsuba(camera());
	}
	//stopAnimation();
}
//******************************************************************************
//******************************************************************************
void GLWidget::test()
{
	test2();
}
//******************************************************************************
//******************************************************************************
void GLWidget::test1()
{	
	WaveGroupMP *waveGr1 = new WaveGroupMP();
	_hybridOcean = new HybridOcean();
	_hybridOcean->addWaveGroup(waveGr1);
}
//******************************************************************************
void GLWidget::test2()
{	
	// Création d'un groupe de vague
	float k1 = 2*M_PI/13.0; // nombre d'onde : 2pi/lambda
  	float w1 = sqrt(9.81*k1); // pulsation (relation de dispersion par grande profondeur)
  	float theta1 = 0.4;//0.1; // direction du vecteur d'onde kx=k.cos(theta), ky=k.sin(theta)
  	float r1 = 0.5/k1; // rayon des plus grosses vagues au centre du groupe (r.k==1 est la cambrure limite du déferlement)
  	float phi01 = 0; // phase initiale de l'onde
  	float zeta1 = M_PI/7; // phase instantannée maximale (-zeta<dphiw<zeta)
  	int n1 = 7; // nombre de vagues sous l'enveloppe

	// Création d'un groupe de vague
	float k2 = 2*M_PI/10.0; // nombre d'onde : 2pi/lambda
  	float w2 = sqrt(9.81*k2); // pulsation (relation de dispersion par grande profondeur)
  	float theta2 = -0.2; // direction du vecteur d'onde kx=k.cos(theta), ky=k.sin(theta)
  	float r2 = 0.6/k2; // rayon des plus grosses vagues au centre du groupe (r.k==1 est la cambrure limite du déferlement)
  	float phi02 = 0.01; // phase initiale de l'onde
  	float zeta2 = M_PI/6; // phase instantannée maximale (-zeta<dphiw<zeta)
  	int n2 = 5; // nombre de vagues sous l'enveloppe

	WaveGroupMP *waveGr1 = new WaveGroupMP(n1,k1,theta1,r1,phi01,zeta1);
	WaveGroupMP *waveGr2 = new WaveGroupMP(n2,k2,theta2,r2,phi02,zeta2);

	_hybridOcean = new HybridOcean();
	_hybridOcean->addWaveGroup(waveGr1);
	_hybridOcean->addWaveGroup(waveGr2);
}

//******************************************************************************
//******************************************************************************
