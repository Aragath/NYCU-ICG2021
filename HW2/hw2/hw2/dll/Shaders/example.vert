#version 430

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texcoord;

//out vec3 color; // to fragment shader
out vec2 out_Texcoord;

uniform mat4 Projection;
uniform mat4 ModelView;

void main() {
  // the final rendered position of vertex
  gl_Position = Projection * ModelView * vec4(position, 1.0);
  out_Texcoord = texcoord;
}
