#version 330

uniform sampler2D tex;

in vec2 texCoords;

out vec4 fragColor;

#define N 5
#define N2 N*N

int Partition(inout vec4 B[N2], int i0, int i1){
    vec4 aux[N2];
    vec4 pivot = B[(i0 + i1)/2];
    int k = i0;
    for(int i = i0; i <= i1; i++)
        if(B[i].g < pivot.g)
            aux[k++] = B[i];

    int pos = k;
    for(int i = i0; i <= i1; i++)
        if(B[i].g == pivot.g)
            aux[k++] = B[i];

    for(int i = i0; i <= i1; i++)
        if(B[i].g > pivot.g)
            aux[k++] = B[i];

    for(int i = i0; i <= i1; i++)
        B[i] = aux[i];

    return pos;
}

vec4 quickselect(int k, vec4 B[N2]){
    int i0 = 0;
    int i1 = N2-1;
    while(i0 < i1){
        int i = Partition(B, i0, i1);
        if(i == k)
            return B[i];
        if(i < k)
            i0 = i+1;
        else
            i1 = i-1;
    }
    return B[i0];
}

void main(){
	vec4 sampleTex[N*N];
	ivec2 pos = ivec2(gl_FragCoord.xy);
	int k = 0;
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++){
			sampleTex[k] = texelFetch(tex, pos + ivec2(i-N/2, j-N/2), 0);
			k++;
		}
	fragColor = quickselect(N2/2, sampleTex);
}
