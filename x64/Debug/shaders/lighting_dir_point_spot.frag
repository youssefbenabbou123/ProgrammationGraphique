//-----------------------------------------------------------------------------
// Fragment shader pour plusieurs lumières
//
// REMARQUE :
// Ce code de shader n'est pas le plus efficace, mais il permet de comprendre le principe
// et devrait être facile à suivre. Les mêmes calculs diffus et spéculaires 
// sont effectués trois fois séparément. Cela pourrait être optimisé pour ne 
// le calculer qu'une seule fois, avec des atténuations et des multiplicateurs de 
// projecteur appliqués.
//-----------------------------------------------------------------------------

#version 330 core

struct Material 
{
    vec3 ambient;       // Couleur ambiante
    sampler2D diffuseMap; // Carte de texture diffuse
    vec3 specular;      // Couleur spéculaire
    float shininess;    // Brillance
};

struct DirectionalLight
{
	vec3 direction;     // Direction de la lumière
	vec3 ambient;       // Couleur ambiante
	vec3 diffuse;       // Couleur diffuse
	vec3 specular;      // Couleur spéculaire
};

struct PointLight
{
	vec3 position;      // Position de la lumière ponctuelle
	vec3 ambient;       // Couleur ambiante
	vec3 diffuse;       // Couleur diffuse
	vec3 specular;      // Couleur spéculaire

	float constant;     // Atténuation constante
	float linear;       // Atténuation linéaire
	float exponent;     // Atténuation exponentielle
};

struct SpotLight
{
	vec3 position;      // Position de la lumière spot
	vec3 direction;     // Direction du spot
	float cosInnerCone; // Angle interne du cône
	float cosOuterCone; // Angle externe du cône
	vec3 ambient;       // Couleur ambiante
	vec3 diffuse;       // Couleur diffuse
	vec3 specular;      // Couleur spéculaire
	int on;             // État de la lumière (allumée ou éteinte)

	float constant;     // Atténuation constante
	float linear;       // Atténuation linéaire
	float exponent;     // Atténuation exponentielle
};

in vec2 TexCoord;       // Coordonnées de texture
in vec3 FragPos;        // Position du fragment
in vec3 Normal;         // Normale du fragment

#define MAX_POINT_LIGHTS 26 // Nombre maximum de lumieres ponctuelles
#define MAX_HEADLIGHTS 8     // 2 phares x 4 voitures en circulation

uniform DirectionalLight sunLight;          // Lumière directionnelle (soleil)
uniform PointLight pointLights[MAX_POINT_LIGHTS]; // Lumières ponctuelles
uniform SpotLight spotLight;                // Lumière spot (torche camera)
uniform SpotLight headlights[MAX_HEADLIGHTS]; // Phares avant de la voiture en circulation
uniform Material material;                  // Matériau
uniform vec3 viewPos;                       // Position de la caméra
uniform int alphaTest;                      // 1 = test alpha (arbres/flammes), 0 = opaque
uniform int isEmissive;                     // 1 = objet auto-lumineux (Lune), 0 = eclairage standard

out vec4 frag_color;                        // Couleur de sortie du fragment

