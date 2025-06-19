const char *cube_vs = "\
// VERTEX_SHADER\n\
#version 330 core\n\
\n\
layout(location=0) in vec3 pos;\n\
layout(location=1) in vec3 normal;\n\
\n\
uniform mat4 model;\n\
uniform mat4 view;\n\
uniform mat4 projection;\n\
\n\
out vec3 frag_position;\n\
flat out vec3 frag_normal;\n\
\n\
void main(void) {\n\
	frag_position = vec3(model * vec4(pos, 1));\n\
	frag_normal   = inverse(transpose(mat3(model))) * normal;\n\
\n\
    gl_Position = projection * view * model * vec4(pos, 1);\n\
}";

const char *cube_fs = "\n\
// FRAGMENT_SHADER\n\
#version 330 core\n\
\n\
vec3 I    = vec3(1, 1, 1);\n\
vec3 Iamb = vec3(0.1, 0.1, 0.1);\n\
vec3 ka   = vec3(0.1, 0.1, 0.1);\n\
vec3 ks   = vec3(0.5, 0.5, 0.5);\n\
\n\
uniform vec3 kd;\n\
uniform vec3 eye_position;\n\
uniform vec3 light_position;\n\
\n\
in vec3 frag_position;\n\
flat in vec3 frag_normal;\n\
\n\
out vec4 frag_color;\n\
\n\
void main(void) {\n\
    vec3 L = normalize(light_position - frag_position);\n\
    vec3 V = normalize(eye_position - frag_position);\n\
    vec3 H = normalize(L + V);\n\
    vec3 N = normalize(frag_normal);\n\
\n\
    float NdotL = dot(N, L);\n\
    float NdotH = dot(N, H);\n\
\n\
    vec3 amb_color  = Iamb * ka;\n\
    vec3 diff_color = I * kd * max(0, NdotL);\n\
    vec3 spec_color = I * ks * pow(max(0, NdotH), 100);\n\
\n\
    if (kd == vec3(1, 1, 1))\n\
        frag_color = vec4(1, 1, 1, 1);\n\
    else\n\
        frag_color = vec4((amb_color + diff_color + spec_color), 1);\n\
}";
