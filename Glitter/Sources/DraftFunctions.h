//
// Created by zhangjie on 2023/7/13.
//

#ifndef GLITTER_DRAFTFUNCTIONS_H
#define GLITTER_DRAFTFUNCTIONS_H
#include <iostream>
#include <glad/glad.h>
#include <filesystem>

// Shaders
const GLchar* vertexShaderSource = "";
const GLchar* fragmentShaderSource = "";
void programShaders() {

    // 创建顶点着色器对象和片段着色器对象
    // Build and compile our shader program
    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // Check for compile time errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // Check for compile time errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // 创建着色器程序对象并链接着色器
    // Link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    // 删除着色器对象，因为它们已经链接到着色器程序中
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void listFilesRecursively(const std::filesystem::path& path)
{
    if (std::filesystem::is_directory(path))
    {
        for (const auto& entry : std::filesystem::directory_iterator(path))
        {
            if (std::filesystem::is_directory(entry))
            {
                listFilesRecursively(entry.path());
            }
            else
            {
                std::cout << entry.path() << std::endl;
            }
        }
    }
    else
    {
        std::cout << path << std::endl;
    }
}
#endif //GLITTER_DRAFTFUNCTIONS_H


/* 反斜杠后面不能有空格 */
#define ASSERT(x) if (!(x)) __builtin_trap()
#define GLCall(x) GLClearError(); \
x; \
ASSERT(GLLogCall(#x, __FILE__, __LINE__))

// 自定义错误回调函数
static void errorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

static void GLClearError() {
    while (glGetError() != GL_NO_ERROR) {
    }
}

static void GLCheckError() {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ")" << std::endl;
    }
}

static bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ")" << function << " " << file << ":" <<
                  line << std::endl;
        return false;
    }
    return true;
}
