#ifndef IMGUI_FRONTEND_H
#define IMGUI_FRONTEND_H

#include <GLFW/glfw3.h>

#include "frontends/frontend.hpp"

extern struct Frontend ImGui_Frontend;

static void glfw_error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void drop_callback(GLFWwindow* window, int count, const char** paths);

// Sets up GLFW context
static int setup_glfw();
// Sets up ImGui context
static int setup_ImGui();

static void render_ImGui();
// Update the texture that is used to display the emulator screen
static void update_texture();
static uint8_t get_vram_pixel(uint16_t tile_start, uint8_t pixel);

// Sets up everything required for the interface
static int start_interface();
// Updates the interface (called once per frame)
static int update_interface();
// Closes the interface and cleans up
static void stop_interface();
static void update_io();

#endif