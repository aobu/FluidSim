#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class FluidSolver;

class OpenGLRenderer {
public:

    // ==================================================
    // VARIABLES
    // ==================================================

    

    // ==================================================
    // FUNCTIONS
    // ==================================================

    OpenGLRenderer(int width, int height, const char* title, int n, FluidSolver* fluidSolver);
    ~OpenGLRenderer();

    bool initialize();
    void run();

private:

    // ==================================================
    // VARIABLES
    // ==================================================

    int m_width, m_height;
    const char* m_title;
    GLFWwindow* m_window;
    int N;
    FluidSolver* m_fluidSolver;

    GLuint shaderProgram;
    GLuint VAO, VBO, EBO;
    GLuint texture;

    // ==================================================
    // FUNCTIONS
    // ==================================================

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void processInput();
    void setupShadersAndBuffers();
    void renderDensityGrid(float* densityGrid);

    // Helper methods to add fluid and velocity
    void addFluid(int x, int y);
    void addVelocity(int x, int y, float velocityX, float velocityY);
};

#endif // OPENGLRENDERER_H
