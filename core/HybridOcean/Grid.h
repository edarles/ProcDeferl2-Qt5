/****************************************************************************/
/****************************************************************************/
#ifndef _GRID_H__
#define _GRID_H__

#include <Eigen/Dense>
#include <vector>
#include <GL/gl.h>
using namespace Eigen;
using namespace std;

#pragma once
#define INF 1000000007

class Grid
{
/****************************************************************************/
	public : 

		Grid();
		Grid(Vector3f min, Vector3f max, float dx, float dy, float dz);
		virtual ~Grid();

		/**************** GETTERS ***************************/
		Vector3f getCenter();
		Vector3f getMin() ;
		Vector3f getMax();
		float    getDx();
		float    getDy();
		float    getDz();
		int      getNx();
		int      getNy();
		int      getNz();
		int      getMn();
	
		Vector3f    getPos(int ix, int iy, int iz);
		Vector3f    getPos(int indexCell);
		Vector4f getColor();
		float    getAngleRotation();

		/**************** SETTERS ***************************/
		void     setCenter(Vector3f center);
		void     setMin(Vector3f min);
		void     setMax(Vector3f max);
		void     setDx(float dx);
		void     setDy(float dy);
		void     setDz(float dz);
		void     setNx(int nx);
		void     setNy(int ny);
		void     setNz(int nz);
		void     setPos(int ix, int iy, int iz, Vector3f pos);
		void	 setColor(Vector4f color);
		void     setAngleRotation(float angleRotation);

		/**************** ROUTINE DE CREATION ***************************/
		virtual void init();
		virtual void createGrid();

		/**************** ROUTINE DE TRANSFORMATION *********************/
		virtual void translate(Vector3f t);
		virtual void scale(Vector3f s);
		virtual void rotate(float angle);

		/**************** ROUTINE DE CHANGEMENT DE REPERE ***************/
		Vector3f getLocalRotated(Vector3f pos);
		Vector3f getLocalRotate(Vector3f pos);

		/**************** ROUTINE DE MERGING  ***************************/
		virtual void merge(Grid* other);

		/**************** ROUTINE D'AFFICHAGE ***************************/
		virtual void display();

		/**************** ROUTINE D'EXPORT EN OBJ ***********************/
		virtual void exportOBJ(const char* filename);

	protected :

		 Vector3f m_center, m_min, m_max;
		 float m_dx, m_dy, m_dz;
		 float m_sizeX, m_sizeY, m_sizeZ;
		 int m_nx, m_ny, m_nz, m_n;
		 vector<Vector3f> m_pos;

		 Vector4f color;
		 float angleRotation;
};
#endif
/****************************************************************************/
/****************************************************************************/
