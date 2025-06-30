#version 330
out vec4 FragColor;

uniform mat4 View;
uniform int ShadowType;

in vec3 fragPos;

struct Ray{
	vec3 origin;
	vec3 direction;
};

struct Sphere{
	float radius;
	vec3  center;
	vec4  color;
	int   type;
};
#define DIFFUSE_TYPE  1
#define SPECULAR_TYPE 2
#define LIGHT_TYPE    3

// Dados de uma interseção entre um raio e uma esfera
struct Intersection{
	bool  exists;
	vec3  pos;
	vec3  normal;
	float t;
	int   sphere_index;
};

// Parâmetros usados na cena
const float bigRadius   = 1e5;
const float smallRadius = 3.0;
const float lightRadius = 2.0;
const vec3  lightPos    = vec3(0, 9, -18);

// Descrição da cena
const Sphere spheres[] = Sphere[](
	Sphere(bigRadius,   vec3(bigRadius+10, 0, 0),  vec4(0, 0, 1, 1),       DIFFUSE_TYPE  ),
	Sphere(bigRadius,   vec3(-bigRadius-10, 0, 0), vec4(1, 0, 0, 1),       SPECULAR_TYPE ),
	Sphere(bigRadius,   vec3(0, bigRadius+10, 0),  vec4(0.5, 0.5, 0.5, 1), DIFFUSE_TYPE  ),
	Sphere(bigRadius,   vec3(0, -bigRadius-10, 0), vec4(0.5, 0.5, 0.5, 1), DIFFUSE_TYPE  ),
	Sphere(bigRadius,   vec3(0, 0, bigRadius+30),  vec4(0.5, 0.5, 0.5, 1), DIFFUSE_TYPE  ),
	Sphere(bigRadius,   vec3(0, 0, -bigRadius-30), vec4(0.5, 0.5, 0.5, 1), DIFFUSE_TYPE  ),
	Sphere(smallRadius, vec3(4, -7, -20),          vec4(1.0, 1.0, 1.0, 1), SPECULAR_TYPE ),
	Sphere(smallRadius, vec3(-3, -7, -15),         vec4(1.0, 1.0, 0.5, 1), DIFFUSE_TYPE  ),
	Sphere(smallRadius, vec3(-4, 3, -22),          vec4(0.8, 1.0, 1.0, 1), DIFFUSE_TYPE  ),
	Sphere(smallRadius, vec3(-4, -7, 2),           vec4(0.8, 1.0, 1.0, 1), DIFFUSE_TYPE  ),
	Sphere(lightRadius, lightPos,                  vec4(1.0, 1.0, 1.0, 1), LIGHT_TYPE    )
);

// Calcula a interseção entre um raio e a cena (exceto a esfera indicada pelo índice except)
Intersection calculateIntersection(Ray r, int except);

// Calcula a iluminação em um ponto de interseção
vec4 illumination(vec3 viewDir, vec3 lightPos, Intersection I);

// Calcula o reflexo da luz em uma superfície espelhada
vec4 mirror(Intersection I, Ray r, vec3 light);

void main(){
	mat4 iView = inverse(View);
	// Lança um raio da posição do observador até o pixel
	Ray r = Ray(vec3(0,0,0), normalize(fragPos));	
	r.origin = vec3(iView*vec4(r.origin, 1));
	r.direction = vec3(iView*vec4(r.direction, 0));

	Intersection I = calculateIntersection(r, -1);
	if(!I.exists)
		discard;

	// Calcula a cor do pixel
	FragColor = illumination(-r.direction, lightPos, I) + mirror(I, r, lightPos);
}

const float EPS = 1e-5;

// Menor solucao positiva da equacao ax^2 + bx + c = 0
// Retorna -1 se nao houver solucao positiva
float smallerT(float a, float b, float c){
	float delta = b*b - 4*a*c;
	if(delta < 0)
		return -1.0;

	float t1 = (-b-sqrt(delta))/(2*a);
	float t2 = (-b+sqrt(delta))/(2*a);

	if(t1 > EPS && t2 > EPS) 
		return min(t1, t2);
	
    if(t1 > EPS)
        return t1;

    if(t2 > EPS)
        return t2;
	
	return -1.0;
}

// Calcula a interseção entre um raio e a esfera com o índice dado
Intersection intersection(Ray ray, int index){
	// Dados da esfera
	Sphere sphere = spheres[index];
	vec3 center   = sphere.center;
	vec3 dif      = ray.origin - center;
	
	// coeficientes da equação de segundo grau
	float a = dot(ray.direction, ray.direction);
	float b = 2*dot(dif, ray.direction);
	float c = dot(dif, dif) - sphere.radius*sphere.radius;
	
	// resolve a equação
	float t = smallerT(a, b, c);

	Intersection I;
	I.exists = t > EPS;

	if(I.exists){
		// se há solução, obtém os dados da interseção
		vec3 P   = ray.origin + t*ray.direction;
		I.pos    = P;
		I.normal = normalize(P - center);
		I.t      = t;
		I.sphere_index = index;
	}
	return I;
}

