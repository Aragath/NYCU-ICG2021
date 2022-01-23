#version 430

uniform sampler2D texture;
uniform vec3 WorldCamPos;
uniform vec3 WorldLightPos;
uniform vec3 Ka;
uniform vec3 Kd;
uniform vec3 Ks;
uniform vec3 La;
uniform vec3 Ld;
uniform vec3 Ls;
uniform int  gloss;
uniform bool EdgeFlag;

in vec2 uv;
in vec3 normal;
in vec3 fragPos;
in vec4 worldPos;

out vec4 color;
void main()
{
	// TODO : Calculate the Blinn-Phong Shading
	// Hint :
	//		  1. See the formula of Phong-Blinn lighting model / reflection model (you can refer to "ICG_4Shading" slides)
	//		  2. For specular , gloss is the shininess coefficient and it's recommemded that 
	//				- using the quarter of gloss if you calculate sepcular in original Phong lighting model.
	//				- using the full of gloss if you calculate sepcular by halfway angle(Blinn-Phong).
	//		  3. The "albedo" variable is the color(texture) of a pixel
	//		  4. using EdgeFlag to determine whether or not to add the edge color to "every pixel"
	//		  5. using the angle between view and pixel to determine the edge intensity (be careful of that the pixel of edge should have higher intensity of edge color)
	//				- you can using the color you like to draw the edge

	vec4 albedo = texture2D(texture, uv);

	// add in phong compnents
	vec4 ambient = vec4(La, 1.0) * vec4(Ka, 1.0) * albedo;

	vec3 L = normalize(WorldLightPos - fragPos);
	vec3 N = normalize(normal);
	float diff = max(dot(L, N), 0.0); // dot always >= 0
	vec4 diffuse = vec4(Ld, 1.0) * vec4(Kd, 1.0) * albedo * diff;

	vec3 V = normalize(WorldCamPos - fragPos);
	vec3 R = reflect(-L, N);
	float spec = pow(max(dot(V, R), 0.0), gloss); // dot always >= 0
	vec4 specular = vec4(Ls, 1.0) * vec4(Ks, 1.0) * albedo * spec;

	float edge_intensity = 0;
	if(EdgeFlag == true) edge_intensity = 1 - max(dot(V, N), 0.0);
	vec4 edge_color = vec4(1, 1, 1, 1)  * pow(edge_intensity, 3);

	// it's the color which only contain texture , you need to change the output to phong shading result  
	//color = albedo;
	color = ambient + diffuse + specular + edge_color;
	//color =  edge_color;

} 