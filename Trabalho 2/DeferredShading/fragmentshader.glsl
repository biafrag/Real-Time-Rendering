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

in vec2 UV; //Coordenadas de textura do quad
out vec3 finalColor; // Cor final do objeto

uniform sampler2D gTangente; //Textura de tangente no espaço do olho
uniform sampler2D gNormal;  //Textura de normal no espaço do olho
uniform sampler2D gPosition; //Textura de posição no espaço do olho
uniform sampler2D gTex; //Textura de textura de bump mapeada e expandida
uniform sampler2D gDepth; //Textura de profundidade

uniform vec3 lightPos; // Posição de uma luz em coordenada do olho

struct Light {
    vec3 Position;
    vec3 Color;
};
const int NR_LIGHTS = 20;
uniform Light lights[NR_LIGHTS]; //Vetor de luzes na posição do olho

uniform int mode; //Variável que define qual vai ser a imagem exibida

//Função para linearizar profundidade para exibir na tela
float linearizeDepth(vec2 uv)
{
    float zNear = 0.1;
    float zFar  = 100.0;
    float depth = texture2D(gDepth, uv).x;
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

void main()
{
    vec3 fragPos = texture(gPosition,UV).rgb;
    vec3 fragNormal = normalize(texture(gNormal,UV).rgb);
    vec3 fragTang = normalize(texture(gTangente,UV).rgb);

    //Definindo variáveis para cálculo de atenuação
    float constant = 1.0;
    float linear = 0.09;
    float quadratic = 0.0032;

    //Modo para ver resultado com bump e com luzes coloridas se mexendo
    if(mode == 0)
    {
        //Calculo de espaço tangente

        //Bitangente no espaço do olho
        vec3 bitangentVertexEye = (cross(fragNormal,fragTang));

        //Matriz de rotação tbn para transformar luz para o eapaço tangente
        mat3 rotation = transpose(mat3(fragTang,bitangentVertexEye,fragNormal));

        //Viewer no espaco tangente
        vec3 V = normalize(rotation*(-fragPos));

        vec3 ambient = material.ambient; //Componente da luz ambiente

        //Normal usada eh a de textura de mapa de normal
        vec3 N = normalize(texture(gTex,UV).rgb);

        finalColor = vec3(0,0,0);

        //Loop para contabilizar todas as luzes
        for(int i = 0; i < 20; i++)
        {
            //Colocando luz no espaco tangente
            vec3 L = normalize(rotation*normalize(lights[i].Position - fragPos));

            float distance = length(lights[i].Position - fragPos);
            float attenuation = 1.0/(constant + linear*distance + quadratic*(distance*distance));

            //Calcula produto interno entre luz e normal no espaco tangente
            float iDif = dot(L,N);

            //Calcula componente difusa da luz
            vec3 diffuse = max(0,iDif) * material.diffuse ;

            finalColor += attenuation * lights[i].Color *(ambient  + diffuse );

            //Calculando especular
            vec3 r = normalize(reflect(-L, N));

            float iSpec = abs(pow(max(dot(V, r), 0.0), material.shininess));

            //Calcula componente especular
            vec3 specular = iSpec * material.specular * lights[i].Color;

            finalColor += specular * attenuation;
        }

    }

    //Modo para ver buffer de posição
    else if (mode == 1)
    {
        finalColor = texture(gPosition,UV).rgb;
    }

    //Modo para ver buffer de normal
    else if (mode == 2)
    {
        finalColor = texture(gNormal,UV).rgb ;
    }

    //Modo para ver buffer de profundidade
    else if (mode == 3)
    {
        float d = linearizeDepth(UV);
        finalColor = vec3(d);
    }

    //Modo para ver buffer de tangente
    else if (mode == 4)
    {
        finalColor = texture(gTangente,UV).rgb ;
    }

    //Modo para ver buffer de bump
    else if(mode == 5)
    {
        finalColor = texture(gTex,UV).rgb ;
    }
}
