#version 410 core

struct Material //Propriedades do material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
uniform vec3 color;

in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragUV;
in vec3 light;
in vec3 tanViewer;
uniform sampler2D sampler; //Textura difusa
uniform sampler2D normalsampler; // Textura de mapa de normal
out vec3 finalColor; // Cor final do objeto

vec3 expand(vec3 v)
{
   return (v - 0.5) * 2;
}

void main()
{
    vec3 ambient = material.ambient * texture(sampler, fragUV).rgb; //Componente da luz ambiente
    vec3 diffuse = vec3(0.0,0.0,0.0);
    vec3 specular = vec3(0.0,0.0,0.0);

    //Normal usada é a de textura de mapa de normal
    vec3 N = normalize(expand(texture(normalsampler,fragUV).rgb));

    //Normalizando novamente a luz no espaço do olho
    vec3 L = normalize(light);

    //Calcula produto interno entre luz e normal no espaço do olho
    float iDif = dot(light,N);

    //Se certifica que a luz e a normal não são perpendiculares
    if( iDif > 0 )
    {
        diffuse = iDif * material.diffuse * texture(sampler, fragUV).rgb;// Calcula componente difusa da luz

        vec3 V = tanViewer;/*normalize(-fragPos);*/ // Viewer
        vec3 H = normalize(L + V);

        float iSpec = pow(max(dot(N,H),0.0),material.shininess);
        specular = iSpec * material.specular; //Calcula componente especular
    }

    finalColor = ambient + diffuse + specular;

}
