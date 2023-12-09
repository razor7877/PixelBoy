#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_glfw.h"
#include "ImGui/backends/imgui_impl_opengl3.h"
#include "ImGui/imgui_memory_editor.h"
#include <stdio.h>

#include "main.hpp"
#include "interface.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include "io.hpp"
#include "interrupts.hpp"
#include "rom.hpp"
#include "ppu.hpp"

// OpenGL context
GLFWwindow* window;
const char* glsl_version;

GLuint display_texture{};

uint8_t vram_buffer[384 * 8 * 8 * 4]; // VRAM stores up to 384 8x8 tiles
GLuint vram_texture{};

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

    ImGuiIO& io = ImGui::GetIO();

    if (io.WantCaptureKeyboard)
        return;

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

void drop_callback(GLFWwindow* window, int count, const char** paths)
{
    int i;
    for (i = 0; i < count; i++)
        printf("Dropped file: %s\n", paths[i]);
}

int setup_glfw()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return -1;

    // GL 3.0 + GLSL 130
    glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Create window with graphics context
    window = glfwCreateWindow(1280, 720, "GameBoy Emulator", NULL, NULL);
    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    return 0;
}

int setup_ImGui()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    io = ImGui::GetIO();

return 0;
}

void render_ImGui()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Gameboy display
    {
        update_texture();

        ImGui::Begin("Display");
        ImGui::Image((void*)(intptr_t)display_texture, ImVec2(160, 144));
        ImGui::End();
    }

    // CPU
    {
        ImGui::Begin("CPU", 0, ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize(ImVec2(200, 320));

        ImGui::Text("Cycle count: %d", cycle_count);

        ImGui::Text("Registers (hex/decimal):");
        ImGui::Text("AF: 0x%04x / %d", AF, AF);
        ImGui::Text("BC: 0x%04x / %d", BC, BC);
        ImGui::Text("DE: 0x%04x / %d", DE, DE);
        ImGui::Text("HL: 0x%04x / %d", HL, HL);

        ImGui::NewLine();

        ImGui::Text("pc: 0x%04x / %d", pc, pc);
        ImGui::Text("sp: 0x%04x / %d", sp, sp);

        ImGui::NewLine();

        ImGui::Text("Flags (F register):");

        ImGui::Text("Zero: %d", get_flags(FLAG_ZERO)); // Get 7th bit
        ImGui::Text("Substraction (BCD): %d", get_flags(FLAG_NEGATIVE)); // Get 6th bit
        ImGui::Text("Half Carry (BCD): %d", get_flags(FLAG_HALFCARRY)); // Get 5th bit
        ImGui::Text("Carry: %d", get_flags(FLAG_CARRY)); // Get 4th bit

        ImGui::NewLine();

        ImGui::End();
    }

    // Current opcode / Disassembly
    {
        ImGui::Begin("Opcode", 0, ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize(ImVec2(200, 120));

        ImGui::Text("Current opcode: 0x%02x", opcode);
        ImGui::Text("Disassembly: %s", instructions[opcode].disassembly);
        ImGui::Text("Operand length: %d", instructions[opcode].operand_length);
        ImGui::Text("Operand: 0x%02x", operand);

        ImGui::End();
    }

    // Memory explorer
    {
        static MemoryEditor mem_edit;
        mem_edit.DrawWindow("Memory Editor", memory, sizeof(memory));
    }

    // ROM explorer
    {
        static MemoryEditor rom_explorer;
        rom_explorer.DrawWindow("ROM Explorer", rom, sizeof(rom));
    }

    // VRAM explorer
    {
        static MemoryEditor vram_explorer;
        vram_explorer.DrawWindow("VRAM Explorer", vram, sizeof(vram));
    }

    // VRAM viewer
    {
        for (uint16_t tile = 0; tile < 384; tile++)
        {
            for (uint8_t row = 0; row < 8; row++)
            {
                for (uint8_t pixel = 0; pixel < 8; pixel++)
                {
                    uint8_t color = get_vram_pixel(tile * 16, (row * 8) + pixel) + 1;
                    uint32_t pixel_address = (64 * 4 * tile) + ((row * 8 + pixel) * 4);

                    vram_buffer[pixel_address] = 0x3F * color;
                    vram_buffer[pixel_address + 1] = 0x3F * color;
                    vram_buffer[pixel_address + 2] = 0x3F * color;
                    vram_buffer[pixel_address + 3] = 0x3F * color;
                }
            }
        }

        glGenTextures(1, &vram_texture);
        glBindTexture(GL_TEXTURE_2D, vram_texture);

        // Setup filtering parameters for display
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 192, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, vram_buffer);

        ImGui::Begin("VRAM viewer");
        ImGui::Image((void*)(intptr_t)vram_texture, ImVec2(192, 128));
        ImGui::End();
    }

    // OAM explorer
    {
        static MemoryEditor oam_explorer;
        oam_explorer.DrawWindow("OAM Explorer", OAM, sizeof(OAM));
    }

    // Joypad
    {
        ImGui::Begin("Joypad", 0, ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize(ImVec2(200, 200));

        ImGui::Text("I/O Register: %02x", io_register);
        ImGui::Text("Select action: %d", get_key(JOYPAD_ACTION));
        ImGui::Text("Select direction: %d", get_key(JOYPAD_DIRECTION));
        ImGui::Text("Down / Start: %d", get_key(JOYPAD_DOWN));
        ImGui::Text("Up / Select: %d", get_key(JOYPAD_UP));
        ImGui::Text("Left / B: %d", get_key(JOYPAD_LEFT));
        ImGui::Text("Right / A: %d", get_key(JOYPAD_RIGHT));

        ImGui::End();
    }

    // PPU
    {
        ImGui::Begin("PPU", 0, ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize(ImVec2(200, 260));

        ImGui::Text("Cycle count: %d", ppu_cycle_count);
        ImGui::Text("LCDC: %02x", LCDC);
        ImGui::Text("STAT: %02x", STAT);
        ImGui::Text("SCY: %02x", SCY);
        ImGui::Text("SCX: %02x", SCX);
        ImGui::Text("LY: %02x", LY);
        ImGui::Text("LYC: %02x", LYC);
        ImGui::Text("DMA: %02x", DMA);
        ImGui::Text("BGP: %02x", BGP);
        ImGui::Text("OBP0: %02x", OBP0);
        ImGui::Text("OBP1: %02x", OBP1);
        ImGui::Text("WY: %02x", WY);
        ImGui::Text("WX: %02x", WX);

        ImGui::End();
    }

    // Interrupts
    {
        ImGui::Begin("Interrupts", 0, ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize(ImVec2(100, 100));

        ImGui::Text("IME: %d", IME);
        ImGui::Text("IE: %x", IE);
        ImGui::Text("IF: %x", IF);

        ImGui::End();
    }

    // Timer
    {
        ImGui::Begin("Timer", 0, ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize(ImVec2(100, 100));

        ImGui::Text("DIV: %02x", (DIV & 0xFF00) >> 8);
        ImGui::Text("TIMA: %02x", TIMA);
        ImGui::Text("TMA: %02x", TMA);
        ImGui::Text("TAC: %02x", TAC);

        ImGui::End();
    }

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}

void update_texture()
{
    glGenTextures(1, &display_texture);
    glBindTexture(GL_TEXTURE_2D, display_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 160, 144, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame_buffer);
}

uint8_t get_vram_pixel(uint16_t tile_start, uint8_t pixel)
{
    uint8_t row = vram[tile_start + (pixel / 8) + (pixel % 8)];
    uint8_t upper_byte_mask = 0x1 << (8 + pixel % 8); // 0b10000000
    uint8_t lower_byte_mask = 0x1 << (pixel % 8);
    uint8_t color = (row & upper_byte_mask) | (row & lower_byte_mask << 1);

    return color;
}

int start_interface()
{
    if (setup_glfw() != 0)
        return -1;

    setup_ImGui();

    glfwSetKeyCallback(window, key_callback);
    glfwSetDropCallback(window, drop_callback);

    update_texture();

    return 0;
}

int update_interface()
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

    glfwPollEvents();

    render_ImGui();

    if (glfwWindowShouldClose(window))
        return 1;

    return 0;
}

void stop_interface()
{
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}