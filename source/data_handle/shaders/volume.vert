#version 330 core
layout(location = 0) in vec3 aPos;      // 3D position
layout(location = 1) in vec3 aTexCoord; // 3D tex coords

out vec3 TexCoord;

uniform mat4 model_view;
uniform mat4 projection;

void main() {
    gl_Position = projection * model_view * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}