#ifndef INTERFACE_H
#define INTERFACE_H

#include <GLFW/glfw3.h>

static void glfw_error_callback(int error, const char* description);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Sets up GLFW context
int setup_glfw();
// Sets up ImGui context
int setup_ImGui();

void render_ImGui();
// Update the texture that is used to display the emulator screen
void update_texture();
uint8_t get_vram_pixel(uint16_t tile_start, uint8_t pixel);

// Sets up everything required for the interface
int start_interface();
// Updates the interface (called once per frame)
int update_interface();
// Closes the interface and cleans up
void stop_interface();

#endif