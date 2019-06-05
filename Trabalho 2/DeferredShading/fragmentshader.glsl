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

uniform sampler2D gTangente;
//uniform sampler2D gDepth;
uniform sampler2D gNormal;
uniform sampler2D gPosition;
uniform sampler2D gTex;
//uniform sampler2D normalSampler;
//uniform sampler2D gLight;
//uniform sampler2D gTanViewer;
uniform vec3 lightPos; // Posição da luz em coordenada do olho


struct Light {
    vec3 Position;
    vec3 Color;
};
const int NR_LIGHTS = 20;
uniform Light lights[NR_LIGHTS];

uniform int mode;
float linearizeDepth(vec2 uv)
{
    float zNear = 0.1;
    float zFar  = 1000.0;
    float depth = 0;/*texture2D(gDepth, uv).x;*/
    return (2.0 * zNear) / (zFar + zNear - depth * (zFar - zNear));
}

vec3 expand(vec3 v)
{
   return (v - 0.5) * 2;
}

void main()
{
    if(mode == 0)
    {
        //Fazendo com bump
        vec3 fragPos = texture(gPosition,UV).rgb;
        vec3 fragNormal = normalize(texture(gNormal,UV).rgb);
        vec3 fragTang = texture(gTangente,UV).rgb;

        //Calculo de espaço tangente
        //Bitangente no espaço do olho
        vec3 bitangentVertexEye = (cross(fragNormal,fragTang));

        //Matriz de rotação tbn para transformar luz para o eapaço tangente
        mat3 rotation = transpose(mat3(fragTang,bitangentVertexEye,fragNormal));

        //Viewer no espaco tangente
        vec3 tanViewer = rotation*(-fragPos);

        vec3 ambient = material.ambient;//Componente da luz ambiente
        vec3 specular = vec3(0.0,0.0,0.0);

        //Normal usada eh a de textura de mapa de normal
        vec3 N = normalize(expand(texture(gTex,UV).rgb));

        //Viewer
        vec3 V = normalize(tanViewer);

       // finalColor += specular;

        float constant = 1.0;
        float linear = 0.09;
        float quadratic = 0.0032;
        finalColor = vec3(0,0,0);
        for(int i = 0; i < 20; i++)
        {
            //Colocando luz no espaco tangente
            vec3 tanLight = rotation*normalize(lights[i].Position - fragPos);


            //Normalizando novamente a luz no espaço tangente
            vec3 L = normalize(tanLight);

            float distance = length(lights[i].Position - fragPos);
            float attenuation = 1.0/(constant + linear*distance + quadratic*(distance*distance));

            //Calcula produto interno entre luz e normal no espaco tangente
            float iDif = dot(L,N);

            //Calcula componente difusa da luz
            vec3 diffuse = max(0,iDif) * material.diffuse * lights[i].Color;

            finalColor += attenuation * (ambient * lights[i].Color + diffuse );

            //HalfVector
            vec3 H = normalize(L + V);
            vec3 r = normalize(reflect(-L, N));

            float iSpec = abs(pow(max(dot(V, r), 0.0), material.shininess));

            //Calcula componente especular
            specular = iSpec * material.specular * lights[i].Color;

            finalColor += specular * attenuation;
        }
//        vec3 fragTang = texture(gTangente,UV).rgb;
//        finalColor = length(fragTang) == 0 ? vec3(1, 0, 0) : vec3(0, 1, 0);

//        vec3 fragPos = texture(gPosition,UV).rgb;
//        finalColor = fragPos;
//        vec3 fragNormal = texture(gNormal,UV).rgb;
//        finalColor = length(fragNormal) == 0 ? vec3(1, 0, 0) : vec3(0, 1, 0);

//        vec3 fragTex = texture(gTex,UV).rgb;
//        finalColor = fragTex;


    }
    else if (mode == 1)
    {
        //Iluminação simples de Phong
        vec3 fragPos = texture(gPosition,UV).rgb;
        vec3 fragNormal = texture(gNormal,UV).rgb;

        //Normal usada eh a de textura de mapa de normal
        vec3 N = normalize(fragNormal);
        //Viewer
        vec3 V = normalize(-fragPos);
        float constant = 1;
        float linear = 0.0009;
        float quadratic = 0.0032;

        finalColor = vec3(0,0,0);

        vec3 ambient = material.ambient;//Componente da luz ambiente
        vec3 specular = vec3(0.0,0.0,0.0);
        for(int i = 0; i < 1; i++)
        {
            //Normalizando novamente a direção da luz
            vec3 L = normalize(lightPos - fragPos);

            float distance = length(lightPos - fragPos);
            float attenuation = 1.0/(constant + linear*distance + quadratic*(distance*distance));
           // float attenuation = 1;
            //Calcula produto interno entre luz e normal
            float iDif = dot(L,N);

            //Calcula componente difusa da luz
            vec3 diffuse = max(0,iDif) * material.diffuse * lights[i].Color;

            finalColor += (ambient* lights[i].Color + diffuse ) * attenuation;

            //HalfVector
            vec3 H = normalize(L + V);
            //Se certifica que a luz e a normal nao sao perpendiculares
            if( iDif > 0 )
            {
                //HalfVector
                vec3 r = normalize(reflect(-L, N));

                float iSpec = pow(max(dot(r,V),0.0),material.shininess);

                //Calcula componente especular
                specular = iSpec * material.specular * lights[i].Color;
            }

            finalColor += specular*attenuation;

        }
    }
    else if (mode == 2)
    {
        finalColor = texture(gPosition,UV).rgb;
    }
    else if (mode == 3)
    {
        vec3 fragNormal= texture(gNormal,UV).rgb ;
        finalColor = length(fragNormal) == 0 ? vec3(1, 0, 0) : vec3(0, 1, 0);
    }
    else if (mode == 4)
    {
        float d = linearizeDepth(UV);
        finalColor = vec3(d);
    }
    else if (mode == 5)
    {
//        finalColor = texture(gTangente,UV).rgb ;
        vec3 fragTang = texture(gTangente,UV).rgb ;
        finalColor = length(fragTang) == 0 ? vec3(1, 0, 0) : vec3(0, 1, 0);

    }
    else if(mode == 6)
    {
        //Fazendo com bump
        vec3 fragPos = texture(gPosition, UV).rgb;
        vec3 fragNormal = normalize(texture(gNormal, UV).rgb);
        vec3 fragTang = normalize(texture(gTangente, UV).rgb);

        //finalColor = (length(fragTang) == 0.0) ? vec3(1, 0, 0) : vec3(0, 1, 0);
        //return;

        //Calculo de espaço tangente
        //Bitangente no espaço do olho
        vec3 bitangentVertexEye = cross(fragNormal,fragTang);

        //Matriz de rotação tbn para transformar luz para o eapaço tangente
        mat3 rotation = transpose(mat3(fragTang,bitangentVertexEye,fragNormal));

        //Colocando luz no espaco tangente
        vec3 tanLight = rotation*normalize(lightPos - fragPos);

        //Viewer no espaco tangente
        vec3 tanViewer = normalize(rotation*(-fragPos));

        vec3 ambient = material.ambient;//Componente da luz ambiente
        vec3 specular = vec3(0.0,0.0,0.0);

        //Normal usada eh a de textura de mapa de normal
        vec3 N = normalize(expand(texture(gTex,UV).rgb));

        //Normalizando novamente a luz no espaço tangente
        vec3 L = tanLight;

        //Calcula produto interno entre luz e normal no espaco tangente
        float iDif = dot(tanLight,N);

        //Calcula componente difusa da luz
        vec3 diffuse = max(0,iDif) * material.diffuse;

        finalColor = ambient + diffuse;

        //Se certifica que a luz e a normal nao sao perpendiculares
        if( iDif > 0 )
        {
            //Viewer
            vec3 V = tanViewer;

            //HalfVector
            vec3 H = normalize(L + V);

            vec3 r = normalize(reflect(-L, N));

            float iSpec = abs(pow(max(dot(V,r),0.0),material.shininess));

            //Calcula componente especular
            specular = iSpec * material.specular;
        }

        finalColor += specular;
    }
}
