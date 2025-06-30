#ifndef PHONG_H
#define PHONG_H

#include "vec.h"

struct Light{
    vec4 position;
	vec3 Ia = vec3{0.2, 0.2, 0.2};
	vec3 Id = vec3{1.0, 1.0, 1.0};
	vec3 Is = vec3{1.0, 1.0, 1.0};
};

template<class Material>
RGB illumination(vec3 position, vec3 normal, Light light, Material material){
	vec3 N = normalize(normal);
	
    // direção do observador
	vec3 wr = normalize(-position); 
	
	// Posição da luz no referencial do observador
	vec4 lightPos = light.position;

	// Direção da luz
	vec3 wi = (lightPos[3] == 0)?
		normalize(toVec3(lightPos)): // luz direcional
		normalize(toVec3(lightPos) - position); // luz pontual

	// direção do raio refletido
	vec3 vs = normalize(reflect(-wi, N)); 

	vec3 ambient = material.Ka*light.Ia;
	vec3 diffuse = std::max(0.0f, dot(wi, N))*material.Kd*light.Id;
	vec3 specular = pow(std::max(0.0f, dot(vs, wr)), material.Ns)*material.Ks*light.Is;
    
    return toColor(ambient + diffuse + specular);
}


#endif