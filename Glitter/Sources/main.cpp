// Local Headers
#include "glitter.hpp"
#include "Camera.h"

// System Headers
#include <GLFW/glfw3.h>
#include "consoleColor.hpp"
#include <iostream>
#include <cmath>
#include <filesystem>


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ChernoShader.hpp"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "DraftFunctions.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void do_movement();
void loadAndCreateTexture();
void draft_dir();
unsigned int createShaderProgram(const std::string& filePath);
void listFilesRecursively(const std::filesystem::path& path);
//const char* getResourcePathWith(const std::string filename);
const char* getResourcePathWith(const std::string& filename);
const char* concatenatePath(const std::filesystem::path& directoryPath, const std::string& filename);
const char* getImageResourcePathWith(const std::string& filename);

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 600;
// Holds uniform value of texture mix
GLfloat mixValue = 0.2f;


// The MAIN function, from here we start the application and run the game loop
std::ostream& operator<<(std::ostream& os, const glm::mat4& mat) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            os << mat[i][j] << " ";
        }
        os << std::endl;
    }
    return os;
}


std::filesystem::path executableParentPath = std::filesystem::current_path();
const char* concatenatePath(const std::filesystem::path& directoryPath, const std::string& filename)
{
    std::filesystem::path filePath = directoryPath;
    filePath.append(filename);
    std::string filePathString = filePath.string();
    char* result = new char[filePathString.length() + 1];
    std::strcpy(result, filePathString.c_str());
    return result;
}

// Camera起始位置
GLfloat deltaTime = 0.0f;   // 当前帧遇上一帧的时间差
GLfloat lastFrame = 0.0f;   // 上一帧的时间

// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
// Light attributes
glm::vec3 lightPos(1.2f, 1.0f, 1.2f);
//鼠标起始位置
bool firstMouse = true;
GLfloat lastX = 400, lastY = 300;
// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat yaw = -0.0f, pitch = 0.0f;
//鼠标滚轮
GLfloat fov =  45.0f;

