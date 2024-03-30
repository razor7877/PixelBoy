#include "frontends/opengl/frontend.h"
#include "main.h"
#include "cpu.h"
#include "rom.h"
#include "logging.h"

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

    log_info("info test\n");
    log_debug("debug test\n");
    log_warning("warning test\n");
    log_error("error test\n");

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
