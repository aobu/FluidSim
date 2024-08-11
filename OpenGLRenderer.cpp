#include "OpenGLRenderer.h"
#include "FluidSolver.h"
#include <iostream>

OpenGLRenderer::OpenGLRenderer(int width, int height, const char* title, int n, FluidSolver* fluidSolver)
    : m_width(width),
    m_height(height),
    m_title(title),
    m_window(nullptr),
    N(n),
    m_fluidSolver(fluidSolver),
    EBO(0),
    shaderProgram(0),
    texture(0),
    VAO(0),
    VBO(0)
{
}

OpenGLRenderer::~OpenGLRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &texture);
    glfwTerminate();
}

bool OpenGLRenderer::initialize() {
    // init glfw
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    // config
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create window
    m_window = glfwCreateWindow(m_width, m_height, m_title, nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    // make current context
    glfwMakeContextCurrent(m_window);

    // load function pointers with glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    // set viewport
    glViewport(0, 0, m_width, m_height);

    // register framebuffer size callback
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    // set shader buffrs textures
    setupShadersAndBuffers();

    return true;
}

GLuint compileShader(GLenum type, const char* source) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    // error check
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    return shader;
}

// create and link a shader program from vertex and fragment shader sources
GLuint createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    // create shader program and link shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check for linking errors
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // clean up shaders
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

void OpenGLRenderer::setupShadersAndBuffers() {
    // vert shader source
    const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    layout (location = 1) in vec2 aTexCoord;
    out vec2 TexCoord;
    void main() {
        gl_Position = vec4(aPos, 0.0, 1.0);
        TexCoord = aTexCoord;
    }
    )";

    // fragment shader source
    const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoord;
    out vec4 FragColor;
    uniform sampler2D texture1;
    void main() {
        float density = texture(texture1, TexCoord).r;
        FragColor = vec4(density, density, density, 1.0);
    }
    )";

    // compile and link shaders
    shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    float vertices[] = {
        // positions    // texture coords
        -1.0f,  1.0f,   0.0f, 1.0f,  // top left
         1.0f,  1.0f,   1.0f, 1.0f,  // top right
         1.0f, -1.0f,   1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f,   0.0f, 0.0f   // bottom left
    };

    unsigned int indices[] = {
        0, 1, 2,  // first triangle
        2, 3, 0   // second triangle
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // generate texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void OpenGLRenderer::renderDensityGrid(float* densityGrid) {
    // update the texture with the new density grid
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, N + 2, N + 2, 0, GL_RED, GL_FLOAT, densityGrid);

    // render the quad
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void OpenGLRenderer::run() {
    // main loop
    while (!glfwWindowShouldClose(m_window)) {
        // input
        processInput();

        // step fluid simulation
        m_fluidSolver->Step();

        // rendering commands here
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // get density grid from fluid solver
        float* densityGrid = m_fluidSolver->GetDensity();

        // render density grid
        renderDensityGrid(densityGrid);

        // swap buffers and poll IO events
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

void OpenGLRenderer::framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void OpenGLRenderer::processInput() {
    // check escape is pressed to close  window
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(m_window, true);

    // state of the left and right mouse buttons
    int leftMouseButtonState = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_LEFT);
    int rightMouseButtonState = glfwGetMouseButton(m_window, GLFW_MOUSE_BUTTON_RIGHT);

    // mouse position
    double xpos, ypos;
    glfwGetCursorPos(m_window, &xpos, &ypos);

    static double lastXpos = xpos, lastYpos = ypos;

    // mouse coordinates to grid coordinates
    int gridX = static_cast<int>(xpos / m_width * N);
    int gridY = static_cast<int>((m_height - ypos) / m_height * N); // invert Y

    if (rightMouseButtonState == GLFW_PRESS) {
        // right-click: add fluid density at  current mouse position
        addFluid(gridX, gridY);
    }

    if (leftMouseButtonState == GLFW_PRESS) {
        // left-click: calculate the velocity from mouse movement
        float velocityX = static_cast<float>(xpos - lastXpos);
        float velocityY = static_cast<float>(lastYpos - ypos); // invert Y

        velocityX = velocityX / 10;
        velocityY = velocityY / 10;
        // apply velocity to fluid at current mouse position
        addVelocity(gridX, gridY, velocityX, velocityY);
    }

    // update last mouse position
    lastXpos = xpos;
    lastYpos = ypos;
}

void OpenGLRenderer::addFluid(int x, int y) {
    m_fluidSolver->AddInputToField(DENSITY, x, y, 30.0f);
}

void OpenGLRenderer::addVelocity(int x, int y, float velocityX, float velocityY) {
    m_fluidSolver->AddInputToField(VELOCITY_U, x, y, velocityX);
    m_fluidSolver->AddInputToField(VELOCITY_V, x, y, velocityY);
}