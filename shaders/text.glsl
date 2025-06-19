// VERTEX_SHADER
#version 330 core

layout (location = 2) in vec4 vertex;

out vec2 tex_coords;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    tex_coords = vertex.zw;
}

// FRAGMENT_SHADER
#version 330 core

in  vec2 tex_coords;
out vec4 frag_color;

uniform sampler2D text;
uniform vec3 text_color;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, tex_coords).r);
    frag_color = vec4(text_color, 1.0) * sampled;
}
