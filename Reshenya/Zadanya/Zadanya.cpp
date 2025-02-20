#define GLEW_DLL
#define GLFW_DLL

#include <cstdio>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main()
{
    printf("Hello World!\n");
    glfwInit();
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: could not start GLFW3.\n");
        return 1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* Oboyudno;
    Oboyudno = glfwCreateWindow(512, 512, "Qvadro", NULL, NULL);
    
    glfwMakeContextCurrent(Oboyudno);
    glewExperimental = GL_TRUE;
    glewInit();

    while (!glfwWindowShouldClose(Oboyudno)) {
        glClearColor(0.1, 0.7, 0.9, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        glColor3f(0.8, 0.3, 0.4);
        glBegin(GL_QUADS);

        glVertex2f(-0.8, 0);
        glVertex2f(-0.8, 0.8);
        glVertex2f(0.8, 0.8);
        glVertex2f(0.8, 0);
        

        glEnd();

        glfwSwapBuffers(Oboyudno);
        glfwPollEvents();
    }
    glfwTerminate();
}
