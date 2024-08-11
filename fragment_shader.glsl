#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2D texture1;

void main() {
    // sample density value from texture
    float density = texture(texture1, TexCoord).r;

    // apply  color transformation (x / (1 + x))
    float transformedDensity = density / (1.0 + density);

    // output
    FragColor = vec4(color, 1.0);
}
