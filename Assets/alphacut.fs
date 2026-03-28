#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
uniform sampler2D texture0;
uniform vec4 colDiffuse;

out vec4 finalColor;

void main()
{
    vec4 texColor = texture(texture0, fragTexCoord);

    vec4 color = texColor * fragColor * colDiffuse;

    if (color.a <= 0.01) discard;

    finalColor = color;
}