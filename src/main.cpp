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
    load_rom("roms/mooneye/acceptance/timer/tim00_div_trigger.gb");

    start_interface();

    // Main loop
    for (;;)
    {
        execute_cycle();
        if (update_interface() == 1)
            break;

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    stop_interface();

    return 0;
}
