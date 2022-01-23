#version 430

in vec2 out_Texcoord;
out vec4 frag_color;

uniform sampler2D ball_texture;

void main() {
    //the color of pixel
    frag_color = texture2D(ball_texture, out_Texcoord);

}
