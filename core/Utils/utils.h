#include <GL/glew.h>
#include <iostream>
#include <math.h>
#include <QImage>
using namespace std;

#pragma once
#define MAX_PARTICLES 100000

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

	glDrawArrays(GL_QUADS, 0, nb);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

        glUseProgram(0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);

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

	glDrawArrays(GL_QUADS, 0, nb);
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

        glUseProgram(0);

	glLineWidth(1.0);
}
