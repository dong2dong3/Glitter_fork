//
// Created by Echo Zhangjie on 29/7/23.
//

#include "Renderer.h"
#include <iostream>

void GLClearError() {
    while (glGetError() != GL_NO_ERROR) {
    }
}

bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ")" << function << " " << file << ":" <<
                  line << std::endl;
        return false;
    }
    return true;
}
void Renderer::Clear() const {
    glClearColor(0.75f, 0.52f, 0.3f, 1.0f);
}

void Renderer::Draw(const VertexArray &va, const IndexBuffer &ib, const ChernoShaderV2 &shader) const {
    shader.Bind();
    va.Bind();
    ib.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
}