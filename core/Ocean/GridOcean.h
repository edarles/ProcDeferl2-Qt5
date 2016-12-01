/****************************************************************************/
/****************************************************************************/
#ifndef _GRID_OCEAN_H__
#define _GRID_OCEAN_H__

#include <Grid.h>
#include <WaveGroup.h>
#include <vector>
#include <QImage>
#include <Sprays.h>

using namespace std;

class BreakingWave;

class GridOcean : public Grid
{
/****************************************************************************/
	public : 

		GridOcean();
		GridOcean(Vector3f min, Vector3f max, float dx, float dz);
		virtual ~GridOcean();

		/**************** GETTERS ***************************/
		Vector3f    getInitPos(int ix, int iz);
		Vector3f    getInitPos(int indexCell);
		Vector3f    getVel(int ix, int iz);
		Vector3f    getVel(int indexCell);
		Vector3f    getDVel(int ix, int iz);
		Vector3f    getDVel(int indexCell);
		float       getT();
		float       getTx();
		float       getTz();
		QImage      getTexBubbles();
		float       getLifeTimeBubbles(int index);

		Sprays*     getSprays();
		
		/**************** SETTERS ***************************/
		void setInitPos(int ix, int iz, Vector3f initPos);
		void setVel(int ix, int iz, Vector3f vel);
		void setDVel(int ix, int iz, Vector3f dVel);
		void setT(float t);
		void setTexBubbles(QImage tex);
		void setLifeTimeBubbles(int index, float lifeTime);

		/**************** ROUTINE DE REINITIALISATION *******/
		void init();
		void reinitPos();
		
		/**************** ROUTINE DE MAJ ********************/
		void update(vector<WaveGroup*> waveGroups, float dt);

		/**************** ROUTINE DE GENERATION DE VAGUES DEFERLANTES ******/
		void generateBreakingWaves(vector<WaveGroup*> waveGroups, vector<BreakingWave*> *breakingWaves, float dt);

		/**************** ROUTINE DE RELAXATION DE LA MOUSSE ************/
		void relaxBubbles(int ix, int iz);

		/**************** SURCHARGE DE LA METHODE D'EXPORT AU FORMAT OBJ ***/
		void exportOBJ(const char* filename);

		/**************** SURCHARGE METHODE DISPLAY (AVEC TEXTURE) ******/
		void  display();

/****************************************************************************/
	protected : 

		 vector<Vector3f> m_initPos;
		 vector<Vector3f> m_vel;
		 vector<Vector3f> m_dVel; // m_dVel = pentes 
		 vector<float>    lifeTimeBubbles;

		 vector<Vector3f> bordersA, bordersB, bordersC, bordersD, bordersE;
		 int m_ny_border;

		 float m_t;
		 float tx, tz;

		 int nb;
		 float* posDisplay, *colors, *uv;
		 static GLuint m_program1, m_program2, tex1;

		 QImage texBubbles, texOcean;
		 void initTexture();

		 void storePolygon(int i, int j, int nb);
		 void storeUV(int i, int j, int nb);
		 void storeColor(int nb);

		 void createBorders();
		 void displayBorder(vector<Vector3f> border, int nl1, int nl2);
		 void displayBorders();

		 Sprays *sprays;
};
#endif
/****************************************************************************/
/****************************************************************************/
