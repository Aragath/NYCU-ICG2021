#version 430
uniform sampler2D texture;
uniform vec3 WorldLightPos;
uniform vec3 WorldCamPos;
uniform vec3 Kd;
uniform bool EdgeFlag;

in vec2 uv;
in vec3 normal;
in vec3 fragPos;
in vec4 worldPos;

out vec4 color;

void main()
{
 // TODO : Calculate the Toon Shading with 5 level of threshold
 // Hint :
 //		  1. Calculate the color inteensity determined by the angles between the Light and normal vectors
 //		  2. The "albedo" variable is the color(texture) of a pixel
 //		  3. Using EdgeFlag to determine whether or not to add the edge color to "every pixel"
 //		  4. Using the angle between view and pixel to determine the edge intensity (be careful of that the pixel of edge should have higher intensity of edge color)
 //				- you can using the color you like to draw the edge

 vec4 albedo = texture2D(texture, uv);

 vec3 L = normalize(WorldLightPos - fragPos);
 vec3 N = normalize(normal);
 float level = max(dot(L, N), 0.0); // dot always >= 0

 float intensity;

 if(level > 0.75) intensity = 0.8;
 else if(level > 0.30) intensity = 0.6;
 else intensity = 0.4;

 vec3 V = normalize(WorldCamPos - fragPos);

 float edge_intensity = 0;
 if(EdgeFlag == true) edge_intensity = 1 - max(dot(V, N), 0.0);
 vec4 edge_color = vec4(1, 1, 1, 1)  * pow(edge_intensity, 3);

 // it's the color which only contain darker texture to show different shader, you need to change the output to toon shading result
 color = vec4(Kd, 1.0) * albedo * intensity + edge_color;

}