int main(int argc, char * argv[]) {
    // glfw初始化
    glfwInit();
    // 设置错误回调函数
    glfwSetErrorCallback(errorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // 正向兼容模式
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
    // 通过glfw注册鼠标事件回调
    glfwSetCursorPosCallback(window, mouse_callback);
    // 通过glfw注册鼠标滚轮事件回调
    glfwSetScrollCallback(window, scroll_callback);

    // Load OpenGL Functions
    gladLoadGL();
    std::cout << BLUE << "OpenGL " << glGetString(GL_VERSION) << RESET << std::endl;
    std::cout << "Current working directory: " << executableParentPath << std::endl;

    // 查询GPU最大支持顶点个数 opengl version
    GLint nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << GREEN << "Maximum nr of vertex attributes supported: " << nrAttributes << RESET << std::endl;
    std::cout << "GL_VERSION = " << glGetString(GL_VERSION) << std::endl;

    // Define the viewport dimensions
    glViewport(0, 0, HEIGHT, WIDTH);
    /**
     在深度测试过程中，当一个像素要被绘制时，OpenGL会将该像素的深度值与深度缓冲区中对应位置的深度值进行比较。
     如果该像素的深度值比深度缓冲区中的值更接近观察者，那么它将被绘制；否则，它将被认为是被覆盖的像素而被丢弃。
     */
    // Setup OpenGL options
    glEnable(GL_DEPTH_TEST);

    // Set up vertex data (and buffer(s)) and attribute pointers
    GLfloat vertices[] =
    {
        // Positions          // Normals           // Texture Coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // Positions of the point lights
    glm::vec3 pointLightPositions[] = {
            glm::vec3( 0.7f,  0.2f,  2.0f),
            glm::vec3( 2.3f, -3.3f, -4.0f),
            glm::vec3(-4.0f,  2.0f, -12.0f),
            glm::vec3( 0.0f,  0.0f, -3.0f)
    };
    // == ==============================================================================================
    //       DESERT
    // == ==============================================================================================
//    glm::vec3 pointLightColors[] = {
//            glm::vec3(1.0f, 0.6f, 0.0f),
//            glm::vec3(1.0f, 0.0f, 0.0f),
//            glm::vec3(1.0f, 1.0, 0.0),
//            glm::vec3(0.2f, 0.2f, 1.0f)
//    };
    // == ==============================================================================================
    //       FACTORY
    // == ==============================================================================================
//    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//    glm::vec3 pointLightColors[] = {
//            glm::vec3(0.2f, 0.2f, 0.6f),
//            glm::vec3(0.3f, 0.3f, 0.7f),
//            glm::vec3(0.0f, 0.0f, 0.3f),
//            glm::vec3(0.4f, 0.4f, 0.4f)
//    };
    // == ==============================================================================================
    //       HORROR
    // == ==============================================================================================
//    glm::vec3 pointLightColors[] = {
//            glm::vec3(0.1f, 0.1f, 0.1f),
//            glm::vec3(0.1f, 0.1f, 0.1f),
//            glm::vec3(0.1f, 0.1f, 0.1f),
//            glm::vec3(0.3f, 0.1f, 0.1f)
//    };
    // == ==============================================================================================
    //       BIOCHEMICAL LAB
    // == ==============================================================================================
//    glm::vec3 pointLightColors[] = {
//            glm::vec3(0.4f, 0.7f, 0.1f),
//            glm::vec3(0.4f, 0.7f, 0.1f),
//            glm::vec3(0.4f, 0.7f, 0.1f),
//            glm::vec3(0.4f, 0.7f, 0.1f)
//    };

    // == ==============================================================================================
    //       DESERT
    // == ==============================================================================================
    glm::vec3 pointLightColors[] = {
            glm::vec3(1.0f, 0.6f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 1.0, 0.0),
            glm::vec3(0.2f, 0.2f, 1.0f)
    };

    // 获取视口
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    // First, set the container's VAO (and VBO)
    GLuint VBO, containerVAO;
    glGenVertexArrays(1, &containerVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(containerVAO);
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);

    // Then, we set the light's VAO (VBO stays the same. After all, the vertices are the same for the light object (also a 3D cube))
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    // We only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it; the VBO's data already contains all we need.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Set the vertex attributes (only position data for the lamp))
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);

    // Load textures
    GLuint diffuseMap, specularMap, emissionMap;
    glGenTextures(1, &diffuseMap);
    glGenTextures(1, &specularMap);
    glGenTextures(1, &emissionMap);
    int iwidth, iheight;
    unsigned char* image;
    // Diffuse map
    image = stbi_load(concatenatePath(executableParentPath, "Resources/container2.jpg"), &iwidth, &iheight, 0, 0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    // Specular map
    image = stbi_load(concatenatePath(executableParentPath, "Resources/container2_specular.jpg"), &iwidth, &iheight, 0, 0);
    glBindTexture(GL_TEXTURE_2D, specularMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Build and compile our shader program
    unsigned int lightingProgram = createShaderProgram(concatenatePath(executableParentPath, "Shaders/lighting.shader"));
    unsigned int lampProgram = createShaderProgram(concatenatePath(executableParentPath, "Shaders/lamp.shader"));
    // Set texture units
    glUseProgram(lightingProgram);
    glUniform1i(glGetUniformLocation(lightingProgram, "material.diffuse"),  0);
    glUniform1i(glGetUniformLocation(lightingProgram, "material.specular"), 1);
    // 渲染
    while (!glfwWindowShouldClose(window))
    {

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        do_movement();

        // Render
        // Clear the colorbuffer
        // glClearColor(0.1f, 0.1f, 0.1f, 1.0f)
        // HORROR
        //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        // BIOCHEMICAL LAB
        // glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
        //  DESERT
        glClearColor(0.75f, 0.52f, 0.3f, 1.0f);


        // glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glUseProgram(lightingProgram);
        GLint viewPosLoc  = glGetUniformLocation(lightingProgram, "viewPos");
        glUniform3f(viewPosLoc,  camera.Position.x, camera.Position.y, camera.Position.z);
        // Set material properties
        glUniform1f(glGetUniformLocation(lightingProgram, "material.shininess"), 32.0f);
//
//        GLint lightPosLoc = glGetUniformLocation(lightingProgram, "light.position");
//        GLint lightSpotdirLoc    = glGetUniformLocation(lightingProgram, "light.direction");
//        GLint lightSpotCutOffLoc = glGetUniformLocation(lightingProgram, "light.cutOff");
//        GLint lightSpotOuterCutOffLoc = glGetUniformLocation(lightingProgram, "light.outerCutOff");
//
//        glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
//        glUniform3f(lightSpotdirLoc,    camera.Front.x, camera.Front.y, camera.Front.z);
//        glUniform1f(lightSpotCutOffLoc, glm::cos(glm::radians(12.5f)));
//        glUniform1f(lightSpotOuterCutOffLoc, glm::cos(glm::radians(17.5f)));
//
//        // Set lights properties
//        glUniform3f(glGetUniformLocation(lightingProgram, "light.ambient"),   0.1f, 0.1f, 0.1f);
//        // We set the diffuse intensity a bit higher; note that the right lighting conditions differ with each lighting method and environment.
//        // Each environment and lighting type requires some tweaking of these variables to get the best out of your environment.
//        glUniform3f(glGetUniformLocation(lightingProgram, "light.diffuse"),   0.8f, 0.8f, 0.8f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "light.specular"),  1.0f, 1.0f, 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "light.constant"),  1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "light.linear"),    0.09);
//        glUniform1f(glGetUniformLocation(lightingProgram, "light.quadratic"), 0.032);


        // == ==========================
        // Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
        // the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
        // by defining light types as classes and set their values in there, or by using a more efficient uniform approach
        // by using 'Uniform buffer objects', but that is something we discuss in the 'Advanced GLSL' tutorial.
        // == ==========================
//        // Directional light
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.ambient"), 0.05f, 0.05f, 0.05f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.diffuse"), 0.4f, 0.4f, 0.4f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
//
//        // Point light 1
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].ambient"), 0.05f, 0.05f, 0.05f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].diffuse"), 0.8f, 0.8f, 0.8f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].specular"), 1.0f, 1.0f, 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[0].constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[0].linear"), 0.09);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[0].quadratic"), 0.032);
//        // Point light 2
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].ambient"), 0.05f, 0.05f, 0.05f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].diffuse"), 0.8f, 0.8f, 0.8f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].specular"), 1.0f, 1.0f, 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[1].constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[1].linear"), 0.09);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[1].quadratic"), 0.032);
//        // Point light 3
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].ambient"), 0.05f, 0.05f, 0.05f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].diffuse"), 0.8f, 0.8f, 0.8f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].specular"), 1.0f, 1.0f, 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[2].constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[2].linear"), 0.09);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[2].quadratic"), 0.032);
//        // Point light 4
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].ambient"), 0.05f, 0.05f, 0.05f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].diffuse"), 0.8f, 0.8f, 0.8f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].specular"), 1.0f, 1.0f, 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[3].constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[3].linear"), 0.09);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[3].quadratic"), 0.032);
//
//        // SpotLight
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.diffuse"), 0.8f, 0.8f, 0.0f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.specular"), 0.8f, 0.8f, 0.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.linear"), 0.09);
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.quadratic"), 0.032);
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.outerCutOff"), glm::cos(glm::radians(13.0f)));

