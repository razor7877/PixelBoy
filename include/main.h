#ifndef MAIN_H
#define MAIN_H

extern uint8_t rom[0x8000];

static void glfw_error_callback(int error, const char* description);
int setup_glfw();
int setup_ImGui();
void update_texture();
void render_ImGui();
int load_rom(std::string path);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif