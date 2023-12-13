#include <stdio.h>

#include <glad/glad.h>

#include "frontends/opengl/frontend.h"
#include "frontends/opengl/audio.h"
#include "main.h"
#include "cpu.h"
#include "memory.h"
#include "io.h"
#include "interrupts.h"
#include "rom.h"
#include "ppu.h"

struct Frontend OpenGL_Frontend = {
    start_interface,
    update_interface,
    stop_interface,
    update_io
};

static GLFWwindow* window;
static GLuint display_texture = 0;

GLuint shader;
GLuint VAO;
GLuint VBO;
GLuint tex_coords_BO;

GLuint PBO; // Pixel buffer object
void* pbo_mem; // Pointer to the memory allocated for the PBO

// Startup resolution
#define WINDOW_WIDTH 480
#define WINDOW_HEIGHT 432

// Current resolution (can be changed by window resize callbacks)
int windowWidth = WINDOW_WIDTH;
int windowHeight = WINDOW_HEIGHT;

const float quad_verts[] = {
    -1.0f, 1.0f, 0.0f, // Top left
    1.0f, 1.0f, 0.0f, // Top right
    -1.0f, -1.0f, 0.0f, // Bottom left

    1.0f, 1.0f, 0.0f, // Top right
    1.0f, -1.0f, 0.0f, // Bottom right
    -1.0f, -1.0f, 0.0f, // Bottom left
};

const float quad_tex_coords[] = {
    0.0f, 0.0f, // Top left
    1.0f, 0.0f, // Top right
    0.0f, 1.0f, // Bottom left

    1.0f, 0.0f, // Top right
    1.0f, 1.0f, // Bottom right
    0.0f, 1.0f, // Bottom left
};


static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (action)
    {
        case GLFW_PRESS:
        {
            switch (key)
            {
            case GLFW_KEY_1:
                handle_instruction();
                break;

            case GLFW_KEY_2:
                for (int i = 0; i < 10; i++)
                    handle_instruction();
                break;

            case GLFW_KEY_3:
                for (int i = 0; i < 1000; i++)
                    handle_instruction();
                break;

            case GLFW_KEY_4:
                execute_frame();
                break;

            case GLFW_KEY_5:
                for (int i = 0; i < 60; i++)
                    execute_frame();
                break;

            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
            }
        }
    }
}

static void drop_callback(GLFWwindow* window, int count, const char** paths)
{
    int i;
    for (i = 0; i < count; i++)
        printf("Dropped file: %s\n", paths[i]);

    unload_rom();
    if (load_rom(paths[0]) == 0)
    {
        reset_emulator();
        
        // We create a null-terminated char array from the ROM title so that
        // GLFW doesn't read out of bounds
        char title[17];
        for (int i = 0; i < 16; i++)
        {
            title[i] = cartridge_header.title[i];
        }
        title[16] = '\0';

        glfwSetWindowTitle(window, title);
    }
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

// Sets up GLFW context
static int setup_glfw()
{
    // Intialize GLFW context
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Load a ROM file to run", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Set callback functions for window resizing and handling input
    glfwSetKeyCallback(window, key_callback);
    glfwSetDropCallback(window, drop_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Check if GLAD loaded successfully
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    return 0;
}

static void compile_shaders()
{
    const char* vShaderCode =
    "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;"
    "layout(location = 1) in vec2 aTexCoord;"
    "out vec2 TexCoord;"
    "void main()"
    "{"
    "   gl_Position = vec4(aPos, 1.0);"
    "   TexCoord = aTexCoord;"
    "}";

    const char* fShaderCode =
    "#version 330 core\n"
    "in vec2 TexCoord;"
    "out vec4 FragColor;"
    "uniform sampler2D texture1;"
    "void main()"
    "{"
    "   float col = texture(texture1, TexCoord).r;"
    "   vec3 green_scale = vec3(col * (128.0f / 255.0f), col * (160.0f / 255.0f), 48.0f / 255.0f);"
    "   FragColor = vec4(green_scale, 1.0);"
    "}";

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n %s\n", infoLog);
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }

    shader = glCreateProgram();
    glAttachShader(shader, vertex);
    glAttachShader(shader, fragment);
    glLinkProgram(shader);

    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader, 512, NULL, infoLog);
        printf("ERROR::SHADER::PROGRAM::LINKING_FAILED\n%s\n", infoLog);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

static void setup_quad()
{
    // Generate VAO and VBO and bind them
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    // Send vertices
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_verts), quad_verts, GL_STATIC_DRAW);

    // Enable layout 0 input in shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Send tex coords
    glGenBuffers(1, &tex_coords_BO);

    glBindBuffer(GL_ARRAY_BUFFER, tex_coords_BO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad_tex_coords), quad_tex_coords, GL_STATIC_DRAW);

    // Enable layout 1 input in shader
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
}

// Update the texture that is used to display the emulator screen
static void update_texture()
{
    glClearColor(1.0f, 0.5f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);
    pbo_mem = glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    memcpy(pbo_mem, frame_buffer, sizeof(frame_buffer));
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 160, 144, GL_RED, GL_UNSIGNED_BYTE, 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

// Sets up everything required for the interface
static int start_interface()
{
    if (setup_glfw() != 0)
        return -1;

    compile_shaders();
    setup_quad();
    
    glGenBuffers(1, &PBO);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBO);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, sizeof(frame_buffer), frame_buffer, GL_STREAM_DRAW);

    // Setup texture for displaying the emulator to screen
    glGenTextures(1, &display_texture);
    glBindTexture(GL_TEXTURE_2D, display_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 160, 144, 0, GL_RED, GL_UNSIGNED_BYTE, pbo_mem);
    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (start_audio() != 0)
        printf("Error while trying to start PortAudio!\n");

    return 0;
}

// Updates the interface (called once per frame)
static int update_interface()
{
    // Swap new frame and poll GLFW for inputs
    glfwSwapBuffers(window);
    glfwPollEvents();

    update_texture();

    if (glfwWindowShouldClose(window))
        return 1;

    return 0;
}

// Closes the interface and cleans up
static void stop_interface()
{
    stop_audio();

    glfwDestroyWindow(window);
    glfwTerminate();
}

static void update_io()
{
    if (get_key(JOYPAD_DIRECTION))
    {
        if (glfwGetKey(window, GLFW_KEY_DOWN))
            set_key(JOYPAD_DOWN);
        else
            clear_key(JOYPAD_DOWN);

        if (glfwGetKey(window, GLFW_KEY_UP))
            set_key(JOYPAD_UP);
        else
            clear_key(JOYPAD_UP);

        if (glfwGetKey(window, GLFW_KEY_LEFT))
            set_key(JOYPAD_LEFT);
        else
            clear_key(JOYPAD_LEFT);

        if (glfwGetKey(window, GLFW_KEY_RIGHT))
            set_key(JOYPAD_RIGHT);
        else
            clear_key(JOYPAD_RIGHT);
    }
    else if (get_key(JOYPAD_ACTION))
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
            set_key(JOYPAD_SELECT);
        else
            clear_key(JOYPAD_SELECT);

        if (glfwGetKey(window, GLFW_KEY_ENTER))
            set_key(JOYPAD_START);
        else
            clear_key(JOYPAD_START);

        if (glfwGetKey(window, GLFW_KEY_Q))
            set_key(JOYPAD_B);
        else
            clear_key(JOYPAD_B);

        if (glfwGetKey(window, GLFW_KEY_S))
            set_key(JOYPAD_A);
        else
            clear_key(JOYPAD_A);
    }
    else { io_register = (io_register & 0xF0) | 0x0F; }

}
