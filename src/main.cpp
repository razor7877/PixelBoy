#include <chrono>
#include <thread>

#include "main.hpp"
#include "cpu.hpp"
#include "interface.hpp"
#include "rom.hpp"

// Main code
int main(int, char**)
{
    //load_boot_rom("roms/dmg_boot.bin");
    //load_rom("roms/blargg/cpu_instrs/cpu_instrs.gb");
    //load_rom("roms/mooneye/acceptance/oam_dma_restart.gb");
    //load_rom("roms/Dr. Mario (World).gb");
    //load_rom("roms/Super Mario Land (JUE) (V1.1) [!].gb");
    //load_rom("roms/Pokemon - Blue Version (USA, Europe) (SGB Enhanced).gb");
    //load_rom("roms/Tetris (World) (Rev A).gb");

    start_interface();

    // Main loop
    for (;;)
    {
        if (update_interface() == 1)
            break;

        execute_frame();
        //std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    stop_interface();

    unload_rom();

    return 0;
}
