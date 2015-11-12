#version 330 core

in vec3 matambF;
in vec3 matdiffF;
in vec3 matspecF;
in float matshinF;

in vec4 vertexSCO;
in vec3 normalSCO;


out vec4 FragColor;

// Valors per als components que necessitem dels focus de llum
// vec3 colFocus = vec3(0.8, 0.8, 0.8);
vec3 llumAmbient = vec3(0.2, 0.2, 0.2);
vec3 posFocus = vec3(1, 1, 1); 

vec3 posFocusCamera = vec3(0.0, 0.0, 0.0);  // en SCA
vec3 posFocusFix = vec3(1.0, 1.0 , 1.0);

vec3 colorLlum;


uniform vec3 llumFixa;
uniform vec3 llumCamera;

vec3 Lambert (vec3 NormSCO, vec3 L) 
{
    // S'assumeix que els vectors que es reben com a paràmetres estan normalitzats
    // Inicialitzem color a component ambient
    vec3 colRes = llumAmbient * matambF;

    // Afegim component difusa, si n'hi hax
    if (dot (L, NormSCO) > 0)
      colRes = colRes + colorLlum * matdiffF * dot (L, NormSCO);
    return (colRes);
}

vec3 Phong (vec3 NormSCO, vec3 L, vec4 vertSCO) 
{
    // Els vectors estan normalitzats

    // Inicialitzem color a Lambert
    vec3 colRes = Lambert (NormSCO, L);

    // Calculem R i V
    if (dot(NormSCO,L) < 0)
      return colRes;  // no hi ha component especular

    vec3 R = reflect(-L, NormSCO); // equival a: normalize (2.0*dot(NormSCO,L)*NormSCO - L);
    vec3 V = normalize(-vertSCO.xyz);

    if ((dot(R, V) < 0) || (matshinF == 0))
      return colRes;  // no hi ha component especular
    
    // Afegim la component especular
    float shine = pow(max(0.0, dot(R, V)), matshinF);
    return (colRes + matspecF * colorLlum * shine); 
}



void main()
{	
	vec3 focusSCA = posFocusCamera;
    vec4 focusSCO =  vec4(focusSCA,1.0);
    vec4 L = normalize(focusSCO - vertexSCO);

    vec3 color1, color2;

    colorLlum = llumFixa;
    color1 = Phong(normalize(normalSCO),L.xyz, vertexSCO);

    focusSCA = posFocusFix;
    focusSCO =  vec4(focusSCA,1.0);
    L = normalize(focusSCO - vertexSCO);
    
    colorLlum = llumCamera;
    color2 = Phong(normalize(normalSCO),L.xyz, vertexSCO);

    vec3 fcolor = color1 + color2;


	FragColor = vec4(fcolor,1);	
}
