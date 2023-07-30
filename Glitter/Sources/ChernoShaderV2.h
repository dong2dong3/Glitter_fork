////
//// Created by Echo Zhangjie on 29/7/23.
////

#pragma once
#include <iostream>
#include <unordered_map>
struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

class ChernoShaderV2 {
private:
    std::string m_FilePath;
    unsigned int m_RenderID;
    // caching for uniforms
    std::unordered_map<std::string, unsigned int> m_UniformLocationCache;
public:
    ChernoShaderV2(const std::string& filepath);
    ~ChernoShaderV2();
    void Bind() const;
    void Unbind() const;

    // Set uniforms
    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
private:
    ShaderProgramSource ParseShader(const std::string& filePath);
    unsigned int CompileShader(unsigned int type, const std::string& source);
    unsigned int CreateShader(const std::string &vertexShader, const std::string &fragmentShader);
    unsigned int GetUniformLocaiton(const std::string& name);
};