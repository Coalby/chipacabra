#ifndef CHIP_8_H
#define CHIP_8_H

#include <array>
#include <fstream>
#include "display.h"
#include "opcode.h"

#define CHIP_8_MEM_SIZE     0x1000
#define CHIP_8_RESERVED_MEM 0x200
#define CHIP_8_MEM_START    0
#define ROM_MEM_SIZE        CHIP_8_MEM_SIZE - CHIP_8_RESERVED_MEM
#define ROM_MEM_START       CHIP_8_RESERVED_MEM
#define REGISTER_COUNT      0x10
#define STACK_SIZE          12
#define KEY_SIZE            0x10

#define OVERFLOW_OCCURED        0x01
#define OVERFLOW_DID_NOT_OCCUR  0x00

#define UNDERFLOW_OCCURED       0x00
#define UNDERFLOW_DID_NOT_OCCUR 0x01

// TODO: Replace with constexpr?
#define ADDR_BOUNDARY_DETECT(addr)          (addr >= 0 && addr < CHIP_8_MEM_SIZE)
#define REGISTER_BOUNDARY_DETECT(register)  (register >= 0 && register < REGISTER_COUNT)
#define MEMORY_SIZE_DETECT(size)            (size <= 0 && size > ROM_MEM_SIZE)
#define KEY_BOUNDARY_CHECK(key)             (key <= 0 && key > KEY_SIZE)

const unsigned char FontSet[] {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

class Chip8 {
    public:
        Chip8() {
            writeMemory(&FontSet, sizeof(FontSet), CHIP_8_MEM_START);
        };

        ~Chip8() {};

        void readNextInstruction() {
            // Did not use PC++ on both to ease future development
            opcodes.executeOpcode(GET_OPCODE(memory[PC], memory[PC+1]), *this);
        };

        // Are C-style arrays the best choice here? 
        // Don't want to use vectors for embedded due to dynamic allocation
        char writeMemory(const void* data, size_t size, size_t offset) {
            if(MEMORY_SIZE_DETECT(size + offset))
                return -1;
            
            const unsigned char* byteData = static_cast<const unsigned char*>(data);
            std::copy(byteData, byteData + size, memory + offset);

            return 0;
        };

        // Is this the best format for default variables?
        void printMemory(size_t offset = 0, size_t size = CHIP_8_MEM_SIZE) const {
            // TODO: Range Checking
            // TODO: Replace this with better logging system
            while(offset < size) {
                if(offset % 16 == 0)
                    printf("\n0x%04X: ", static_cast<unsigned int>(offset));
                printf("%02X ", memory[offset]);
                offset++;
            }

            printf("\n");   // Makes output clearer

            return;
        };

        void printDebug(void) {
            printf("Last used opcode: %02X%02X ", memory[PC], memory[PC+1]);
            printf("PC: %04X    SP: %02X    I: %04X\n", PC, SP, I);
            return;
        };

        // Using -1 as error detection. Should be good since we only go up to 0xFFF
        const short getMachineCode(unsigned short addr) const {
            return (ADDR_BOUNDARY_DETECT(addr) ? memory[addr] : -1);
        };

        // TODO: Don't all these access functions defeat the purpose of OOP? Alternative structure?
        char setProgramCounter(unsigned short addr) {
            if(!(ADDR_BOUNDARY_DETECT(addr)))
                return -1;
            
            PC = addr;
            return 0;
        };

        char addProgramCounter(unsigned short value) {
            if((PC + value) >= 0xFFF)
                return -1;

            PC += value;
            return 0;
        };

        const unsigned char getRegisterValue(unsigned char registerNumber) const {
            return (REGISTER_BOUNDARY_DETECT(registerNumber) ? v[registerNumber] : -1);
        };

        char setRegisterValue(unsigned char registerNumber, unsigned char value) {
            if(!REGISTER_BOUNDARY_DETECT(registerNumber))
                return -1;

            v[registerNumber] = value;
            return 0;
        };

        char setOverflowRegister(unsigned char overflow) {
            if(overflow != OVERFLOW_OCCURED && overflow != OVERFLOW_DID_NOT_OCCUR)
                return 0;

            v[REGISTER_COUNT-1] = overflow;

            return 1;
        }

        char setI(unsigned short value) {
            // TODO: Include boundary checking
            I = value;

            return 0;
        };

        char setKey(unsigned short key) {
            if(!KEY_BOUNDARY_CHECK(key))
                return -1;

            key_pressed = key;
            
            return 0;
        };

        const pixels::PixelBuffer& getPixels() const {
            return pixels;
        };
        
        // Using friend so opcodes can access memory/stack/regis
        friend class Opcodes;

    private:
        Opcodes opcodes;
        unsigned char memory[CHIP_8_MEM_SIZE];
        unsigned char v[REGISTER_COUNT];
        unsigned short stack[STACK_SIZE];

        unsigned short PC {ROM_MEM_START}; // Have PC start on ROM
        unsigned char SP {};
        unsigned short I {};

        unsigned short delay_timer {};
        unsigned short sound_timer {};

        unsigned char key_pressed {};

        pixels::PixelBuffer pixels {};
};

#endif