// Déclarations de fonctions
vec3 calcDirectionalLightColor(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 calcPointLightColor(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcSpotLightColor(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

//-----------------------------------------------------------------------------------------------
// Entrée principale du shader
//-----------------------------------------------------------------------------------------------
void main()
{ 
	vec4 texColor = texture(material.diffuseMap, TexCoord);
	if (alphaTest == 1 && texColor.a < 0.25)
		discard;

	if (isEmissive == 1) {
		// Lune rayonnante : texture propre et lueur argentee
		frag_color = vec4(texColor.rgb * vec3(1.30, 1.35, 1.45), 1.0);
		return;
	}

	vec3 normal = normalize(Normal); // Normaliser la normale
	vec3 viewDir = normalize(viewPos - FragPos); // Direction de la caméra

    // Ambiante ----------------------------------------------------------------------------------
	vec3 ambient = sunLight.ambient * material.ambient * texColor.rgb; // Calcul de la couleur ambiante
	vec3 outColor = vec3(0.0f); // Couleur de sortie initialisée à zéro

	outColor += calcDirectionalLightColor(sunLight, normal, viewDir); // Ajouter la contribution de la lumière directionnelle

    // Boucle pour ajouter la contribution des lumières ponctuelles
	for(int i = 0; i < MAX_POINT_LIGHTS; i++)
        outColor += calcPointLightColor(pointLights[i], normal, FragPos, viewDir);  

	// Si la lumière spot n'est pas allumée, ne pas ajouter sa couleur diffuse et spéculaire
	if (spotLight.on == 1)
		outColor += calcSpotLightColor(spotLight, normal, FragPos, viewDir);

	// Phares avant de la voiture en circulation (projecteurs froids sur la route)
	for (int i = 0; i < MAX_HEADLIGHTS; i++) {
		if (headlights[i].on == 1)
			outColor += calcSpotLightColor(headlights[i], normal, FragPos, viewDir);
	}

	frag_color = vec4(ambient + outColor, 1.0f); // Couleur finale du fragment
}

//-----------------------------------------------------------------------------------------------
// Calculer l'effet de la lumière directionnelle et retourner la somme des 
// couleurs diffuse et spéculaire résultantes
//-----------------------------------------------------------------------------------------------
vec3 calcDirectionalLightColor(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction); // Normaliser la direction de la lumière (négative car c'est une direction)

	// Diffuse -------------------------------------------------------------------------------------
    float NdotL = max(dot(normal, lightDir), 0.0); // Produit scalaire entre normale et direction de la lumière
    vec3 diffuse = light.diffuse * NdotL * vec3(texture(material.diffuseMap, TexCoord)); // Calcul de la couleur diffuse
    
    // Spéculaire - Blinn-Phong ------------------------------------------------------------------
	vec3 halfDir = normalize(lightDir + viewDir); // Vecteur demi-angle
	float NDotH = max(dot(normal, halfDir), 0.0f); // Produit scalaire entre normale et vecteur demi-angle
	vec3 specular = light.specular * material.specular * pow(NDotH, material.shininess); // Calcul de la couleur spéculaire

	return (diffuse + specular); // Retourner la somme diffuse + spéculaire
}

//-----------------------------------------------------------------------------------------------
// Calculer l'effet de la lumière ponctuelle et retourner la somme des 
// couleurs diffuse et spéculaire résultantes
//-----------------------------------------------------------------------------------------------
vec3 calcPointLightColor(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos); // Direction de la lumière ponctuelle

	// Diffuse -----------------------------------------------------------------------------------
    float NdotL = max(dot(normal, lightDir), 0.0); // Produit scalaire entre normale et direction de la lumière
    vec3 diffuse = light.diffuse * NdotL * vec3(texture(material.diffuseMap, TexCoord)); // Calcul de la couleur diffuse
    
    // Spéculaire - Blinn-Phong ------------------------------------------------------------------
	vec3 halfDir = normalize(lightDir + viewDir); // Vecteur demi-angle
	float NDotH = max(dot(normal, halfDir), 0.0f); // Produit scalaire entre normale et vecteur demi-angle
	vec3 specular = light.specular * material.specular * pow(NDotH, material.shininess); // Calcul de la couleur spéculaire

	// Attenuation en utilisant Kc, Kl, Kq -------------------------------------------------------
	float d = length(light.position - FragPos); // Distance entre la lumière et le fragment
	float attenuation = 1.0f / (light.constant + light.linear * d + light.exponent * (d * d)); // Calcul de l'attenuation

	diffuse *= attenuation;
	specular *= attenuation;

	return (diffuse + specular);
}

//------------------------------------------------------------------------------------------------
// Calculer l'effet de la lumière spot et retourner la somme des 
// couleurs diffuse et spéculaire résultantes
//------------------------------------------------------------------------------------------------
vec3 calcSpotLightColor(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos); // Direction de la lumière spot
	vec3 spotDir  = normalize(light.direction); // Direction du spot

	float cosDir = dot(-lightDir, spotDir); // Angle entre le vecteur de direction de la lumière et le vecteur de direction du spot
	float spotIntensity = smoothstep(light.cosOuterCone, light.cosInnerCone, cosDir); // Intensité du spot

	// Diffuse -----------------------------------------------------------------------------------
    float NdotL = max(dot(normal, lightDir), 0.0); // Produit scalaire entre normale et direction de la lumière
    vec3 diffuse = light.diffuse * NdotL * vec3(texture(material.diffuseMap, TexCoord)); // Calcul de la couleur diffuse
    
    // Spéculaire - Blinn-Phong ------------------------------------------------------------------
	vec3 halfDir = normalize(lightDir + viewDir); // Vecteur demi-angle
	float NDotH = max(dot(normal, halfDir), 0.0f); // Produit scalaire entre normale et vecteur demi-angle
	vec3 specular = light.specular * material.specular * pow(NDotH, material.shininess); // Calcul de la couleur spéculaire

	// Atténuation en utilisant Kc, Kl, Kq -------------------------------------------------------
	float d = length(light.position - FragPos); // Distance entre la lumière et le fragment
	float attenuation = 1.0f / (light.constant + light.linear * d + light.exponent * (d * d)); // Calcul de l'atténuation

	diffuse *= attenuation * spotIntensity; // Appliquer l'atténuation et l'intensité du spot à la couleur diffuse
	specular *= attenuation * spotIntensity; // Appliquer l'atténuation et l'intensité du spot à la couleur spéculaire
	
	return (diffuse + specular); // Retourner la somme diffuse + spéculaire
}

