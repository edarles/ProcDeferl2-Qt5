#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QtGui>
#include <QMainWindow>
#include <QtCore/QDir>
#include <QtGui>
#include <QMainWindow>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QMenuBar>
#include <QFileDialog>

#include <glWidget.h>

using namespace std;
//************************************************************************/
//************************************************************************/
class Window : public QMainWindow
{
    Q_OBJECT

//************************************************************************/
public:
    Window();

    GLWidget* getGLWidget();
    void setGLWidget(GLWidget* glWidget);

//************************************************************************/
public slots:

     // slots du menu Particles
     void addParticlesRendering();
     void addParticlesBoundary();

     // slots du menu Animation
     void play();
     void stop();
     void init();

     // slots pour export dans Mistuba Renderer
     void exportMitsuba();
     void exportData();

private :

    //************************************************************************/
    GLWidget *glWidget;
    QMenuBar *menuBar;

    QMenu *menuParticles;
    QMenu *menuAnimation;
    QMenu *menuExport;

    // ACTION DU MENU PARTICULES
    QAction *addParticlesRendering_act;
    QAction *addParticlesBoundary_act;

     // ACTION MENU ANIMATION
    QAction *play_act;
    QAction *stop_act;
    QAction *init_act;

    // ACTION EXPORT FOR RENDER TO MITSUBA
    QAction *exportMitsuba_act;
    QAction *exportData_act;

    QHBoxLayout *layout2;
    QVBoxLayout *mainLayout;
    QDir current_dir;

    void alertBox(QString text, QMessageBox::Icon);
};
//************************************************************************/
//************************************************************************/
#endif

