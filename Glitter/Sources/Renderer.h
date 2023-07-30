//
// Created by Echo Zhangjie on 29/7/23.
//

#pragma once
#include "glitter.hpp"

#include "VertexArray.h"
#include "IndexBuffer.h"
#include "ChernoShaderV2.h"

/* 反斜杠后面不能有空格 */
#define ASSERT(x) if (!(x)) __builtin_trap()
#define GLCall(x) GLClearError(); \
x; \
ASSERT(GLLogCall(#x, __FILE__, __LINE__))

// 自定义错误回调函数

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);


class Renderer {
public:
    void Clear() const;
    void Draw(const VertexArray& va, const IndexBuffer& ib, const ChernoShaderV2& shader) const;
};