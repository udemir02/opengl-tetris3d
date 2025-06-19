// VERTEX_SHADER
#version 330 core

layout(location=0) in vec3 pos;
layout(location=1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 frag_position;
flat out vec3 frag_normal;

void main(void) {
	frag_position = vec3(model * vec4(pos, 1));
	frag_normal   = inverse(transpose(mat3(model))) * normal;

    gl_Position = projection * view * model * vec4(pos, 1);
}

// FRAGMENT_SHADER
#version 330 core

vec3 I    = vec3(1, 1, 1);
vec3 Iamb = vec3(0.1, 0.1, 0.1);
vec3 ka   = vec3(0.1, 0.1, 0.1);
vec3 ks   = vec3(0.5, 0.5, 0.5);

uniform vec3 kd;
uniform vec3 eye_position;
uniform vec3 light_position;

in vec3 frag_position;
flat in vec3 frag_normal;

out vec4 frag_color;

void main(void) {
    vec3 L = normalize(light_position - frag_position);
    vec3 V = normalize(eye_position - frag_position);
    vec3 H = normalize(L + V);
    vec3 N = normalize(frag_normal);

    float NdotL = dot(N, L);
    float NdotH = dot(N, H);

    vec3 amb_color  = Iamb * ka;
    vec3 diff_color = I * kd * max(0, NdotL);
    vec3 spec_color = I * ks * pow(max(0, NdotH), 100);

    if (kd == vec3(1, 1, 1))
        frag_color = vec4(1, 1, 1, 1);
    else
        frag_color = vec4((amb_color + diff_color + spec_color), 1);

}
