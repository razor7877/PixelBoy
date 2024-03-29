#include "frontends/opengl/frontend.h"
#include "main.h"
#include "cpu.h"
#include "rom.h"

struct Frontend frontend;

// Main code
int main(int argc, char* argv[])
{
    frontend = OpenGL_Frontend;
    //load_rom("roms/Super Mario Land (JUE) (V1.1) [!].gb");
    //load_rom("roms/Legend of Zelda, The - Link's Awakening (U) (V1.2) [!].gb");

    // Drag and drop ROM loading
    if (argc > 1)
        load_rom(argv[1]);

    if (frontend.start() != 0)
        return -1;

    // Main loop
    for (;;)
    {
        if (frontend.update() == 1)
            break;

        execute_frame();
    }

    frontend.stop();
    unload_rom();

    return 0;
}
