////
//// Created by Echo Zhangjie on 29/7/23.
////
//
//#pragma once
//#include <iostream>
//class ChernoShaderV2 {
//private:
//    std::string m_FilePath;
//    unsigned int m_RenderID;
//    // caching for uniforms
//public:
//    ChernoShaderV2(const std::string& filepath);
//    ~ChernoShaderV2();
//    void Bind() const;
//    void Unbind() const;
//
//    // Set uniforms
//    void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
//private:
//    unsigned int GetUniformLocaiton(const std::string& name);
//    bool CompileShader();
//};