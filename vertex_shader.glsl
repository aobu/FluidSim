#version 330 core

// input
layout (location = 0) in vec2 aPos;      // pos
layout (location = 1) in vec2 aTexCoord; // tex

// output
out vec2 TexCoord;

void main() {
    // pos
    gl_Position = vec4(aPos, 0.0, 1.0);

    // pass to frag
    TexCoord = aTexCoord;
}
