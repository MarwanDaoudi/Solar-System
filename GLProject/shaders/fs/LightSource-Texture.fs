#version 330 core
out vec4 FragColor;

in vec2 texCoord;

uniform vec3 objectColor;
uniform sampler2D tex;

void main()
{
    FragColor = vec4(texture(tex, texCoord).rgb * vec3(objectColor), 1.0); // set all 4 vector values to 1.0
}

