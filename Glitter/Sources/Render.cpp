//
// Created by Echo Zhangjie on 29/7/23.
//

#include "Render.h"
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
