//ЭВМб-23-1 - Бузиков Н.А - Вариант 3//

#define GLEW_DLL
#define GLFW_DLL

#include <cstdio>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "ShederLab.h"

GLfloat points[] = {
    -0.8f,  0.0f, 0.0f, 
     -0.8f,  0.8f, 0.0f,  
     0.8f, 0.8f, 0.0f,  
    0.8f, 0.0f, 0.0f  
};

GLuint index[] = {
    0, 1, 2,  
    0, 2, 3   
};

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window;
    window = glfwCreateWindow(512, 512, "Qvadro", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Shader ourShader("vertex_shader.glsl", "fragment_shader.glsl");

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1, 0.7, 0.9, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        float timeValue = glfwGetTime();
        float Red = abs(sin(timeValue));
        float Green = abs(cos(timeValue));
        float Blue = abs(sin(timeValue));

        ourShader.Use();
        ourShader.SetUniform("ourColour", Red, Green, Blue, 0.5);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}