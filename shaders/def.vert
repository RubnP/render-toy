#version 450


layout(binding = 0) uniform trans_mat {
  mat4 mod_world;
  mat4 world_cam;
  mat4 proj;
} tr_mat;

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec3 in_color;
layout(location = 2) in vec2 in_tex_coord;


layout(location = 0) out vec3 frag_color;
layout(location = 1) out vec2 frag_tex_coord;

void main() {
    gl_Position = tr_mat.proj * tr_mat.world_cam * tr_mat.mod_world * vec4(in_position, 0.0, 1.0);
    frag_color = in_color;
    frag_tex_coord = in_tex_coord;
}
