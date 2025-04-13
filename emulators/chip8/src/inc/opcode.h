#ifndef OPCODE_H
#define OPCODE_H

#include <array>
#include "display.h"

// Macros
#define GET_OPCODE(highByte, lowByte)   ((highByte << 8) | lowByte)

#define OPCODE_COUNT        35

// Forward declaration used since I saw a cyclic reference in chip8.cpp
class Chip8;

extern Display display;

// Could've just used a switch statement but I like the cleaner/modular format
// So tightly coupled to Chip8 class I wonder if it should be incorporated?
class Opcodes {
    public:
        void executeOpcode(unsigned short opcode, Chip8& chip8);

    private:
        typedef void(*OpcodeHandler)(unsigned short, Chip8&);

        typedef struct {
            unsigned short mask;
            unsigned short opcode;
            OpcodeHandler opcodeHandler;
        }OpcodeMapping;

        // Opcode handlers (there's a lot)
        // TODO: Should this functionality be moved to the implementation file outside of a class?
        static void opCallMchnCode(unsigned short opcode, Chip8& chip8);
        static void opClearScreen(unsigned short opcode, Chip8&);
        static void OpReturnFromSub(unsigned short opcode, Chip8&);
        static void opJumpAddr(unsigned short opcode, Chip8&);
        static void opCallSub(unsigned short opcode, Chip8&);
        static void opSEvxByte(unsigned short opcode, Chip8&);
        static void opSkip(unsigned short opcode, Chip8&);
        static void opLoadVx(unsigned short opcode, Chip8& chip8);
        static void opAddVx(unsigned short opcode, Chip8& chip8);
        static void opLoadI(unsigned short opcode, Chip8& chip8);
        static void opDrawSprite(unsigned short opcode, Chip8& chip8);

        // Constant at compile time since it won't change
        static constexpr std::array<OpcodeMapping, 8> opcodeLookup {{
            {0xF000, 0x0000, &opCallMchnCode},
            {0xF00F, 0x00E0, &opClearScreen},
            {0xF00F, 0x00EE, &OpReturnFromSub},
            {0xF000, 0x1000, &opJumpAddr},
            {0xF000, 0x6000, &opLoadVx},
            {0xF000, 0x7000, &opAddVx},
            {0xF000, 0xA000, &opLoadI},
            {0xF000, 0xD000, &opDrawSprite},
        }};

};

#endif
