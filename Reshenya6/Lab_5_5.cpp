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

glm::vec3 CameraPos = glm::vec3(50.0, 5.0, 50.0);
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
void setMat3(Shader shader, const std::string& name, const glm::mat3& mat) {
    glUniformMatrix3fv(glGetUniformLocation(shader.shaderProgramID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void handleInput(GLFWwindow* window);

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

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "W", NULL, NULL);
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
    Model model("mu.obj");

    while (!glfwWindowShouldClose(window)) {
        handleInput(window);

        view = glm::lookAt(CameraPos, CameraPos + CameraFront, CameraUp);
        float t = 0.0f;
        t = glfwGetTime();
        glClearColor(0.1f, 0.0f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        
        // Матрицы
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        shader.setMat4("model", glm::value_ptr(modelMatrix));
        shader.setMat4("view", glm::value_ptr(view));
        shader.setMat4("projection", glm::value_ptr(projection));

        // Материал
        shader.setVec3("material.ambient", sin(t+0.1f), 0.2f, 0.8f);
        shader.setVec3("material.diffuse", 0.7f, 0.7f, 0.4f);
        shader.setVec3("material.specular", 1.0f, 1.0f, 1.0f);
        shader.setFloat("material.shininess", 128.0f);

        // Свет
        shader.setVec3("light.ambient", 0.7f, 0.7f, 0.7f);
        shader.setVec3("light.diffuse", 0.4f, 0.7f, 0.4f);
        shader.setVec3("light.specular", 0.7f, 0.7f, 0.7f);
        shader.setVec3("light.position", 40.0f, 20.0f, 10.0f);

        // Камера
        shader.setVec3("viewPos", CameraPos.x, CameraPos.y, CameraPos.z);

        // Матрица нормалей
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
        setMat3(shader, "normalMatrix", normalMatrix);

        // Рендер модели
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
