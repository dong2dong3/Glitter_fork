//
//  Shader.cpp
//  Echo First GLFW APP
//
//  Created by zhangjie on 2023/7/9.
//

#include "Shader.hpp"
#include <stdio.h>
#include <unistd.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath) {
    // 1. 从文件路径中获取顶点/片段着色器
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // 保证ifstream对象可以抛出异常：
    vShaderFile.exceptions(std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::badbit);
    std::cout << "zjzjzj vertexPath directory: " << vertexPath << std::endl <<
    "zjzjzj vertexPath directory: "  << fragmentPath << std::endl;

    try {
      // 打开文件
      vShaderFile.open(vertexPath);
      fShaderFile.open(fragmentPath);
      std::stringstream vShaderStream, fShaderStream;
      // 读取文件的缓冲内容到流中
      vShaderStream << vShaderFile.rdbuf();
      fShaderStream << fShaderFile.rdbuf();
      // 关闭文件
      vShaderFile.close();
      fShaderFile.close();
      // 转换流至GLchar数组
      vertexCode = vShaderStream.str();
      fragmentCode = fShaderStream.str();

    }
    catch(std::ifstream::failure e)
    {
      std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const GLchar* vShaderCode = vertexCode.c_str();
    const GLchar* fShaderCode = fragmentCode.c_str();
    // 2. 编译着色器
    GLuint vertex, fragment;
    GLint success;
    GLchar infoLog[512];

    // 顶点着色器
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // 打印编译错误（如果有的话）
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    };

    // 片段着色器也类似
    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // Print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
//      glGetShaderInfoLog(fragment, 512, NULL, infoLog);
//      std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
      // cherno way
      int length;
      glGetShaderiv(fragment, GL_INFO_LOG_LENGTH, &length);
      char *message = (char *)alloca(length * sizeof(char));
      glGetShaderInfoLog(vertex, length, &length, message);
      std::cout << "Fail to compile" <<  (fragment == GL_VERTEX_SHADER ? "vertex": "fragment") << "shader!" << std::endl;
      std::cout << message << std::endl;
    }

    // 着色器程序
    this->Program = glCreateProgram();
    glAttachShader(this->Program, vertex);
    glAttachShader(this->Program, fragment);
    glLinkProgram(this->Program);
    glValidateProgram(this->Program);
    // 打印连接错误（如果有的话）
    glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // 删除着色器，它们已经链接到我们的程序中了，已经不再需要了
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
  }
  
  void Shader::Use()
  {
      glUseProgram(Program);
  }
  
  void Shader::setFloat(float offset) {
    GLint xOffset = glGetUniformLocation(Program, "xOffset");
    glUseProgram(Program);
    glUniform1f(xOffset, offset);
  }
  
//void Shader::setShow(int show) {
//  GLint sshow = glGetUniformLocation(Program, "show");
//  glUseProgram(Program);
//  glUniform1f(sshow, show);
//}
