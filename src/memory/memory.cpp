#include "memory.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
void Chip8::init(){
    pc = 0x200;
    i = 0;
    sp = 0;
    drawFlag = false;

    std::ranges::fill(stack,0);
    std::ranges::fill(display,0);
    std::ranges::fill(V,0);

    u8 font[80] = {
                    0xf0,0x90,0x90,0x90,0xf0,//0
                    0x20,0x60,0x20,0x20,0x70,//1
                    0xF0,0x10,0xF0,0x80,0xF0,//2
                    0xF0,0x10,0xF0,0x10,0xF0,//3
                    0x90,0x90,0xF0,0x10,0x10,//4
                    0xF0,0x80,0xF0,0x10,0xF0,//5
                    0xF0,0x80,0xF0,0x90,0xF0,//6
                    0xF0,0x10,0x20,0x40,0x40,//7
                    0xF0,0x90,0xF0,0x90,0xF0,//8
                    0xF0,0x90,0xF0,0x10,0xF0,//9
                    0xF0,0x90,0xF0,0x90,0x90,//A
                    0xE0,0x90,0xE0,0x90,0xE0,//B
                    0xF0,0x80,0x80,0x80,0xF0,//C
                    0xE0,0x90,0x90,0x90,0xE0,//D
                    0xF0,0x80,0xF0,0x80,0xF0,//E
                    0xF0,0x80,0xF0,0x80,0x80
                };
    for(int i = 0; i<80; i++){
        memory[i+0x050] = font[i];
    }
}
bool Chip8::loadROM(const std::string &filename){
    std::ifstream file(filename,std::ios::binary | std::ios::ate);
    if(file.is_open()){
        std::streampos size = file.tellg();
        char* buffer = new char[size];

        file.seekg(0,std::ios::beg);
        file.read(buffer,size);
        file.close();

        for(int i = 0; i<size; i++){
            memory[0x200 + i] = buffer[i];
        }
        delete[] buffer;
        return true;
    }
    return false;
}
void Chip8::Cycle(){
    u16 opcode = ((memory[pc]<<8)|memory[pc+1]);
    u8 x = (opcode & 0x0f00) >> 8;
    u8 y = (opcode & 0x00f0) >> 4;
    u8 n = (opcode & 0x000f);
    u8 nn = (opcode & 0x00ff);
    u16 nnn = opcode & 0x0FFF;
    bool stepped = false; 
    switch(opcode & 0xF000) {
        case 0x0000:
            if(opcode == 0x00E0){
                std::ranges::fill(display, 0);
            }
            else if(opcode == 0x00EE){
                sp--;
                pc = stack[sp]; 
            }
            break;

        case 0x1000:
            pc = nnn;
            stepped = true; 
            break;

        case 0x2000:
            stack[sp] = pc;
            sp++;
            pc = nnn;
            stepped = true;
            break;

        case 0x3000:
            if(V[x] == nn)
                pc += 2;
            break;
        case 0x4000:
            if(V[x] != nn)
                pc += 2;
            break;
        case 0x5000:
            if(V[x] == V[y]) pc+=2;
            break;
        case 0x6000:
            V[x] = nn;
            break;

        case 0x7000:
            V[x] += nn;
            break;
        case 0x8000:{
            switch(opcode & 0x000F){
                case(0x0):
                    V[x] = V[y];
                    break;
                case(0x1):
                    V[x] = V[x] | V[y];
                    break;
                case(0x2):
                    V[x] = V[x]&V[y];
                    break;
                case(0x3):
                    V[x] = V[x]^V[y];
                    break;
                case(0x4):{
                    u16 sum = V[x] + V[y];
                    u16  carry = (sum > 255)?1:0;
                    V[x] = sum & 0xFF;
                    V[0xF] = carry;
                    break;
                }
                case(0x5):{
                    u16 diff = V[x] - V[y];
                    V[x] = diff;
                    u16 not_borrow = (V[x] >= V[y])?1:0;
                    V[0xF] = not_borrow;
                    break;
                }
                case(0x6):{
                    u16 lsb = V[x]&1;
                    V[x] = V[x] >> 1;
                    V[0xF] = lsb;
                    break;
                }
                case(0x7):{
                    u16 diff = V[y] - V[x];
                    V[x] = diff;
                    u16 not_borrow = (V[y] >= V[x])?1:0;
                    V[0xF] = not_borrow;
                    break;
                }
                case(0xE):{
                    uint8_t msb = (V[x] & 0x80) >> 7;
                    V[x] = V[x] << 1;
                    V[0xF] = msb;
                    break;
                }
            }
            break;
        }
        case 0x9000:
            if(V[x] != V[y]) pc+=2;
            break;
        case 0xA000:
            i = nnn;
            break;
        case 0xB000:
            pc = nnn + V[0];
            stepped = true;
            break;
        case 0xC000:
            u8 random_byte = rand()%256;
            V[x] = random_byte & nn;
            break;
        case 0xD000:
        {
            V[0xF] = 0;
            for(int row = 0; row < n; row++){
                uint8_t sprite = memory[i + row];
                for(int col = 0; col < 8; col++){
                    if(sprite & (0x80 >> col)){
                        int px = (V[x] + col) % 64;
                        int py = (V[y] + row) % 32;
                        int idx = py * 64 + px;

                        if(display[idx]) V[0xF] = 1;
                        display[idx] ^= 1;
                    }
                }
            }
            drawFlag = true;
            break;
        }    
        case 0xE000:{
            switch(opcode & 0x00FF){
                case 0x9E:{
                    if (keypad[V[x]] != 0) pc+=2;
                    break;
                }
                case 0xA1:{
                    if(keypad[V[x]] == 0) pc+=2;
                    break;
                }
            
            }
            break;
        }
        case 0xF000:
            switch(opcode & 0x00FF)
            {
                case 0x07:
                    V[x] = delay_timer;
                    break;
                case 0x0A:{
                    bool key_pressed = false;
                    for(int i = 0; i<16; i++){
                        if(keypad[i] != 0){
                            V[x] = i;
                            key_pressed = true;
                            break;
                        }
                    }
                    if(!key_pressed) pc -= 2;
                    break;
                }
                case 0x15:
                    delay_timer = V[x];
                    break;
                case 0x18:
                    sound_timer = V[x];
                    break;
                case 0x1E:
                    i += V[x];
                    break;

                case 0x29:
                    i = 0x050 + (V[x] * 5); 
                    break;
                case 0x33:{
                    u8 value = V[x];
                    u8 hundreds = value/100;
                    u8 tens = (value/10)%10;
                    u8 ones = value%10;
                    memory[i] = hundreds;
                    memory[i+1] = tens;
                    memory[i+2] = ones;
                    break;
                }
                case 0x55:{
                    for(int j = 0; j<=x; j++){
                        memory[i+j] = V[j];
                    }
                    break;
                }
                case 0x65:
                    for(int j = 0; j <= x; j++){
                        V[j] = memory[i + j];
                    }
                    break;
                default:
                    std::cout << "Unknown opcode in case 0xF000: " << std::hex << opcode << std::endl;
            }
            break;
        default:
            std::cout << "Unknown opcode: " << std::hex << opcode << std::endl;
    }

    // THE SINGLE PC INCREMENT
    if (!stepped) {
        pc += 2;
    }
}