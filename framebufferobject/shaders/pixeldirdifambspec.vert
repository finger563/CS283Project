#version 330

layout (std140) uniform Matrices {
	mat4 projViewModelMatrix;
	mat4 viewModelMatrix;
	mat3 normalMatrix;
	mat3 viewMatrix;
};


uniform vec3 lightDir;

in vec4 position;
in vec3 normal;
in vec2 texCoord;

out Data {
	vec2 texCoord;
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
} DataOut;

void main () {
	
	DataOut.normal = normalize(normalMatrix * normal);
	DataOut.lightDir = vec3(viewMatrix * lightDir);

	vec3 pos = vec3(viewModelMatrix * position);
	DataOut.eye = -pos;

	DataOut.texCoord = texCoord;

	gl_Position = projViewModelMatrix * position;	
}