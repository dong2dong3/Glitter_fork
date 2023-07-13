//
//  ChernoShader.hpp
//  EchoOpenGL
//
//  Created by zhangjie on 2023/7/10.
//

#ifndef ChernoShader_hpp
#define ChernoShader_hpp

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <GL/glew.h>

struct ShaderProgramSource {
  std::string VertexSource;
  std::string FragmentSource;
};
static ShaderProgramSource ParseShader(const std::string& filePath)
{
  std::ifstream stream(filePath); /* 这里没判断文件是否能正常打开 is_open */
  enum class ShaderType {
    NONE = -1, VERTEX = 0, FRAGMENT = 1
  };
  std::string line;
  std::stringstream ss[2];
  ShaderType type = ShaderType::NONE;
  while (getline(stream, line)) {
    if (line.find("#shader") != std::string::npos) { /* 找到#shader标记 */
      if (line.find("vertex") != std::string::npos) { /* 顶点着色器标记 */
        type = ShaderType::VERTEX;
      } else if (line.find("fragment") != std::string::npos) { /* 片段着色器标记 */
        type = ShaderType::FRAGMENT;
      }
    } else {
      ss[(int)type] << line << '\n';
    }
  }
  return { ss[0].str(),ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
  unsigned int id = glCreateShader(type); /* 创建对应类型的着色器 */
  const char* src = source.c_str();
  glShaderSource(id, 1, &src, nullptr); /* 设置着色器源码 */
  glCompileShader(id); /* 编译着色器 */
  /* 编译错误处理 */
  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result); // 获取当前着色器编译状态
  if (result == GL_FALSE) {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length); // 获取日志长度
    char* msg = (char*)alloca(length * sizeof(char)); /* Cherno这里采用的alloca, 根据IDE提示, 我这里改成了_malloca函数 */
    glGetShaderInfoLog(id, length, &length, msg); // 获取日志信息
    std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex":"fragment") << " shader!" << std::endl;
    std::cout << msg << std::endl;
    glDeleteShader(id); // 删除着色器
    return 0;
  }
  return id;
}

static unsigned int CreateShader(const std::string &vertexShader, const std::string &fragmentShader) {
  unsigned int program = glCreateProgram(); /* 创建程序 */
  unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
  
  /* 将着色器附加到程序上 */
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program); /* 链接程序 */
  glValidateProgram(program); /* 验证 */
  
  /* 删除着色器 */
  glDeleteShader(vs);
  glDeleteShader(fs);
  
  return program;
}

#endif /* ChernoShader_hpp */
