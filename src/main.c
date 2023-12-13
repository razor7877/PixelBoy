#include "frontends/opengl_frontend.h"
#include "main.h"
#include "cpu.h"
#include "rom.h"

struct Frontend frontend;

// Main code
int main(int argc, char* argv[])
{
    frontend = OpenGL_Frontend;
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
