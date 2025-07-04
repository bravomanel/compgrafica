#version 430
out vec4 FragColor;

uniform mat4 View;
uniform mat4 Model;

uniform mat4 iView;
uniform mat4 iModel;

uniform vec4 lightPos;

/////////////////////////////////////////////////////

struct Vertex{
	vec3 position;
	vec3 normal;
	vec2 texCoords;
};

layout (binding = 0) buffer TrianglesBuffer{
	Vertex tris[];
};

/////////////////////////////////////////////////////

struct Range{
	uint begin;
	uint end;
	uint mapKd_index;
	int children[8];
};

layout (binding = 1) buffer RangesBuffer{
	Range ranges[];
};

/////////////////////////////////////////////////////
struct BoundingBox{
	vec3 pmin;
	vec3 pmax;
};

layout (binding = 2) buffer BBsBuffer{
	BoundingBox bbs[];
};

uniform int root_index;

/////////////////////////////////////////////////////

uniform sampler2D mapKd[32];

uniform vec3 pmin;
uniform vec3 pmax;

in vec3 fragPos;

struct Ray{
	vec3 origin;
	vec3 direction;
};

// Dados de uma interseção entre um raio e a superficie
struct Intersection{
	bool  exists;
	Vertex vertex;
	uint  mapKd_index;
	float t;
};

Intersection intersection(Ray r);

// Calcula a iluminação em um ponto de interseção
vec4 illumination(vec3 viewDir, Vertex v, vec3 light, uint mat_index);

void main(){
	mat4 M = iModel*iView;
	// Lança um raio da posição do observador até o pixel
	Ray r = Ray(vec3(0,0,0), normalize(fragPos));
	r.origin = vec3(M*vec4(r.origin, 1));
	r.direction = normalize(vec3(M*vec4(r.direction, 0)));

	Intersection I = intersection(r);
	if(!I.exists)
		discard;

	// Calcula a cor do pixel
	vec3 light = vec3(iModel*lightPos);
	FragColor = illumination(r.direction, I.vertex, light, I.mapKd_index);
}

bool intersect(Ray ray, vec3 P[3], out float t, out float u, out float v){ 
    // Algoritmo de Moller-Trumbore
    // Resolver o sistema: [E1 E2 -D][u v t]^T = C0
    // onde E1 = P1 - P0
    //      E2 = P2 - P0
    //      C0 =  C - P0

    vec3 dir = ray.direction;
    vec3 orig = ray.origin;

    vec3 E1 = P[1] - P[0]; 
    vec3 E2 = P[2] - P[0]; 
    vec3 DxE2 = cross(dir, E2); 
    float det = dot(E1, DxE2); 

    // ray and triangle are parallel if det is close to 0
    if (abs(det) < 1e-15) 
        return false; 

    float invDet = 1/det; 
    vec3 C0 = orig - P[0]; 

    // u =  det(C0, E2, -D) / det(E1, E2, -D) 
    //   = (C0 . (D x E2) ) / (E1 . (D x E2) )
    u = dot(C0, DxE2)*invDet; 
    if (u < 0 || u > 1) 
        return false; 

    vec3 C0xE1 = cross(C0, E1); 

    // v =  det(E1, C0, -D) / det(E1, E2, -D) 
    //   = (D . (C0 x E1) ) / (E1 . (D x E2) )
    v = dot(dir, C0xE1)*invDet; 
    if (v < 0 || u + v > 1) 
        return false; 

    // t =  det(E1, E2, C0) / det(E1, E2, -D) 
    //   = (E2 . (C0xE1) ) / (E1 . (D x E2) )
    t = dot(E2, C0xE1)*invDet; 

    return t > 1e-3; 
} 

bool intersect(Ray ray, BoundingBox BB){
	float tmin = 0;
    float tmax = 1.0f/0.0f;
    for(int i = 0; i < 3; i++){
        float dmin = BB.pmin[i] - ray.origin[i];
        float dmax = BB.pmax[i] - ray.origin[i];
        float d = ray.direction[i];
        if(d == 0){
    	    if(dmin > 0 || dmax < 0)
                return false;
        }else{
            float a = dmin/d;
            float b = dmax/d;
            if(b < a){
				float c = a;
				a = b;
				b = c;
			}
            if(a > tmin)
                tmin = a;
            if(b < tmax)
                tmax = b;
         }
    }
    return tmin <= tmax;
}

#define STACK_MAX_SIZE 50

// Calcula a interseção entre um raio e a superfície
Intersection intersection(Ray ray){	
	Intersection I;
	I.exists = false;

	int stack[STACK_MAX_SIZE];
    int stack_size = 0;
    stack[stack_size++] = root_index;

    while(stack_size > 0 && stack_size <= STACK_MAX_SIZE){
        int top = stack[--stack_size];
        if(!intersect(ray, bbs[top]))
			continue;

		//I.exists = true;
		//return I;

        Range R = ranges[top];
		if(R.children[0] == -1){
			for(uint i = R.begin; i != R.end; i += 3){
				Vertex Tri[3] = {tris[i], tris[i+1], tris[i+2]};
				vec3 T[3] = {Tri[0].position, Tri[1].position, Tri[2].position};
				float t, u, v;
				if(intersect(ray, T, t, u, v)){
					if(!I.exists || t < I.t){
						float w = 1 - u - v;
						vec3 N[3] = { Tri[0].normal,    Tri[1].normal,    Tri[2].normal    };
						vec2 C[3] = { Tri[0].texCoords, Tri[1].texCoords, Tri[2].texCoords };

						I.exists           = true;
						I.vertex.position  = w*T[0] + u*T[1] + v*T[2];
						I.vertex.normal    = w*N[0] + u*N[1] + v*N[2];
						I.vertex.texCoords = w*C[0] + u*C[1] + v*C[2];
						I.mapKd_index      = R.mapKd_index;
						I.t                = t;
					}
				}
			}
		}

		for(int k = 0; k < 8 && R.children[k] != -1; k++)
			stack[stack_size++] = R.children[k];
    }

	return I;
}

// Calcula iluminação em um ponto de interseção
vec4 illumination(vec3 viewDir, Vertex v, vec3 light, uint mat_index){
	vec3 N = normalize(v.normal);
	
    // direção do observador
	vec3 wr = normalize(-viewDir); 

	// Direção da luz
	vec3 wi = normalize(light - v.position); // luz pontual

	// direção do raio refletido
	vec3 vs = normalize(reflect(-wi, N)); 

	float Ns = 100;

	vec3 Kd = texture(mapKd[mat_index], v.texCoords).rgb;

	vec3 ambient  = 0.2*Kd;
	vec3 diffuse  = max(0, dot(wi, N))*Kd;
	vec3 specular = pow(max(0, dot(vs, wr)), Ns)*vec3(1, 1, 1);
	return vec4(ambient + diffuse + specular, 1);
}