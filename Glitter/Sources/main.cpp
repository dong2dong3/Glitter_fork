// Local Headers
#include "glitter.hpp"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Cherno_Texture.h"
#include "ChernoShaderV2.h"

#include "Camera.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
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

static void errorCallback(int error, const char* description) {
    std::cerr << "GLFW Error " << error << ": " << description << std::endl;
}

static void GLCheckError() {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ")" << std::endl;
    }
}

// Window dimensions
const GLuint WIDTH = 640, HEIGHT = 480;
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
    GLFWwindow* window = glfwCreateWindow(1280, 960, "LearnOpenGL", nullptr, nullptr);
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

    /**
     * 交换间隔，交换缓冲区之前等待的帧数，通常称为v-sync
     * 默认情况下，交换间隔为0
     * 这里设置为1，即每帧更新一次
     **/
    glfwSwapInterval(1);

    {
        float positions[] = {
                -50.0f, -50.0f, 0.0f, 0.0f,  // 0
                50.0f, -50.0f, 1.0f, 0.0f,  // 1
                50.0f, 50.0f, 1.0f, 1.0f,  // 2
                -50.0f, 50.0f, 0.0f, 1.0f   // 3
        };

        // ±ØÐëÓÃ unsigned
        unsigned int indices[] = {
                0, 1, 2,
                2, 3, 0
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        unsigned int vao; /* 保存顶点数组对象ID */
        GLCall(glGenVertexArrays(1, &vao)); /* 生存顶点数组 */
        GLCall(glBindVertexArray(vao)); /* 绑定顶点数组 */

        VertexArray va;
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices, 6);

        /* glm::ortho 正交矩阵 */
        /* 这里应该是 960x720 而不是 960x540 的分辨率 */
        glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 720.0f, -1.0f, 1.0f);
        /* 相机位置 视图矩阵 x&y&z */
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
//        /* 模型矩阵 对象位置 */
//        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(200, 200, 0));
//
//        glm::mat4 mvp = proj * view * model; /* 模型视图投影矩阵 */

        ChernoShaderV2 shader(FilePathFor("Shaders/Basic.shader"));
        shader.Bind();
        shader.SetUniform4f("u_Color",  0.8f, 0.3f, 0.8f, 1.0f);

        Cherno_Texture texture(FilePathFor("Resources/textures/ChernoLogo.png"));
        texture.Bind();
        // bind texture to slot 0
        shader.SetUniform1i("u_Texture", 0);

        /* 解绑 */
        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        Renderer renderer;

        float r = 0.0f;
        float increment = 0.05f;
        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        glm::vec3 translationA(200, 200, 0);
        glm::vec3 translationB(400, 200, 0);

        ImGui::CreateContext(NULL);
        ImGui_ImplGlfwGL3_Init(window, true);
        ImGui::StyleColorsDark();

        // 渲染
        while (!glfwWindowShouldClose(window)) {
            /* Render here */
//        GLCall(glClear(GL_COLOR_BUFFER_BIT));
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderer.Clear();
            ImGui_ImplGlfwGL3_NewFrame();

            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
                glm::mat4 mvp = proj * view * model; /* 模型视图投影矩阵 */
                shader.Bind();
                shader.SetUniformMat4f("u_MVP", mvp);
                /* 绘制 */
                renderer.Draw(va, ib, shader);
            }

            {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
                glm::mat4 mvp = proj * view * model; /* 模型视图投影矩阵 */
                shader.Bind();
                shader.SetUniformMat4f("u_MVP", mvp);
                /* 绘制 */
                renderer.Draw(va, ib, shader);
            }
            va.Bind();
            ib.Bind();

            if (r > 1.0f) {
                increment = -0.1f;
            } else if (r < 0.0f) {
                increment = 0.1f;
            }
            r += increment;
            {
                ImGui::SliderFloat3("Translation A", &translationA.x, 0.0f, 960.0f);
                ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 960.0f);
                // Edit 1 float using a slider from 0.0f to 1.0f
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }
            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }

    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
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
