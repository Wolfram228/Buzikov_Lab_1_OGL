#include "Shader.h"
#define GLEW_DLL
#define GLFW_DLL
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm.hpp"
#include "matrix_transform.hpp"
#include "type_ptr.hpp"
#include <iostream>
#include "Model.h"

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

glm::vec3 CameraPos = glm::vec3(0.0, 0.0, 5.0);
glm::vec3 CameraFront = glm::vec3(0.0, 0.0, -1.0);
glm::vec3 CameraUp = glm::vec3(0.0, 1.0, 0.0);

float lastX = SCR_WIDTH / 2;
float lastY = SCR_HEIGHT / 2;
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;

glm::mat4 projection = glm::perspective(
    glm::radians(45.0f),
    (float)SCR_WIDTH / (float)SCR_HEIGHT,
    0.1f,
    100.0f);

glm::mat4 view = glm::lookAt(CameraPos, CameraPos + CameraFront, CameraUp);

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void handleInput(GLFWwindow* window);

//Камера//
void handleMovement(GLFWwindow* window, float speed) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        CameraPos += speed * CameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        CameraPos -= speed * CameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        CameraPos -= glm::normalize(glm::cross(CameraFront, CameraUp)) * speed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        CameraPos += glm::normalize(glm::cross(CameraFront, CameraUp)) * speed;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D MODELECHKA", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader shader("shader/vertex_sheder.glsl", "shader/fragment_shader.glsl");

    ////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////Модель///////////////////////////////////////////
    Model model("mu.obj");
    ///////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////

    while (!glfwWindowShouldClose(window)) {
        handleInput(window);

        view = glm::lookAt(CameraPos, CameraPos + CameraFront, CameraUp);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setMat4("view", glm::value_ptr(view));
        shader.setMat4("projection", glm::value_ptr(projection));

        model.Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    CameraFront = glm::normalize(front);
}

void handleInput(GLFWwindow* window) {

    float moveSpeed = 2.5f * 0.016f; 

    handleMovement(window, moveSpeed);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
