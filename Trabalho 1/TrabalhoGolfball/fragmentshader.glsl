#version 410 core

struct Material //Propriedades do material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

//Variaveis de entrada
in vec2 fragUV; //Coordenada de textura
in vec3 light; //Posição da luz no espaço tangente
in vec3 tanViewer; // Posição do viewer no espaço tangente
in vec3 tang;
in vec3 fragNormal;
in vec3 fragPos;
uniform sampler2D sampler; //Textura difusa
uniform sampler2D normalsampler; // Textura de mapa de normal
uniform bool hasDT; //Variavel que diz se vai ter textura difusa ou não
out vec3 finalColor; // Cor final do objeto

vec3 expand(vec3 v)
{
   return (v - 0.5) * 2;
}

void main()
{
//    vec3 ambient = material.ambient;//Componente da luz ambiente
//    vec3 specular = vec3(0.0,0.0,0.0);

//    //Normal usada eh a de textura de mapa de normal
//    vec3 N = normalize(expand(texture(normalsampler,fragUV).rgb));

//    //Normalizando novamente a luz no espaço tangente
//    vec3 L = normalize(light);

//    //Calcula produto interno entre luz e normal no espaco tangente
//    float iDif = dot(light,N);

//    //Calcula componente difusa da luz
//    vec3 diffuse = max(0,iDif) * material.diffuse;

//    finalColor = ambient + diffuse;

//    //Checa se tem textura difusa ou não
//    if(hasDT == true)
//    {
//        finalColor *= texture(sampler, fragUV).rgb;
//    }

//    //Se certifica que a luz e a normal nao sao perpendiculares
//    if( iDif > 0 )
//    {
//        //Viewer
//        vec3 V = tanViewer;

//        //HalfVector
//        vec3 H = normalize(L + V);

//        float iSpec = pow(max(dot(N,H),0.0),material.shininess);

//        //Calcula componente especular
//        specular = iSpec * material.specular;
//    }

//    finalColor += specular;

    finalColor = vec3(1,1,1)*noise3(fragPos) + vec3(0.5,0,0.5);

}
