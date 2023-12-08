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
    //load_rom("roms/blargg/cpu_instrs/individual/11-op a,(hl).gb");
    //load_rom("roms/mooneye/acceptance/oam_dma_restart.gb");
    //load_rom("roms/Dr. Mario (World).gb");
    load_rom("roms/Tetris (World) (Rev A).gb");

    start_interface();

    // Main loop
    for (;;)
    {
        execute_cycle();
        if (update_interface() == 1)
            break;

        //std::this_thread::sleep_for(std::chrono::milliseconds(4));
    }

    stop_interface();

    return 0;
}
