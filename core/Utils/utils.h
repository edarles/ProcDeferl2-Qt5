#include <GL/glew.h>
#include <stdio.h>
#include <math.h>

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

