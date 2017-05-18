#include <GL/glew.h>
#include <iostream>
#include <math.h>
#include <QImage>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

#pragma once

#define MAX_PARTICLES 100000

// ACTIVATE SPRAYS GENERATION
#define SPRAYS 1
#define LIFETIME_MAX 100

// ACTIVATE FBM
#define FBM 0

// FBM PARAMETERS
#define GAIN 0.65
#define OCTAVES 20
#define SCALE_AMP 4

const Vector3f gravity = Vector3f(0,-9.81,0);

// fast 1/sqrtf(number) -> ONLY VALID for IEEE754simple precision float
#pragma once
inline float Q_rsqrt( float number )
{
  long i; float x2, y;
  const float threehalfs = 1.5f;

  x2 = number * 0.5f;
  y  = number;
  i  = * ( long * ) &y;
  i  = 0x5f3759df - ( i >> 1 );
  y  = * ( float * ) &i;
  y  = y * ( threehalfs - ( x2 * y * y ) ); 
  return y;
}

#pragma once
inline GLuint _compileProgram(const char *vsource, const char *fsource)
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertexShader, 1, &vsource, 0);
    glShaderSource(fragmentShader, 1, &fsource, 0);
    
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    // check if program linked
    GLint success = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        char temp[256];
	printf("not succes:%d\n",success);
        glGetProgramInfoLog(program, 256, 0, temp);
        printf("Failed to link program:\n%s\n", temp);
        glDeleteProgram(program);
        program = 0;
    }
    else
	printf("succes:%d\n",success);
    return program;
}

#pragma once
inline void displaySpheres(GLuint m_program, float* pos, float* colors, int nbPos)
{
	glEnable(GL_POINT_SPRITE_ARB);
	glEnable(GL_BLEND);                               
   	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);  // how transparency acts
       	glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);
        glDepthMask(GL_TRUE);
        glEnable(GL_DEPTH_TEST);
			
        glUseProgram(m_program);
        glUniform1f( glGetUniformLocation(m_program, "pointScale"), 768 / tanf(60*0.5f*(float)M_PI/180.0f) );
	glUniform1f( glGetUniformLocation(m_program, "near"), 0.000000001 );
	glUniform1f( glGetUniformLocation(m_program, "far"), 0.00001 );

   	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glColor3f(1,1,1);
	glEnableClientState(GL_VERTEX_ARRAY); 
	glEnableClientState(GL_COLOR_ARRAY);
	
	glColorPointer(4, GL_FLOAT, 0, colors);
        glVertexPointer(4, GL_FLOAT, 0, pos);
	
	glDrawArrays(GL_POINTS, 0, nbPos);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	
        glUseProgram(0);
	glDisable(GL_BLEND);
        glDisable(GL_POINT_SPRITE_ARB);
}

#pragma once
inline void displayPatchesWithTexture(GLuint m_program, GLuint tex1, QImage texBubbles, float* pos, float* colors, float* uv, int nb)
{
     glEnable(GL_LIGHTING);
     glEnable(GL_LIGHT0);
     glEnable(GL_NORMALIZE);
     
     // Light model parameters:
     // -------------------------------------------
     
     GLfloat lmKa[] = {0.0, 0.0, 0.0, 0.0 };
     glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmKa);
     
     glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0);
     glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 0.0);
     
     // -------------------------------------------
     // Spotlight Attenuation
     
     GLfloat spot_direction[] = {1.0, -1.0, -1.0 };
     GLint spot_exponent = 30;
     GLint spot_cutoff = 180;
     
     glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
     glLighti(GL_LIGHT0, GL_SPOT_EXPONENT, spot_exponent);
     glLighti(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff);
    
     GLfloat Kc = 1.0;
     GLfloat Kl = 0.0;
     GLfloat Kq = 0.0;
     
     glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,Kc);
     glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, Kl);
     glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, Kq);
     
     
     // ------------------------------------------- 
     // Lighting parameters:

     GLfloat light_pos[] = {0.0f, 5.0f, 5.0f, 1.0f};
     GLfloat light_Ka[]  = {1.0f, 0.5f, 0.5f, 1.0f};
     GLfloat light_Kd[]  = {1.0f, 0.1f, 0.1f, 1.0f};
     GLfloat light_Ks[]  = {1.0f, 1.0f, 1.0f, 1.0f};

     glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
     glLightfv(GL_LIGHT0, GL_AMBIENT, light_Ka);
     glLightfv(GL_LIGHT0, GL_DIFFUSE, light_Kd);
     glLightfv(GL_LIGHT0, GL_SPECULAR, light_Ks);

     // -------------------------------------------
     // Material parameters:

     GLfloat material_Ka[] = {0.5f, 0.0f, 0.0f, 1.0f};
     GLfloat material_Kd[] = {0.4f, 0.4f, 0.5f, 1.0f};
     GLfloat material_Ks[] = {0.8f, 0.8f, 0.0f, 1.0f};
     GLfloat material_Ke[] = {0.1f, 0.0f, 0.0f, 0.0f};
     GLfloat material_Se = 20.0f;

     glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_Ka);
     glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_Kd);
     glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_Ks);
     glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, material_Ke);
     glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, material_Se);

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	// Binding de la texture pour pouvoir la modifier.
	glBindTexture(GL_TEXTURE_2D, tex1);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	glTexImage2D(GL_TEXTURE_2D, 0, 4, texBubbles.width(), texBubbles.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texBubbles.bits());

	//glActiveTexture(GL_TEXTURE0);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
       	
        glUseProgram(m_program);
      
	glUniform1i(glGetUniformLocation(m_program,"texture0"), 0);

	glEnableClientState(GL_VERTEX_ARRAY); 
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);


	glVertexPointer(3, GL_FLOAT, 0, pos);
	glColorPointer(3, GL_FLOAT, 0, colors);
	glTexCoordPointer(2,GL_FLOAT,0, uv);

	glDrawArrays(GL_TRIANGLES, 0, nb);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glUseProgram(0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);

	glDisable(GL_LIGHTING);
     	glDisable(GL_LIGHT0);
     	glDisable(GL_NORMALIZE);

}

#pragma once
inline void displayPatchesWithoutTexture(GLuint m_program, float* pos, float* colors, int nb)
{
	glLineWidth(2.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        		
        glUseProgram(m_program);
      
	glEnableClientState(GL_VERTEX_ARRAY); 
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, pos);
	glColorPointer(3, GL_FLOAT, 0, colors);

	glDrawArrays(GL_TRIANGLES, 0, nb);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

        glUseProgram(0);

	glLineWidth(1.0);
}
