const char *text_vs = "\
// VERTEX_SHADER\n\
#version 330 core\n\
\n\
layout (location = 2) in vec4 vertex;\n\
\n\
out vec2 tex_coords;\n\
\n\
uniform mat4 projection;\n\
\n\
void main() {\n\
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n\
    tex_coords = vertex.zw;\n\
}\n";

const char *text_fs = "\
// FRAGMENT_SHADER\n\
#version 330 core\n\
\n\
in  vec2 tex_coords;\n\
out vec4 frag_color;\n\
\n\
uniform sampler2D text;\n\
uniform vec3 text_color;\n\
\n\
void main() {\n\
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, tex_coords).r);\n\
    frag_color = vec4(text_color, 1.0) * sampled;\n\
}\n";
