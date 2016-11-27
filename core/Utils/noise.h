#include    <stdlib.h>
#include    <math.h>

extern float noise(int x, int y); 
extern float interpolate(float a, float b, float x);
extern float   smooth_noise(int x, int y);
extern float       noise_handler(float x, float y);
extern float perlin_two(float x, float y, float gain, int octaves, int hgrid);
