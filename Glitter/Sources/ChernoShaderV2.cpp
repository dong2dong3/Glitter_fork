//
// Created by Echo Zhangjie on 29/7/23.
//

#include "ChernoShaderV2.h"
#include "Renderer.h"
#include "glitter.hpp"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <sstream>

ChernoShaderV2::ChernoShaderV2(const std::string& filepath): m_FilePath(filepath), m_RenderID(0) {

    ShaderProgramSource source = ParseShader(filepath);
    m_RenderID = CreateShader(source.VertexSource, source.FragmentSource);
}
ChernoShaderV2::~ChernoShaderV2() {
    GLCall(glDeleteProgram(m_RenderID));
}

void ChernoShaderV2::Bind() const {
    GLCall(glUseProgram(m_RenderID));
}

void ChernoShaderV2::Unbind() const {
    GLCall(glUseProgram(0));
}

ShaderProgramSource ChernoShaderV2::ParseShader(const std::string& filePath)
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

unsigned int ChernoShaderV2::CreateShader(const std::string &vertexShader, const std::string &fragmentShader) {
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

unsigned int ChernoShaderV2::CompileShader(unsigned int type, const std::string& source)
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
void ChernoShaderV2::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
    GLCall(glUniform4f(GetUniformLocaiton(name), v0, 0.3f, 0.8f, 1.0f));
}

unsigned int ChernoShaderV2::GetUniformLocaiton(const std::string& name) {
    if(m_UniformLocationCache.find(name) != m_UniformLocationCache.end()) {
        std::cout << "zjzjzj--use cache" << std::endl;
        return m_UniformLocationCache[name];
    }
    GLCall(unsigned int location = glGetUniformLocation(m_RenderID, name.c_str())); /* 获取指定名称统一变量的位置 */
    if (location == -1) {
        std::cout << "Warning: uniform" << name << "doens't exist!" << std::endl;
    }
    std::cout << "zjzjzj--find cache" << std::endl;
    m_UniformLocationCache[name] = location;
    return location;
}