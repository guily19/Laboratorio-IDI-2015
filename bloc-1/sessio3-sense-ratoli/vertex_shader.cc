#version 330 core

in vec3 vertex;
uniform float val;
uniform mat4 TG;
in vec4 color;

out vec4 fcolor;

void main()
{

    //Aquesta variable sempre a de seri (gl_Position)
    //La funcio posa un 1 a l'ultim parametre
    //cordenades homogenies es fan per poder operar sempre amb multiplicacions
    //gl_Position = vec4(vertex * val,1.0);
    fcolor = color;
    gl_Position = TG * vec4 (vertex, 1.0);
    //color = vec3(1.);
}
