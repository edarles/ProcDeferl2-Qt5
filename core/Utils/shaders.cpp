#define STRINGIFY(A) #A
#include <stdio.h>
// vertex shader

const char *sphereVertexShader = STRINGIFY(
//uniform float pointRadius;  // point size in world space
uniform float pointScale;   // scale to calculate size in pixels
void main()
{
    // calculate window-space point size
    vec3 posEye = vec3(gl_ModelViewMatrix * vec4(gl_Vertex.xyz, 1.0));
    float dist = length(posEye);
    gl_PointSize = gl_Vertex.w* (pointScale / dist);
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);
    gl_FrontColor = gl_Color;
}
);

// pixel shader for rendering points as shaded spheres
const char *spherePixelShader = STRINGIFY(

void main()
{   
    const vec3 lightDir = vec3(0.577, 0.577, 0.577);
    // calculate normal from texture coordinates
    vec3 N;
    N.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
    float mag = dot(N.xy, N.xy);
    if (mag > 1.0) discard;   // kill pixels outside circle
    N.z = sqrt(1.0-mag);
    // calculate lighting
    float diffuse = max(0.0, dot(lightDir, N));
    gl_FragColor = gl_Color * diffuse;

}
);

const char *triangleVertexShader = STRINGIFY(
varying vec3 normal;
varying vec3 position;
void main()
{
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = ftransform();//gl_ModelViewProjectionMatrix * gl_Vertex;  
    gl_FrontColor = gl_Color;
    normal        = normalize(gl_NormalMatrix * gl_Normal);
    position      = vec3(gl_ModelViewMatrix * gl_Vertex);
}
);

// pixel shader for rendering quad
const char *trianglePixelShaderWithTexture = STRINGIFY(
uniform sampler2D texture0;
void main()
{   
    const vec3 lightDir = vec3(0.577, 0.577, 0.577);
    //calculate normal from texture coordinates
    vec3 N;
    N.xy = gl_TexCoord[0].xy*vec2(2.0, -2.0) + vec2(-1.0, 1.0);
   
    // calculate lighting
    float diffuse = max(0.0, dot(lightDir, N));
   
    vec4 colorT = texture2D(texture0,gl_TexCoord[0].st);
    colorT.x = colorT.w*colorT.x; colorT.y = colorT.w*colorT.y; colorT.z = colorT.w*colorT.z;  
    gl_FragColor = colorT;// (gl_Color + colorT);//*diffuse;
}
);

// pixel shader for rendering quad
const char *trianglePixelShaderWithoutTexture = STRINGIFY(
void main()
{   
    gl_FragColor = gl_Color;

}
);


