#version 410 core
uniform vec4 ourColour;
out vec4 Colour;
void main() {
    Colour = ourColour;
}