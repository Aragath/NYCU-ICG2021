#version 430 core
//layout(lines_adjacency) in; // for moon fairy
layout(triangles) in; // for eeve
layout(triangle_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
    //the position in model space , "gl_in.gl_Position" is the position passed from vertex shader
    //so it's in screen space beacuse it have alread multipled with M V P matrix.
    vec3 position;
    vec2 uv;
} gs_in[];

uniform float push_out;

out vec4 color;
out vec2 uv;
// the alpha value of texture
out float alpha;

void main()
{
    vec3 a = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 b = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;

    vec3 face_normal = normalize(cross(a, b));

    alpha = 1.0f;
    color = vec4(1.0,1.0,1.0,1.0);
    for(int i = 0 ; i < gl_in.length() ; i++){
        uv = gs_in[i].uv;
        gl_Position = vec4(gl_in[i].gl_Position.xyz + face_normal * push_out, gl_in[i].gl_Position.w);
        //gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}
