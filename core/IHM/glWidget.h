#ifndef GLWIDGET_H
#define GLWIDGET_H

//************************************************************************/
//************************************************************************/

#include <QWidget>
#include <QtGui>
#include <math.h>
#include <QGLViewer/qglviewer.h>
#include <HybridOcean.h>

//************************************************************************/
//************************************************************************/

using namespace qglviewer;

//************************************************************************/
//************************************************************************/
 class GLWidget : public QGLViewer
 {
  
 public:

   //******************************************************************************
   // GETTERS ET SETTERS
   //******************************************************************************

   HybridOcean* getHybridOcean();
   void setHybridOcean(HybridOcean* hybridOcean);

  //******************************************************************************
  // SURCHARGE DES FONCTIONS
  //******************************************************************************
     virtual void  init();
     virtual void  draw();
     virtual void  animate();
     virtual QSize minimumSizeHint() const;
     virtual QSize sizeHint() const;
     virtual void keyPressEvent( QKeyEvent *e );

  //******************************************************************************
  // FONCTIONS PROPRES
  //******************************************************************************
     void clean();
     void initRender();

     void test();

     // Methodes de testing
     void test1();
     void test2();

  private :

     //******************************************************************************
     HybridOcean* _hybridOcean;

 };

#endif
