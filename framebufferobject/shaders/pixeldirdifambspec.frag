#version 330

uniform sampler2D texUnit;

layout (std140) uniform Material {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
};

in Data {
	vec2 texCoord;
	vec3 normal;
	vec3 eye;
	vec3 lightDir;
} DataIn;

layout (location = 0) out vec4 colorOut;

void main() {

	vec4 spec = vec4(0.0);
	float intSpec;

	vec3 n = normalize(DataIn.normal);
	vec3 ld = normalize(DataIn.lightDir);
	vec3 e = normalize(DataIn.eye);

	float intensity = max(dot(n,ld), 0.0);

	if (intensity > 0.0) {
		vec3 h = normalize(ld + e);	
		float intSpec = max(dot(h,n), 0.0);
		spec = vec4(1.0,1.0,1.0,1.0) * pow(intSpec,100);
	}

	if (texCount == 0)
		colorOut = (vec4(0.2) + intensity * diffuse + spec); 
	else
		colorOut = (vec4(0.2) + intensity * diffuse ) * texture(texUnit, DataIn.texCoord)+ spec;
	//colorOut = vec4(e,1.0);
}