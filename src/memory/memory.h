#pragma once
#include <cstdint>
#include <string>
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
class Chip8{
    public:
    //Ram
    u8 memory[4096];
    //Program counter
    u16 pc;
    //General purpose registers
    u8 V[16];
    //Index register
    u16 i;
    //Delay timer
    u8 delay_timer;
    //Sound timer
    u8 sound_timer;
    //Keypad
    u16 keypad[16];
    //Display
    u32 display[64*32];
    //Stack
    u16 stack[16];
    //Stack Pointer
    u8 sp;
    //Draw Flag
    bool drawFlag;

    //Initialize
    void init();
    bool loadROM(const std::string &filename);
    void Cycle();
};