#include <chrono>
#include <thread>

#include "frontends/imgui_frontend.hpp"
#include "frontends/opengl_frontend.hpp"
#include "main.hpp"
#include "cpu.hpp"
#include "rom.hpp"

Frontend frontend = OpenGL_Frontend;

// Main code
int main(int, char**)
{
    //load_boot_rom("roms/dmg_boot.bin");
    //load_rom("roms/blargg/cpu_instrs/cpu_instrs.gb");
    //load_rom("roms/blargg/cpu_instrs/individual/01-special.gb");
    load_rom("roms/Super Mario Land (JUE) (V1.1) [!].gb");

    if (frontend.start() != 0)
        return -1;

    // Main loop
    for (;;)
    {
        if (frontend.update() == 1)
            break;

        execute_frame();
        //std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    frontend.stop();

    unload_rom();

    return 0;
}
