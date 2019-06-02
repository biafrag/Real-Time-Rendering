#version 410 core

////Variaveis de entrada
//in vec2 fragUV; //Coordenada de textura
//in vec3 light; //Posição da luz no espaço tangente
//in vec3 tanViewer; // Posição do viewer no espaço tangente
//in vec3 fragNormal;

//uniform sampler2D sampler; //Textura difusa
//uniform sampler2D normalsampler; // Textura de mapa de normal
//uniform bool hasDT; //Variavel que diz se vai ter textura difusa ou não

in vec2 UV;
out vec4 finalColor; // Cor final do objeto

//uniform vec3 lightPos; // Posição da luz em coordenada do olho

//vec3 expand(vec3 v)
//{
//   return (v - 0.5) * 2;
//}
//uniform sampler2D gPosition;
//uniform sampler2D gNormal;
//uniform sampler2D gDifusa;
uniform sampler2D gDepth;

float linearizeDepth(vec2 uv)
{
    float zNear = 0.1;
    float zFar  = 1000.0;
    float depth = texture2D(gDepth, uv).x;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

void main()
{

    // retrieve data from G-buffer
//    vec2 TexCoords =
//    vec3 FragPos = texture(gPosition, TexCoords).rgb;
//    vec3 Normal = texture(gNormal, TexCoords).rgb;
//    vec3 Ambiente = texture(gAmbiente, TexCoords).rgb;
//    vec3 Difusa = texture(gDifusa, TexCoords).rgb;
//    vec3 Especular = texture(gSpecShi, TexCoords).rgb;
//    float shi = texture(gSpecShi, TexCoords).a;



//    vec3 ambient = Ambiente;//Componente da luz ambiente
//    vec3 specular = vec3(0.0,0.0,0.0);

//    //Normal usada eh a de textura de mapa de normal
//    vec3 N = fragNormal;/*normalize(expand(texture(normalsampler,fragUV).rgb))*/;

//    //Normalizando novamente a luz no espaço tangente
//    vec3 L = normalize(light);

//    //Calcula produto interno entre luz e normal no espaco tangente
//    float iDif = dot(light,N);

//    //Calcula componente difusa da luz
//    vec3 diffuse = max(0,iDif) * Difusa;

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
//        //vec3 V = tanViewer;

//        //HalfVector
//        vec3 H = normalize(L + V);

//        float iSpec = pow(max(dot(N,H),0.0),shi);

//        //Calcula componente especular
//        specular = iSpec * Especular;
//    }
    float d;
    if (UV.x < 0.5) // left part
      d = linearizeDepth(UV);
    else // right part
      d = texture2D(gDepth, UV).x;
    finalColor = vec4(d,d,d,1.0);
    //finalColor = vec3(0,1,0.5);
//    finalColor = vec4(texture(gDepth, UV).rgb,1);

}
