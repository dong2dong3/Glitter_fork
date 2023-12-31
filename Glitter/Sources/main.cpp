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
#include "FileHandler.h"
#include "Model.h"

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
    std::cout << "Current working directory: " << ExecutableParentPath << std::endl;

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

    // Options
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Setup and compile our shaders
    unsigned int shaderProgram = CreateShaderProgram(FilePathFor("Shaders/model_loading.shader"));

    // Load models
    Model ourModel(FilePathFor("Resources/objects/nanosuit/nanosuit.obj"));

    // Draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // 渲染
    while (!glfwWindowShouldClose(window))
    {

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();
        do_movement();

        // Render
        glClearColor(0.75f, 0.52f, 0.3f, 1.0f);

        // glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate deltatime of current frame
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glUseProgram(shaderProgram);


        // Transformation matrices
        glm::mat4 projection = glm::perspective(camera.Zoom, (float)WIDTH/(float)HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // Draw the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); // Translate it down a bit so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// It's a bit too big for our scene, so scale it down
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        ourModel.Draw(shaderProgram);

        // Swap the buffers
        glfwSwapBuffers(window);
    }


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
