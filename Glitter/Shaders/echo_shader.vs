#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

//out vec3 ourColor;
uniform float xOffset;

out vec3 ourPosition;

void main()
{
//  gl_Position = vec4(position, 1.0f);
  gl_Position = vec4(position.x + xOffset, -position.y, position.z, 1.0); // just add a - to the y position
  
//  ourColor = color;
  ourPosition = position;

}
