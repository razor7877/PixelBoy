#ifndef OPENGL_FRONTEND_H
#define OPENGL_FRONTEND_H

#include <GLFW/glfw3.h>

#include "frontends/frontend.hpp"

extern struct Frontend OpenGL_Frontend;

static void glfw_error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void drop_callback(GLFWwindow* window, int count, const char** paths);

// Sets up GLFW context
static int setup_glfw();
static void compile_shaders();
static void setup_quad();

// Update the texture that is used to display the emulator screen
static void update_texture();

// Sets up everything required for the interface
static int start_interface();
// Updates the interface (called once per frame)
static int update_interface();
// Closes the interface and cleans up
static void stop_interface();
static void update_io();

#endif