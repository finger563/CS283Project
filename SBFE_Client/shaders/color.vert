#version 330

layout (std140) uniform Matrices {
	mat4 projViewModelMatrix;
	mat4 viewModelMatrix;
	mat3 normalMatrix;
	mat3 viewMatrix;
};


in vec4 position;
in vec4 texCoord;

out Data {
	vec4 texCoord;
} DataOut;

void main()
{
	DataOut.texCoord = texCoord;
	gl_Position = projViewModelMatrix * position ;
} 