// Directional light
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.ambient"), 0.3f, 0.24f, 0.14f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.diffuse"), 0.7f, 0.42f, 0.26f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.ambient"), 0.05f, 0.05f, 0.1f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.diffuse"), 0.2f, 0.2f, 0.7);
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.specular"), 0.7f, 0.7f, 0.7f);
//// Point light 1
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].ambient"), pointLightColors[0].x * 0.1,  pointLightColors[0].y * 0.1,  pointLightColors[0].z * 0.1);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].diffuse"), pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].specular"), pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[0].constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[0].linear"), 0.09);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[0].quadratic"), 0.032);
//// Point light 2
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].ambient"), pointLightColors[1].x * 0.1,  pointLightColors[1].y * 0.1,  pointLightColors[1].z * 0.1);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].diffuse"), pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].specular"), pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[1].constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[1].linear"), 0.09);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[1].quadratic"), 0.032);
//// Point light 3
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].ambient"), pointLightColors[2].x * 0.1,  pointLightColors[2].y * 0.1,  pointLightColors[2].z * 0.1);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].diffuse"), pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].specular") ,pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[2].constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[2].linear"), 0.09);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[2].quadratic"), 0.032);
//// Point light 4
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].ambient"), pointLightColors[3].x * 0.1,  pointLightColors[3].y * 0.1,  pointLightColors[3].z * 0.1);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].diffuse"), pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].specular"), pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[3].constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[3].linear"), 0.09);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[3].quadratic"), 0.032);
//// SpotLight
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.diffuse"), 0.8f, 0.8f, 0.0f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.specular"), 0.8f, 0.8f, 0.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.linear"), 0.09);
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.quadratic"), 0.032);
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.outerCutOff"), glm::cos(glm::radians(17.5f)));

