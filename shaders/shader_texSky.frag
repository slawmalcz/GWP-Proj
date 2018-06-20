#version 430 core

uniform sampler2D textureSampler;

in vec3 interpNormal;
in vec2 interpTexCoord;

void main()
{
	vec2 modifiedTexCoord = vec2(interpTexCoord.x, 1.0 - interpTexCoord.y);
	vec3 color = texture2D(textureSampler, modifiedTexCoord).rgb;
	vec3 normal = normalize(interpNormal);
	gl_FragColor = vec4(color, 1.0);
}
