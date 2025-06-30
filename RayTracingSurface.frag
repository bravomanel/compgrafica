#version 330
out vec4 FragColor;

uniform mat4 View;

uniform vec3 lightPos;

in vec3 fragPos;

struct Ray{
	vec3 origin;
	vec3 direction;
};

// Dados de uma interseção entre um raio e a superficie
struct Intersection{
	bool  exists;
	vec3  pos;
	vec3  normal;
	float t;
};

Intersection intersection(Ray r);

// Calcula a iluminação em um ponto de interseção
vec4 illumination(vec3 viewDir, Intersection I);

void main(){
	mat4 iView = inverse(View);
	// Lança um raio da posição do observador até o pixel
	Ray r = Ray(vec3(0,0,0), normalize(fragPos));	
	r.origin = vec3(iView*vec4(r.origin, 1));
	r.direction = normalize(vec3(iView*vec4(r.direction, 0)));

	Intersection I = intersection(r);
	if(!I.exists)
		discard;

	// Calcula a cor do pixel
	FragColor = illumination(r.direction, I);
}

#define SURFACE 2

float surf(vec3 p){
	float x = p.x;
	float y = p.y;
	float z = p.z;

#if SURFACE == 1
	// TORUS	
	float R = 1.5;
	float r = 0.4;
	return pow(sqrt(x*x + y*y) - R, 2) + z*z - r*r;
#elif SURFACE == 2
	// TRECO
	float r = (x*x + y*y);
	return 2*y*(y*y - 3*x*x)*(1-z*z) + r*r - (9*z*z - 1)*(1 - z*z);
#elif SURFACE == 3
	// ELIPSOIDE
	float A = 1;
	float B = 3;
	float C = 2;
	return x*x/(A*A) + y*y/(B*B) + z*z/(C*C) - 1;

#else
	float r = 2;
	// SPHERE
	return x*x + y*y + z*z - r*r;
#endif
}

vec3 surf_normal(vec3 p){
	float x = p.x;
	float y = p.y;
	float z = p.z;
	float d = 0.01;
	return vec3(
		(surf(p + vec3(d, 0, 0)) - surf(p)),
		(surf(p + vec3(0, d, 0)) - surf(p)),
		(surf(p + vec3(0, 0, d)) - surf(p))
	);
}

Intersection search_between(float t0, float t1, Ray ray){
	float f0 = surf(ray.origin + t0*ray.direction);
	float f1 = surf(ray.origin + t1*ray.direction);

	for(int i = 0; i < 10; i++){
		float t = (t0+t1)/2;
		float f = surf(ray.origin + t*ray.direction);
		if(sign(f0) == sign(f)){
			f0 = f;
			t0 = t;
		}else{
			f1 = f;
			t1 = t;
		}
	}

	float t = (t0+t1)/2;

	Intersection I;
	I.exists = true;
	I.pos = ray.origin + t*ray.direction;
	I.normal = surf_normal(I.pos);
	I.t = t;
	return I;
}

// Calcula a interseção entre um raio e a superfície
Intersection intersection(Ray ray){
	float dt = 0.01;
	float t0 = 1;
	float f0 = surf(ray.origin);
	for(int i = 1; i < 1000; i++){
		float t1 = i*dt;
		float f1 = surf(ray.origin + t1*ray.direction);
		if(sign(f0) != sign(f1))
			return search_between(t0, t1, ray);

		f0 = f1;
	}

	Intersection I;
	I.exists = false;
	return I;
}

// Calcula iluminação em um ponto de interseção
vec4 illumination(vec3 viewDir, Intersection I){
	vec3 N = normalize(I.normal);
	
    // direção do observador
	vec3 wr = normalize(-viewDir); 

	// Direção da luz
	vec3 wi = normalize(lightPos - I.pos); // luz pontual

	// direção do raio refletido
	vec3 vs = normalize(reflect(-wi, N)); 

	float Ns = 100;

	vec3 ambient = vec3(0.2, 0.2, 0.2);
	vec3 diffuse = max(0, dot(wi, N))*vec3(0.8, 0.8, 0.8);
	vec3 specular = pow(max(0, dot(vs, wr)), Ns)*vec3(1, 1, 1);
	return vec4(ambient + diffuse + specular, 1);
}