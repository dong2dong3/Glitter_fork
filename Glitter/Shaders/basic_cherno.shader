#shader vertex
#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

out vec3 ourColor;
out vec2 TexCoord;
//uniform mat4 transform;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
  //    gl_Position = vec4(position, 1.0f);
//  gl_Position = transform * vec4(position, 1.0f);
  gl_Position = projection * view * model * vec4(position, 1.0f);
  
  ourColor = color;
  //    TexCoord = texCoord;
  // We swap the y-axis by substracing our coordinates from 1. This is done because most images have the top y-axis inversed with OpenGL's top y-axis.
  TexCoord = vec2(1.0 - texCoord.x, 1.0 - texCoord.y);
}

#shader fragment
#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

//uniform sampler2D ourTexture;
// Texture samplers
uniform sampler2D ourTexture1;
uniform sampler2D ourTexture2;

//uniform int show;
uniform float mixValue;

void main()
{
//    color = texture(ourTexture, TexCoord);
  //我们还可以把得到的纹理颜色与顶点颜色混合，来获得更有趣的效果。我们只需把纹理颜色与顶点颜色在片段着色器中相乘来混合二者的颜色：
//  color = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0f);
  // Linearly interpolate between both textures (second texture is only slightly combined)
//  color = mix(texture(ourTexture1, TexCoord), texture(ourTexture2, TexCoord), 0.2);
  color = mix(texture(ourTexture1, TexCoord) , texture(ourTexture2, TexCoord), mixValue);
}