// HORROR
//// Directional light
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.ambient"), 0.0f, 0.0f, 0.0f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.diffuse"), 0.05f, 0.05f, 0.05);
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.specular"), 0.2f, 0.2f, 0.2f);
//// Point light 1
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].ambient"), pointLightColors[0].x * 0.1,  pointLightColors[0].y * 0.1,  pointLightColors[0].z * 0.1);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].diffuse"), pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].specular"), pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[0].constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[0].linear"), 0.14);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[0].quadratic"), 0.07);
//// Point light 2
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].ambient"), pointLightColors[1].x * 0.1,  pointLightColors[1].y * 0.1,  pointLightColors[1].z * 0.1);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].diffuse"), pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].specular"), pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[1].constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[1].linear"), 0.14);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[1].quadratic"), 0.07);
//// Point light 3
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].ambient"), pointLightColors[2].x * 0.1,  pointLightColors[2].y * 0.1,  pointLightColors[2].z * 0.1);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].diffuse"), pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].specular") ,pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[2].constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[2].linear"), 0.22);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[2].quadratic"), 0.20);
//// Point light 4
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].ambient"), pointLightColors[3].x * 0.1,  pointLightColors[3].y * 0.1,  pointLightColors[3].z * 0.1);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].diffuse"), pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].specular"), pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[3].constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[3].linear"), 0.14);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[3].quadratic"), 0.07);
//// SpotLight
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.diffuse"), 1.0f, 1.0f, 1.0f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.specular"), 1.0f, 1.0f, 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.linear"), 0.09);
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.quadratic"), 0.032);
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.cutOff"), glm::cos(glm::radians(10.0f)));
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.outerCutOff"), glm::cos(glm::radians(15.0f)));

// BIOCHEMICAL LAB
//// Directional light
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.ambient"), 0.5f, 0.5f, 0.5f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.diffuse"), 1.0f, 1.0f, 1.0f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.specular"), 1.0f, 1.0f, 1.0f);
//// Point light 1
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].ambient"), pointLightColors[0].x * 0.1,  pointLightColors[0].y * 0.1,  pointLightColors[0].z * 0.1);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].diffuse"), pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].specular"), pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[0].constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[0].linear"), 0.07);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[0].quadratic"), 0.017);
//// Point light 2
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].ambient"), pointLightColors[1].x * 0.1,  pointLightColors[1].y * 0.1,  pointLightColors[1].z * 0.1);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].diffuse"), pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].specular"), pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[1].constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[1].linear"), 0.07);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[1].quadratic"), 0.017);
//// Point light 3
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].ambient"), pointLightColors[2].x * 0.1,  pointLightColors[2].y * 0.1,  pointLightColors[2].z * 0.1);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].diffuse"), pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].specular") ,pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[2].constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[2].linear"), 0.07);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[2].quadratic"), 0.017);
//// Point light 4
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].ambient"), pointLightColors[3].x * 0.1,  pointLightColors[3].y * 0.1,  pointLightColors[3].z * 0.1);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].diffuse"), pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].specular"), pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[3].constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[3].linear"), 0.07);
//        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[3].quadratic"), 0.017);
//// SpotLight
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.diffuse"), 0.0f, 1.0f, 0.0f);
//        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.specular"), 0.0f, 1.0f, 0.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.constant"), 1.0f);
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.linear"), 0.07);
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.quadratic"), 0.017);
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.cutOff"), glm::cos(glm::radians(7.0f)));
//        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.outerCutOff"), glm::cos(glm::radians(10.0f)));
// DESERT
// Directional light
        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.direction"), -0.2f, -1.0f, -0.3f);
        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.ambient"), 0.3f, 0.24f, 0.14f);
        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.diffuse"), 0.7f, 0.42f, 0.26f);
        glUniform3f(glGetUniformLocation(lightingProgram, "dirLight.specular"), 0.5f, 0.5f, 0.5f);
// Point light 1
        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].position"), pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].ambient"), pointLightColors[0].x * 0.1,  pointLightColors[0].y * 0.1,  pointLightColors[0].z * 0.1);
        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].diffuse"), pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z);
        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[0].specular"), pointLightColors[0].x,  pointLightColors[0].y,  pointLightColors[0].z);
        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[0].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[0].linear"), 0.09);
        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[0].quadratic"), 0.032);
