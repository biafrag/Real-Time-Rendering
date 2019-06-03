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

in vec2 UV;
out vec3 finalColor; // Cor final do objeto

uniform sampler2D gDepth;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gTangente;

uniform vec3 lightPos; // Posição da luz em coordenada do olho

float linearizeDepth(vec2 uv)
{
    float zNear = 0.1;
    float zFar  = 1000.0;
    float depth = texture2D(gDepth, uv).x;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

void main()
{
//    float d;
//    d = linearizeDepth(UV);
//    finalColor = vec3(d,d,d);
    vec3 fragPos = texture(gPosition,UV).rgb;
    vec3 fragNormal = texture(gNormal,UV).rgb;
    vec3 fragTang = texture(gTangente,UV).rgb;

    vec3 ambient = material.ambient;//Componente da luz ambiente
    vec3 specular = vec3(0.0,0.0,0.0);

    //Normal usada eh a de textura de mapa de normal
    vec3 N = fragNormal /*normalize(expand(texture(normalsampler,fragUV).rgb))*/;

    //Normalizando novamente a luz no espaço tangente
    vec3 L = normalize(lightPos - fragPos);

    //Calcula produto interno entre luz e normal no espaco tangente
    float iDif = dot(L,N);

    //Calcula componente difusa da luz
    vec3 diffuse = max(0,iDif) * material.diffuse;

    finalColor = ambient + diffuse;

    //Se certifica que a luz e a normal nao sao perpendiculares
    if( iDif > 0 )
    {
        //Viewer
        vec3 V = -fragPos/*tanViewer*/;

        //HalfVector
        vec3 H = normalize(L + V);

        float iSpec = pow(max(dot(N,H),0.0),material.shininess);

        //Calcula componente especular
        specular = iSpec * material.specular;
    }

    finalColor += specular;


    //finalColor = texture(gNormal,UV).rgb;

}
