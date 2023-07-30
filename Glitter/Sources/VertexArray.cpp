//
// Created by Echo Zhangjie on 30/7/23.
//

#include "VertexArray.h"
#include "Renderer.h"
#include "VertexBufferLayout.h"

VertexArray::VertexArray() {
    GLCall(glGenVertexArrays(1, &m_RendererID)); /* 生存顶点数组 */
    GLCall(glBindVertexArray(m_RendererID)); /* 绑定顶点数组 */
}

VertexArray::~VertexArray() {
    GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout) {
    Bind();
    vb.Bind();
    const auto& elements = layout.GetElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++) {
        const auto &element = elements[i];
        GLCall(glEnableVertexAttribArray(i)); /* 激活顶点属性-索引0-位置 */
        GLCall(glVertexAttribPointer(i, element.count, element.type,
                                     element.normalized, layout.GetStride(), (const void*)offset)); /* 设置顶点属性-索引0 */
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }

}

void VertexArray::Bind() const {
    GLCall(glBindVertexArray(m_RendererID));

}
void VertexArray::Unbind() const {
    GLCall(glBindVertexArray(0));
}