// Point light 2
        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].position"), pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].ambient"), pointLightColors[1].x * 0.1,  pointLightColors[1].y * 0.1,  pointLightColors[1].z * 0.1);
        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].diffuse"), pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z);
        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[1].specular"), pointLightColors[1].x,  pointLightColors[1].y,  pointLightColors[1].z);
        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[1].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[1].linear"), 0.09);
        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[1].quadratic"), 0.032);
// Point light 3
        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].position"), pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z);
        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].ambient"), pointLightColors[2].x * 0.1,  pointLightColors[2].y * 0.1,  pointLightColors[2].z * 0.1);
        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].diffuse"), pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z);
        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[2].specular") ,pointLightColors[2].x,  pointLightColors[2].y,  pointLightColors[2].z);
        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[2].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[2].linear"), 0.09);
        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[2].quadratic"), 0.032);
// Point light 4
        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].position"), pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z);
        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].ambient"), pointLightColors[3].x * 0.1,  pointLightColors[3].y * 0.1,  pointLightColors[3].z * 0.1);
        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].diffuse"), pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z);
        glUniform3f(glGetUniformLocation(lightingProgram, "pointLights[3].specular"), pointLightColors[3].x,  pointLightColors[3].y,  pointLightColors[3].z);
        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[3].constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[3].linear"), 0.09);
        glUniform1f(glGetUniformLocation(lightingProgram, "pointLights[3].quadratic"), 0.032);
// SpotLight
        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.position"), camera.Position.x, camera.Position.y, camera.Position.z);
        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.direction"), camera.Front.x, camera.Front.y, camera.Front.z);
        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.ambient"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.diffuse"), 0.8f, 0.8f, 0.0f);
        glUniform3f(glGetUniformLocation(lightingProgram, "spotLight.specular"), 0.8f, 0.8f, 0.0f);
        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.constant"), 1.0f);
        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.linear"), 0.09);
        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.quadratic"), 0.032);
        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.cutOff"), glm::cos(glm::radians(12.5f)));
        glUniform1f(glGetUniformLocation(lightingProgram, "spotLight.outerCutOff"), glm::cos(glm::radians(13.0f)));
        // Camera/View transformations
        glm::mat4 view = glm::mat4(1.0f);
        view = camera.GetViewMatrix();
        // Projection
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(camera.Zoom, (GLfloat)WIDTH/(GLfloat)HEIGHT, 0.1f, 100.0f);

        // Get their uniform location
        GLint modelLoc = glGetUniformLocation(lightingProgram, "model");
        GLint viewLoc = glGetUniformLocation(lightingProgram, "view");
        GLint projLoc = glGetUniformLocation(lightingProgram, "projection");
        // Pass them to the shaders
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // Note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // Bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        // Bind specular map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        // Draw container
        glBindVertexArray(containerVAO);
        glm::mat4 model = glm::mat4(1.0f);

        for(GLuint i = 0; i < 10; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            GLfloat angle = 20.0f * i;
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        // Also draw the lamp object, again binding the appropriate shader
        glUseProgram(lampProgram);
        // Get location objects for the matrices on the lamp shader (these could be different on a different shader)
        modelLoc = glGetUniformLocation(lampProgram, "model");
        viewLoc  = glGetUniformLocation(lampProgram, "view");
        projLoc  = glGetUniformLocation(lampProgram, "projection");
        // Set matrices
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        // Draw the light object (using light's vertex attributes)
        glBindVertexArray(lightVAO);
        for (GLuint i = 0; i < 4; i++)
        {
            model = glm::mat4() = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);



        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays(1, &containerVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

// Moves/alters the camera positions based on user input
void do_movement() {
    // Camera controls
    if(keys[GLFW_KEY_W])
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if(keys[GLFW_KEY_S])
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if(keys[GLFW_KEY_A])
        camera.ProcessKeyboard(LEFT, deltaTime);
    if(keys[GLFW_KEY_D])
        camera.ProcessKeyboard(RIGHT, deltaTime);
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

    if (action == GLFW_PRESS)
        keys[key] = true;
    else if (action == GLFW_RELEASE)
        keys[key] = false;
    do_movement();
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    // 这个bool变量一开始是设定为true的
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    GLfloat xoffset = xpos - lastX;
    GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int createShaderProgram(const std::string& filePath) {
    ShaderProgramSource source = ParseShader(filePath);
    std::cout << "VERTEX" <<source.VertexSource<< "FRAGMENT" << source.FragmentSource << std::endl;
    unsigned int program = CreateShader(source.VertexSource, source.FragmentSource);
    return program;
}
