#version 460 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix;

void main() {
    FragPos = vec3(model * vec4(position, 1.0));
    Normal = normalMatrix * aNormal;
    gl_Position = projection * view * model * vec4(position, 1.0);
}