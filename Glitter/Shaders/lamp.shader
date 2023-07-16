#shader vertex
#version 330 core
layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0f);
}

#shader fragment
#version 330 core
out vec4 color;

void main()
{
    color = vec4(1.0f); //设置四维向量的所有元素为 1.0f
}