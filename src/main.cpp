#include <chrono>
#include <thread>

#include "main.h"
#include "cpu.h"
#include "interface.h"
#include "rom.h"

// Main code
int main(int, char**)
{
    //load_boot_rom("roms/dmg_boot.bin");
    
    //load_rom("roms/blargg/instr_timing/instr_timing.gb");
    //load_rom("roms/blargg/cpu_instrs/individual/02-interrupts.gb");
    load_rom("roms/mooneye/acceptance/timer/tim00.gb");

    //load_rom("roms/Tetris (World) (Rev A).gb");
    //load_rom("roms/Dr. Mario (World).gb");

    start_interface();

    // Main loop
    for (;;)
    {
        execute_cycle();
        if (update_interface() == 1)
            break;

        std::this_thread::sleep_for(std::chrono::milliseconds(4));
    }

    stop_interface();

    return 0;
}