// Calcula a interseção entre um raio e a cena (exceto a esfera indicada pelo índice except)
Intersection calculateIntersection(Ray r, int except){
	Intersection I;
	I.exists = false;
	for(int i = 0; i < spheres.length(); i++){
		if(i == except)
			continue;

 		Intersection Ii = intersection(r, i);
		if(Ii.exists && (!I.exists || Ii.t < I.t))
			I = Ii;	
	}
	return I;
}

// Calcula sombra dura em um ponto de interseção
float calculate_hard_shadow(vec3 lightPos, Intersection I){
	vec3 lightDir = normalize(lightPos - I.pos);
	
	Ray lightRay = Ray(I.pos, lightDir);
	Intersection Il = calculateIntersection(lightRay, I.sphere_index);

	if(Il.exists && spheres[Il.sphere_index].type != LIGHT_TYPE)
		return 0.0;
	else
		return 1.0;
}

// Monta uma matriz Tangente-Binormal-Normal
mat3 getTBN(vec3 N){
	vec3 T = normalize( cross(N, N.x>0.1? vec3(0, 1, 0): vec3(1, 0, 0)) );
	vec3 B = cross(N, T);
	return mat3(T, B, N);
}

// Calcula um ponto sobre um círculo no plano z=0
// com raio r, e theta = ângulo com o eixo x
vec3 circle_point(float r, float theta){
	return vec3(r*cos(theta), r*sin(theta), 0);
}

// Calcula um valor pseudo-aleatório entre 0 e 1
// com base em um parâmetro x
float rand(float x){
    return fract(sin(x)*43758.5453);
}

// Calcula um valor pseudo-aleatório entre 0 e 1
// com base em um parâmetro 2d co
float rand(vec2 co){
    return fract(sin(dot(co,vec2(12.9898,78.233))) * 43758.5453);
}

// Calcula uma sombra suave em um ponto de interseção
float calculate_shadow(vec3 lightPos, Intersection I){
	// Direção da luz
	vec3 lightDir = normalize(lightPos - I.pos);

	// Monta um referencial local com direção z igual à direção da luz
	mat3 TBN = getTBN(lightDir);

	float sum = 0.0;
	int m = 3;
	int n = 0;
	float dr = 1.0/m;
	// Lança vários raios para a fonte de luz em direções aleatórias 
	for(int j = 0; j < m; j++){
		float ratio = j*dr;
		float radius = ratio*lightRadius;
		int nc = 1 + int(20*ratio);
		n += nc;
		float dt = 2*3.14159/nc;
		for(int i = 0; i < nc; i++){
			float theta = i*dt + (rand(I.pos.xy)-0.5)*dt;
			float rr = radius + (rand(I.pos.zy)-0.5)*dr;
			vec3 dif = TBN*circle_point(rr, theta);

			vec3 lightDir = normalize(lightPos + dif - I.pos);
			Ray lightRay = Ray(I.pos, lightDir);
			Intersection Il = calculateIntersection(lightRay, I.sphere_index);

			if(!Il.exists || spheres[Il.sphere_index].type == LIGHT_TYPE)
				sum += 1.0;
		}
	}

	return sum/n;
}

// Calcula iluminação em um ponto de interseção
vec4 illumination(vec3 viewDir, vec3 lightPos, Intersection I){
	Sphere sphere = spheres[I.sphere_index];

	if(sphere.type == LIGHT_TYPE)
		return spheres[I.sphere_index].color;

	vec3 lightDir = normalize(lightPos - I.pos);
	
	vec3 normal = I.normal;
	vec4 material_color = sphere.color;

	vec4 ambient = 0.2*material_color;
	vec4 diffuse = max(0, dot(lightDir, normal))*material_color;

	float shadow = ShadowType == 0? 1:
	               ShadowType == 1? calculate_hard_shadow(lightPos, I):
				                    calculate_shadow(lightPos, I);

	return ambient + shadow*diffuse;
}

// Calcula o reflexo da luz em uma superfície espelhada
vec4 mirror(Intersection I, Ray r, vec3 light){
	vec4 Color = vec4(0, 0, 0, 0);
	float s = 0.5;
	for(int i = 0; i < 4; i++){
		if(spheres[I.sphere_index].type != SPECULAR_TYPE)
			break;

		r.origin = I.pos;
		r.direction = reflect(r.direction, I.normal);
		I = calculateIntersection(r, I.sphere_index);
		if(I.exists){
			Color += s*illumination(-r.direction, light, I);
		}else
			break;
		s /= 2;
	}
	return Color;
}
