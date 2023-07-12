// Local Headers
#include "glitter.hpp"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>

#include "consoleColor.hpp"

//
//  main.cpp
//  Echo First GLFW APP
//
//  Created by zhangjie on 2023/7/3.
//

#include <iostream>
#include <cmath>

#include "Shader.hpp"
#include <filesystem>

//#include "SOIL.h"
//#include <GLFW/SOIL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ChernoShader.hpp"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

/* 反斜杠后面不能有空格 */
#define ASSERT(x) if (!(x)) __builtin_trap()
#define GLCall(x) GLClearError(); \
x; \
ASSERT(GLLogCall(#x, __FILE__, __LINE__))

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void loadAndCreateTexture();
void draft_dir();
unsigned int createShaderProgram();
void listFilesRecursively(const std::filesystem::path& path);
//const char* getResourcePathWith(const std::string filename);
const char* getResourcePathWith(const std::string& filename);
const char* concatenatePath(const std::filesystem::path& directoryPath, const std::string& filename);

const char* getImageResourcePathWith(const std::string& filename);

const char* getShadersResourcePathWith(const std::string& filename);
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

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
// Holds uniform value of texture mix
GLfloat mixValue = 0.2f;

// Shaders
const GLchar* vertexShaderSource = "";
const GLchar* fragmentShaderSource = "";

GLfloat vertices[] = {
        // 位置              // 颜色
        0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
        0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
};

GLfloat vertices2[] = {
        0.5f,  0.5f, 0.0f,  // Top Right
        0.5f, -0.5f, 0.0f,  // Bottom Right
        -0.5f, -0.5f, 0.0f,  // Bottom Left
        -0.5f,  0.5f, 0.0f   // Top Left
};
GLuint indices[] = {  // Note that we start from 0!
        0, 1, 3,  // First Triangle
        1, 2, 3   // Second Triangle
};

// The MAIN function, from here we start the application and run the game loop

std::filesystem::path executableParentPath = std::filesystem::current_path();

std::ostream& operator<<(std::ostream& os, const glm::mat4& mat) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            os << mat[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
}

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

int import3dmodel()
{
    // 初始化Assimp Importer
//    Assimp::Importer importer;
//
//    // 加载模型文件
//    const aiScene* scene = importer.ReadFile("path/to/model.obj",
//                                             aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
//
//    // 检查模型是否成功加载
//    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
//    {
//        std::cout << "Failed to load model: " << importer.GetErrorString() << std::endl;
//        return -1;
//    }
//
//    // 遍历场景中的节点
//    aiNode* rootNode = scene->mRootNode;
//    for (unsigned int i = 0; i < rootNode->mNumChildren; ++i)
//    {
//        aiNode* childNode = rootNode->mChildren[i];
//
//        // 处理节点的网格数据
//        if (childNode->mNumMeshes > 0)
//        {
//            unsigned int meshIndex = childNode->mMeshes[0];
//            aiMesh* mesh = scene->mMeshes[meshIndex];
//
//            // 在这里可以访问网格数据，例如顶点坐标、法线、纹理坐标等
//            // ...
//
//            // 输出网格的顶点数量
//            std::cout << "Mesh " << i << " has " << mesh->mNumVertices << " vertices." << std::endl;
//        }
//    }
//
    return 0;
}

int main(int argc, char * argv[]) {

    // glfw初始化
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // 此行用来给Mac OS X系统做兼容
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // 创建窗口,获取窗口上上下文
    GLFWwindow* window = glfwCreateWindow(mWidth, mHeight, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 通过glfw注册按键事件回调
    glfwSetKeyCallback(window, key_callback);

    // Load OpenGL Functions
    gladLoadGL();
//    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));
    std::cout << BLUE << "OpenGL " << glGetString(GL_VERSION) << RESET << std::endl;
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::current_path(currentPath.parent_path());
    std::cout << "Current working directory: " << currentPath << std::endl;

    // 查询GPU最大支持顶点个数
//    GLint nrAttributes;
//    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
//    std::cout << GREEN << "Maximum nr of vertex attributes supported: " << nrAttributes << RESET << std::endl;

    // 获取视口
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // 编译着色器程序
    Shader ourShader(currentPath.parent_path().append("Shaders/echo_shader.vs").c_str(), currentPath.parent_path().append("Shaders/echo_shader.frag").c_str());

    // 顶点输入
    GLfloat vertices[] = {
//     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
            0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
            0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
    };

    GLuint indices[] = { // 注意索引从0开始!
            0, 1, 3, // 第一个三角形
            1, 2, 3  // 第二个三角形
    };


    // 顶点数组对象 Vertex Array Object, VAO
    // 顶点缓冲对象 Vertex Buffer Object，VBO
    // 索引缓冲对象：Element Buffer Object，EBO或Index Buffer Object，IBO
    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // !!! Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);
    // 把顶点数组复制到缓冲中供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 设置顶点position属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // 设置顶点color属性指针
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // 设置顶点TexCoord属性指针
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // 使用线框模式进行渲染
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 读取并创建贴图
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // 之后对GL_TEXTURE_2D的所以操作都作用在texture对象上
    // 设置纹理环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // 设置纹理过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 读取图片并生成纹理映射
//    int width, height;
    unsigned char* image = stbi_load("container.jpg", &width, &height, 0, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);
    glBindTexture(GL_TEXTURE_2D, 0);


    // 渲染
    while (!glfwWindowShouldClose(window))
    {
        // 检查事件
        glfwPollEvents();

        // 渲染指令
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 绑定texture
        glBindTexture(GL_TEXTURE_2D, texture);

        // 绘图
        ourShader.Use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 交换缓冲
        glfwSwapBuffers(window);
    }

    // 释放VAO,VBO
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // 释放GLFW分配的内存
    glfwTerminate();

    return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    // 当用户按下ESC键,我们设置window窗口的WindowShouldClose属性为true
    // 关闭应用程序
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // change value of uniform with arrow keys(set amount of textre mix)
    if(key == GLFW_KEY_UP && action == GLFW_PRESS) {
        mixValue += 0.1f;
        if (mixValue >= 1.0f) mixValue = 1.0f;

    }
    if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
    {
        mixValue -= 0.1f;
        if (mixValue <= 0.0f) mixValue = 0.0f;
    }
}

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

void draft_dir() {
    const char* file1 = getShadersResourcePathWith("textures.vs");
    const char* file2 = getShadersResourcePathWith("textures.frag");
    std::cout <<"\nzjzjzj"<< file1 << "\nzjzjzj\n" << file2 << std::endl;
}

void modifyVAO_VBO_VEO() {
    // 创建VAO和VBO
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    // 绑定VAO
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray(VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    // 绑定VBO并存储顶点数据
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    // 配置顶点属性指针
//  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
//  glEnableVertexAttribArray(0);
//
//  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
//  glEnableVertexAttribArray(1);

    // 位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3* sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    // 解绑VBO和VAO
    // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
    glBindVertexArray(0);
}


void loadAndCreateTexture() {
    // Build and compile our shader program
    Shader ourShader("./echo_shaders/textures.vs", "./echo_shaders/textures.frag");

    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] = {
            // Positions          // Colors           // Texture Coords
            0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   3.0f, 3.0f, // Top Right
            0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   3.0f, 0.0f, // Bottom Right
            -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
            -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 3.0f  // Top Left
    };
    GLuint indices[] = {  // Note that we start from 0!
            0, 1, 3, // First Triangle
            1, 2, 3  // Second Triangle
    };

    GLuint VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // TexCoord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // Unbind VAO


    // Load and create a texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);
    // Load image, create texture and generate mipmaps
    int width, height;
//    unsigned char* image = SOIL_load_image("container.jpg", &width, &height, 0, SOIL_LOAD_RGB);
//    unsigned char* image = stbi_load("container.jpg", &width, &height, 0, 0);


//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
//    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
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

const char* concatenatePath(const std::filesystem::path& directoryPath, const std::string& filename)
{
    std::filesystem::path filePath = directoryPath;
    filePath.append(filename);
    std::string filePathString = filePath.string();
    char* result = new char[filePathString.length() + 1];
    std::strcpy(result, filePathString.c_str());
    return result;
}

const char* getResourcePathWith(const std::string& filename)
{
    std::filesystem::path filePath = std::filesystem::current_path();
    filePath.append("Resources/" + filename);
    std::string filePathString = filePath.string();
    char* result = new char[filePathString.length() + 1];
    std::strcpy(result, filePathString.c_str());
    return result;
}

const char* getImageResourcePathWith(const std::string& filename)
{
    std::filesystem::path filePath = std::filesystem::current_path();
    filePath.append("Resources/" + filename);
    std::string filePathString = filePath.string();
    char* result = new char[filePathString.length() + 1];
    std::strcpy(result, filePathString.c_str());
    return result;
}

const char* getShadersResourcePathWith(const std::string& filename)
{
    std::filesystem::path filePath = std::filesystem::current_path();
    filePath.append("Resources/echo_shaders/" + filename);
    std::string filePathString = filePath.string();
    char* result = new char[filePathString.length() + 1];
    std::strcpy(result, filePathString.c_str());
    return result;
}

unsigned int createShaderProgram() {

    ShaderProgramSource source = ParseShader(getShadersResourcePathWith("basic_cherno.shader"));

//  std::cout << "VERTEX" <<source.VertexSource<< "FRAGMENT" << source.FragmentSource << std::endl;
    unsigned int program = CreateShader(source.VertexSource, source.FragmentSource);
    return program;
}


//int main(int argc, char * argv[]) {
//
//    // Load GLFW and Create a Window
//    glfwInit();
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
//    auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);
//
//    // Check for Valid Context
//    if (mWindow == nullptr) {
//        fprintf(stderr, "Failed to Create OpenGL Context");
//        return EXIT_FAILURE;
//    }
//
//    // Create Context and Load OpenGL Functions
//    glfwMakeContextCurrent(mWindow);
//    gladLoadGL();
//    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));
//
//    // Rendering Loop
//    while (glfwWindowShouldClose(mWindow) == false) {
//        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//            glfwSetWindowShouldClose(mWindow, true);
//
//        // Background Fill Color
//        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        // Flip Buffers and Draw
//        glfwSwapBuffers(mWindow);
//        glfwPollEvents();
//    }   glfwTerminate();
//    return EXIT_SUCCESS;
//}
