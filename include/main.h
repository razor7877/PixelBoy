#ifndef MAIN_H
#define MAIN_H

#include <string>

extern uint8_t rom[0x8000];

void update_texture();
int load_rom(std::string path);

#endif