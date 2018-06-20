#version 430 core

uniform sampler2D textureSampler;
uniform sampler2D normalSampler;
uniform vec3 lightDir;

in vec3 interpNormal;
in vec2 interpTexCoord;

void main()
{
	vec2 modifiedTexCoord = vec2(interpTexCoord.x, 1.0 - interpTexCoord.y);
	vec3 color = texture2D(textureSampler, modifiedTexCoord).rgb;
	// obtain normal from normal map in range [0,1]
    vec3 normal = texture(normalSampler, modifiedTexCoord).rgb;
    // transform normal vector to range [-1,1]
     normal = normalize(normal * interpNormal); 
	float diffuse = max(dot(normal, -lightDir), 0.0);
	gl_FragColor = vec4(color * diffuse, 1.0);
}
