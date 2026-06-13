#include "memory.h"
#include <algorithm>
#include <fstream>
#include <iostream>
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
    switch(opcode & 0xF000){
        case 0x0000:
            if(opcode == 0x00E0){
                std::ranges::fill(display,0);
                pc += 2;
            }
            else if(opcode == 0x00EE){
                sp--;
                pc = stack[sp];
            }
            break;
        case 0x1000:
            pc = nnn;
            break;
        case 0x2000:
            stack[sp] = pc;
            sp++;
            pc = nnn;
            break;
        case 0x6000:
            V[x] = nn;
            pc+=2;
            break;
        case 0x7000:
            V[x] += nn;
            pc+=2;
            break;
        case 0xA000:
            i = nnn;
            pc+=2;
            break;
        case 0xD000:
        {
            V[0xF] = 0;
            for(int row = 0; row<n; row++){
                u8 sprite = memory[i+row];
                for(int col = 0; col<8; col++){
                    if(sprite & (0x80 >> col)){
                        int px = (V[x] + col)%64;
                        int py = (V[y] + row)%32;

                        int idx = py*64 + px;
                        if(display[idx]) V[0xF] = 1;

                        display[idx] ^= 1;
                    }
                }
            }
            drawFlag = true;
            pc+=2;
            break;
        }    
        case 0xF000:
            switch(opcode & 0x00FF)
            {
                case 0x1E:
                    i += V[x];
                    pc+=2;
                    break;

                case 0x29:
                    i = V[x]*5;
                    pc+= 2;
                    break;

                case 0x65:
                    for(int j = 0; j<=x; j++){
                        V[j] = memory[i+j];
                    }
                    pc+=2;
                    break;
                default:
                    std::cout<<"Unknown opcode in the case 0xF000..."<<std::hex<<opcode<<std::endl;
            }
            break;
        default:
            std::cout<<"Unknown opcode... "<<std::hex<<opcode<<std::endl;
    }
}