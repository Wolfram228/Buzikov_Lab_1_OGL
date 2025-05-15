#include "Shader.h"
#include "Model.h"
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <matrix_transform.hpp>
#include <type_ptr.hpp>
#include <iostream>

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;

glm::vec3 cameraPos = glm::vec3(25.0f, 5.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float yaw = -90.0f;
float pitch = 0.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct ObjectTransform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 pivotPoint = glm::vec3(0.0f);

    struct {
        float min = -25.0f;
        float max = 20.0f;
    } zLimit;

    struct {
        float min = -3.0f;
        float max = 3.0f;
    } yLimit;

    struct {
        float min = -90.0f;
        float max = 90.0f;
    } rotationLimit;
};

std::vector<ObjectTransform> objectTransforms;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

glm::mat4 calculateModelMatrix(int index) {
    glm::mat4 model = glm::mat4(1.0f);

    if (index == 0) {
        // Основа - без преобразований
        return model;
    }

    if (index == 1) {
        // X01 - только перемещение по Z
        model = glm::translate(model, objectTransforms[1].position);
        return model;
    }

    if (index == 2) {
        // X02 - перемещается вместе с X01, затем по Y
        model = glm::translate(model, objectTransforms[1].position); // Наследуем движение X01
        model = glm::translate(model, objectTransforms[2].position); // Собственное движение по Y
        return model;
    }

    if (index == 3) {
        // X03 - перемещается с X01 и X02, затем вращается по оси Y
        model = glm::translate(model, objectTransforms[1].position); // Наследуем движение X01
        model = glm::translate(model, objectTransforms[2].position); // Наследуем движение X02
        model = glm::translate(model, objectTransforms[3].pivotPoint); // Перемещаем к точке вращения
        model = glm::rotate(model, glm::radians(objectTransforms[3].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)); // Вращение по Y
        model = glm::translate(model, -objectTransforms[3].pivotPoint); // Возвращаем обратно
        return model;
    }

    return model;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Model Transformations", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader shader("vertex_sheder.glsl", "fragment_shader.glsl");
    Model ourModel("mu.obj");

    objectTransforms.resize(4);

    // Настройка ограничений и параметров
    objectTransforms[1].zLimit = { -10.7f, 2.5f }; // Ограничения для X01 (движение по Z)
    objectTransforms[2].yLimit = { -1.7f, 0.5f };  // Ограничения для X02 (движение по Y)
    objectTransforms[3].rotationLimit = { -180.0f, 180.0f }; // Ограничения для X03 (вращение)
    objectTransforms[3].pivotPoint = glm::vec3(23.7627f, -0.68502f, 0.7057f); // Точка вращения для клешни

    shader.use();

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float t = 0.0f;
        t = glfwGetTime();
        shader.setVec3("light.position", glm::vec3(30.0f, 10.0f, 5.0f));
        shader.setVec3("light.ambient", glm::vec3(0.7f, 0.7f, 0.7f));
        shader.setVec3("light.diffuse", glm::vec3(0.4f, 0.7f, 0.4f));
        shader.setVec3("light.specular", glm::vec3(1.0f));
        shader.setVec3("material.ambient", glm::vec3(sin(t + 0.1f), 0.2f, 0.8f));
        shader.setVec3("material.diffuse", glm::vec3(0.7f, 0.7f, 0.4f));
        shader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        shader.setFloat("material.shininess", 128.0f);

        processInput(window);

        glClearColor(0.1f, 0.0f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        shader.setVec3("viewPos", cameraPos);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        for (size_t i = 0; i < ourModel.meshTransforms.size(); ++i) {
            ourModel.meshTransforms[i] = calculateModelMatrix(i);
        }

        ourModel.Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 15.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    float moveSpeed = 20.5f * deltaTime;
    float rotateSpeed = 90.0f * deltaTime;

    // Управление X01 (движение по Z) - клавиши Y/H
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        objectTransforms[1].position.z += moveSpeed;
        if (objectTransforms[1].position.z > objectTransforms[1].zLimit.max)
            objectTransforms[1].position.z = objectTransforms[1].zLimit.max;
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        objectTransforms[1].position.z -= moveSpeed;
        if (objectTransforms[1].position.z < objectTransforms[1].zLimit.min)
            objectTransforms[1].position.z = objectTransforms[1].zLimit.min;
    }

    // Управление X02 (движение по Y) - клавиши U/J
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        objectTransforms[2].position.y += moveSpeed;
        if (objectTransforms[2].position.y > objectTransforms[2].yLimit.max)
            objectTransforms[2].position.y = objectTransforms[2].yLimit.max;
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        objectTransforms[2].position.y -= moveSpeed;
        if (objectTransforms[2].position.y < objectTransforms[2].yLimit.min)
            objectTransforms[2].position.y = objectTransforms[2].yLimit.min;
    }

    // Управление X03 (вращение по Y) - клавиши I/K
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        objectTransforms[3].rotation.y += rotateSpeed;
        if (objectTransforms[3].rotation.y > objectTransforms[3].rotationLimit.max)
            objectTransforms[3].rotation.y = objectTransforms[3].rotationLimit.max;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        objectTransforms[3].rotation.y -= rotateSpeed;
        if (objectTransforms[3].rotation.y < objectTransforms[3].rotationLimit.min)
            objectTransforms[3].rotation.y = objectTransforms[3].rotationLimit.min;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
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
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
}