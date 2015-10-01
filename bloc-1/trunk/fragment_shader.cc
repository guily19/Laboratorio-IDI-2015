#version 330 core

in vec4 fcolor;
out vec4 fragmentColor;
//VARIABLES ESPECIALS
// gl_FragCoord -> quina posicio dins de la pantalla ocupa el pixel vec4 (x,y,z,?)
// gl_FragDepth

// discard -> aquell pixel no executa el FragmentShader

void main()
{
    fragmentColor = fcolor;
    //FragColor = vec4(1.);

    /*
    if(int(gl_FragCoord.y) % 8 == 0) discard;

    if(gl_FragCoord.x < 400. && gl_FragCoord.y < 400.){
        FragColor = vec4(1.,0.,0.,1.);
    }else if(gl_FragCoord.x < 400. && gl_FragCoord.y > 400.){
        FragColor = vec4(0.,1.,0.,1.);
    } else if(gl_FragCoord.x > 400. && gl_FragCoord.y < 400.){
        FragColor = vec4(0.,0.,1.,1.);
    } else {
        FragColor = vec4(0.,0.,0.,1.);
    }
    */
}
