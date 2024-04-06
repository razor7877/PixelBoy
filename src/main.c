#if !defined(_WIN32) && !defined(__linux__) && !defined(__APPLE__)
#error This is not a supported platform for the build! (Windows/Linux/macOS supported)
#endif
// TODO : There seems to be some non-working games on Linux build but not Win10

#include <time.h>
#ifdef _WIN32
#include <Windows.h>
#endif

#include "frontends/opengl/frontend.h"
#include "main.h"
#include "cpu.h"
#include "rom.h"
#include "logging.h"

// The time that one frame should take on the GB, in nanoseconds
uint64_t frame_time = (double)FRAME_CYCLES / (double)CPU_FREQ * 1000000000;

struct Frontend frontend;
uint64_t last_time = 0;
uint64_t current_time = 0;

uint64_t time_now(void)
{
    struct timespec ts;

    if (timespec_get(&ts, TIME_UTC) != TIME_UTC)
    {
        log_error("timespec_get() failed!");
        return 0;
    }

    return 1000000000 * ts.tv_sec + ts.tv_nsec;
}

#ifdef _WIN32
void sleep(uint64_t nanoseconds)
{
    Sleep(nanoseconds / 1000000);
}
#else
void sleep(uint64_t nanoseconds)
{
    struct timespec rem;
    struct timespec req = {
        0,
        nanoseconds
    };

    nanosleep(&req, &rem);
}
#endif

// Main code
int main(int argc, char* argv[])
{
    frontend = OpenGL_Frontend;

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
    while (frontend.update() != 1)
    {
        last_time = current_time;
        current_time = time_now();

        execute_frame();

        uint64_t elapsed_time = current_time - last_time;
        if (frame_time > elapsed_time)
        {
            uint64_t remaining_time = frame_time - elapsed_time;
            sleep(remaining_time);
        }
    }

    frontend.stop();
    unload_rom();

    return 0;
}
