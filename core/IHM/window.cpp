#include <window.h>
//************************************************************************/
//************************************************************************/
Window::Window():QMainWindow()
{
    menuBar = new QMenuBar(0);
    glWidget = new GLWidget() ;

    // MENU PARTICLES
    menuParticles = new QMenu(tr("&Particles"));

    addParticlesRendering_act = new QAction(tr("&Add Rendering Particles"),this);
    addParticlesRendering_act->setShortcut(tr("r"));
    connect(addParticlesRendering_act, SIGNAL(triggered()), this, SLOT( addParticlesRendering()));
    
    addParticlesBoundary_act = new QAction(tr("&Add Boundary Particles"),this);
    addParticlesBoundary_act->setShortcut(tr("b"));
    connect(addParticlesBoundary_act, SIGNAL(triggered()), this, SLOT( addParticlesBoundary()));

    menuParticles->addAction(addParticlesRendering_act);
    menuParticles->addAction(addParticlesBoundary_act);
    menuParticles->show();
     // MENU ANIMATION
    menuAnimation = new QMenu(tr("&Animation"));   
    play_act = new QAction(tr("&Play"),this);
    play_act->setShortcut(tr("p"));
    connect(play_act, SIGNAL(triggered()), this, SLOT(play()));
    stop_act = new QAction(tr("&Stop"),this);
    stop_act->setShortcut(tr("s"));
    connect(stop_act, SIGNAL(triggered()), this, SLOT(stop()));
    init_act = new QAction(tr("&ReInit"),this);
    init_act->setShortcut(tr("i"));
    connect(init_act, SIGNAL(triggered()), this, SLOT(init()));
    menuAnimation->addAction(play_act);
    menuAnimation->addAction(stop_act);
    menuAnimation->addAction(init_act); 

    // MENU EXPORT
    menuExport = new QMenu("&Export");
    exportMitsuba_act = new QAction(tr("&Export Mitsuba"),this);
    exportMitsuba_act->setShortcut(tr("e"));
	
    connect(exportMitsuba_act, SIGNAL(triggered()), this, SLOT(exportMitsuba()));
    exportData_act = new QAction(tr("&Export Data"),this);
    exportData_act->setShortcut(tr("d"));
    connect(exportData_act, SIGNAL(triggered()), this, SLOT(exportData()));
    menuExport->addAction(exportMitsuba_act);
     menuExport->addAction(exportData_act);

    menuBar->addMenu(menuParticles);
    menuBar->addMenu(menuAnimation);
    menuBar->addMenu(menuExport);

    this->setMenuBar(menuBar);
    this->setCentralWidget(glWidget);

    setWindowTitle(tr("Hybrid Breaking Waves"));
}
//************************************************************************/
//************************************************************************/
GLWidget* Window::getGLWidget()
{
	return glWidget;
}
//************************************************************************/
//************************************************************************/
void Window::setGLWidget(GLWidget* G)
{
	 this->glWidget = G;
}
//************************************************************************/
//************************************************************************/
// SLOTS MENU PARTICLES
//************************************************************************/
//************************************************************************/
void Window::addParticlesRendering()
{
	/*if(glWidget->getHybridOcean()!=NULL){
		SolverSPH* solver = glWidget->getHybridOcean()->getSolver();
		if(solver!=NULL){
			if(solver->getGhostRendering()==NULL)
				solver->addGhostRendering();
		}
	}*/	
}
//************************************************************************/
void Window::addParticlesBoundary()
{
}
//************************************************************************/
//************************************************************************/
// SLOTS MENU ANIMATION
//************************************************************************/
//************************************************************************/
void Window::play()
{
 if(glWidget->getHybridOcean()!=NULL)
 {
	printf("fonction animate\n");
 	glWidget->startAnimation();
 	glWidget->animate();
 }
 else 
	alertBox("Ocean non initialisé !!",QMessageBox::Critical);
}
//************************************************************************/
void Window::stop()
{
	glWidget->stopAnimation();
}
//************************************************************************/
void Window::init()
{
	glWidget->clean();
}
//************************************************************************/
//************************************************************************/
// SLOTS MENU EXPORT
//************************************************************************/
//************************************************************************/
void Window::exportMitsuba()
{
 if(glWidget->getHybridOcean()!=NULL)
 	glWidget->getHybridOcean()->exportMitsuba(glWidget->camera());
 else 
	alertBox("Ocean non initialisé !!",QMessageBox::Critical);
}
//************************************************************************/
void Window::exportData()
{
 if(glWidget->getHybridOcean()!=NULL){
	char filenameOBJ[100], filenameP[100];
	char rep[100]="sortie";
 	glWidget->getHybridOcean()->exportData(rep,filenameOBJ,filenameP);
 }
 else 
	alertBox("Ocean non initialisé !!",QMessageBox::Critical);
}
//************************************************************************/
//************************************************************************/
void Window::alertBox(QString text, QMessageBox::Icon icon)
{
	QMessageBox Q;
	Q.setIcon(icon);
	Q.setText(text);
        Q.exec();
}

//************************************************************************/
//************************************************************************/
