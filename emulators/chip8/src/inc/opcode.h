#ifndef OPCODE_H
#define OPCODE_H

#include <array>
#include "display.h"

// Macros
#define GET_OPCODE(highByte, lowByte)   ((highByte << 8) | lowByte)

#define OPCODE_COUNT        35

// Are defines more efficient for calling in embedded systems?
// Opcode Masks
#define OP_CLEAR_SCREEN_MASK                0x00E0
#define OP_RETURN_FROM_SUB_MASK             0x00EE
#define OP_CALL_MCHN_CODE_MASK              0x0000
#define OP_JUMP_ADDR_MASK                   0x1000
#define OP_CALL_SUB_MASK                    0x2000
#define OP_SE_VX_MASK                       0x3000
#define OP_SNE_VX_MASK                      0x4000
#define OP_SE_VX_VY_MASK                    0x5000
#define OP_LOAD_VX_MASK                     0x6000
#define OP_ADD_VX_MASK                      0x7000
#define OP_LOAD_VX_VY_MASK                  0x8000
#define OP_LOAD_OR_VX_VY_MASK               0x8001
#define OP_LOAD_AND_VX_VY_MASK              0x8002
#define OP_LOAD_XOR_VX_VY_MASK              0x8003
#define OP_LOAD_ADD_VX_VY_MASK              0x8004
#define OP_LOAD_SUB_VX_VY_MASK              0x8005
#define OP_LOAD_SHIFT_RIGHT_VX_MASK         0x8006
#define OP_LOAD_SUB_VY_VX_MASK              0x8007
#define OP_LOAD_SHIFT_LEFT_VX_MASK          0x800E
#define OP_SNE_VX_VY_MASK                   0x9000
#define OP_LOAD_I_MASK                      0xA000
#define OP_JUMP_ADDR_V0_MASK                0xB000
#define OP_LOAD_VX_RAND_MASK                0xC000
#define OP_DRAW_SPRITE_MASK                 0xD000
#define OP_SE_KEY_MASK                      0xE09E
#define OP_SNE_KEY_MASK                     0xE0A1
#define OP_LOAD_VX_DELAY_MASK               0xF007
#define OP_LOAD_VX_KEY_MASK                 0xF00A
#define OP_LOAD_DELAY_TO_VX_MASK            0xF015
#define OP_LOAD_SOUND_TO_VX_MASK            0xF018
#define OP_LOAD_I_VX_MASK                   0xF01E
#define OP_LOAD_I_SPRITE_ADDR_MASK          0xF029
#define OP_BCD_VX_MASK                      0xF033
#define OP_STORE_ALL_REGISTER_VALUES_MASK   0xF055
#define OP_LOAD_ALL_REGISTER_VALUES_MASK    0xF065

// Forward declaration used since I saw a cyclic reference in chip8.cpp
class Chip8;

// Is extern actually good code design if its a singleton?
extern Display display;

// Could've just used a switch statement but I like the cleaner/modular format
// So tightly coupled to Chip8 class I wonder if it should be incorporated?
class Opcodes {
    public:
        static void executeOpcode(unsigned short opcode, Chip8& chip8);

    private:
        typedef void(*OpcodeHandler)(unsigned short, Chip8&);

        typedef struct {
            unsigned short mask;
            unsigned short opcode;
            OpcodeHandler opcodeHandler;
        }OpcodeMapping;

        // Opcode handlers (there's a lot)
        // TODO: Should this functionality be moved to the implementation file outside of a class?
        static void opClearScreen(unsigned short opcode, Chip8& chip8);
        static void opReturnFromSub(unsigned short opcode, Chip8& chip8);
        static void opCallMchnCode(unsigned short opcode, Chip8& chip8);
        static void opJumpAddr(unsigned short opcode, Chip8& chip8);
        static void opCallSub(unsigned short opcode, Chip8& chip8);
        static void opSEVx(unsigned short opcode, Chip8& chip8);
        static void opSNEVx(unsigned short opcode, Chip8& chip8);
        static void opSEVxVy(unsigned short opcode, Chip8& chip8);
        static void opLoadVx(unsigned short opcode, Chip8& chip8);
        static void opAddVx(unsigned short opcode, Chip8& chip8);
        static void opLoadVxVy(unsigned short opcode, Chip8& chip8);
        static void opLoadORVxVy(unsigned short opcode, Chip8& chip8);
        static void opLoadANDVxVy(unsigned short opcode, Chip8& chip8);
        static void opLoadXORVxVy(unsigned short opcode, Chip8& chip8);
        static void opLoadADDVxVy(unsigned short opcode, Chip8& chip8);
        static void opLoadSUBVxVy(unsigned short opcode, Chip8& chip8);
        static void opLoadShiftRightVx(unsigned short opcode, Chip8& chip8);
        static void opLoadSUBVyVx(unsigned short opcode, Chip8& chip8);
        static void opLoadShiftLeftVx(unsigned short opcode, Chip8& chip8);
        static void opSNEVxVy(unsigned short opcode, Chip8& chip8);
        static void opLoadI(unsigned short opcode, Chip8& chip8);
        static void opJumpAddrV0(unsigned short opcode, Chip8& chip8);
        static void opLoadVxRand(unsigned short opcode, Chip8& chip8);
        static void opDrawSprite(unsigned short opcode, Chip8& chip8);
        static void opSEKey(unsigned short opcode, Chip8& chip8);
        static void opSNEKey(unsigned short opcode, Chip8& chip8);
        static void opLoadVxDelay(unsigned short opcode, Chip8& chip8);
        static void opLoadVxKey(unsigned short opcode, Chip8& chip8);
        static void opLoadDelayToVx(unsigned short opcode, Chip8& chip8);
        static void opLoadSoundToVx(unsigned short opcode, Chip8& chip8);
        static void opLoadIVx(unsigned short opcode, Chip8& chip8);
        static void opLoadISpriteAddr(unsigned short opcode, Chip8& chip8);
        static void opLoadBCDVx(unsigned short opcode, Chip8& chip8);
        static void opStoreAllRegisterValues(unsigned short opcode, Chip8& chip8);
        static void opLoadAllRegisterValues(unsigned short opcode, Chip8& chip8);

        // Constant at compile time since it won't change
        constexpr static std::array<OpcodeMapping, 34> opcodeLookup {{
            {0xFFFF, OP_CLEAR_SCREEN_MASK, &Opcodes::opClearScreen},
            {0xFFFF, OP_RETURN_FROM_SUB_MASK, &Opcodes::opReturnFromSub},
            //{0xF000, OP_CALL_MCHN_CODE_MASK, &Opcodes::opCallMchnCode},
            {0xF000, OP_JUMP_ADDR_MASK, &Opcodes::opJumpAddr},
            {0xF000, OP_CALL_SUB_MASK, &Opcodes::opCallSub},
            {0xF000, OP_SE_VX_MASK, &Opcodes::opSEVx},
            {0xF000, OP_SNE_VX_MASK, &Opcodes::opSNEVx},
            {0xF000, OP_SE_VX_VY_MASK, &Opcodes::opSEVxVy},
            {0xF000, OP_LOAD_VX_MASK, &Opcodes::opLoadVx},
            {0xF000, OP_ADD_VX_MASK, &Opcodes::opAddVx},
            {0xF00F, OP_LOAD_VX_VY_MASK, &Opcodes::opLoadVxVy},
            {0xF00F, OP_LOAD_OR_VX_VY_MASK, &Opcodes::opLoadORVxVy},
            {0xF00F, OP_LOAD_AND_VX_VY_MASK, &Opcodes::opLoadANDVxVy},
            {0xF00F, OP_LOAD_XOR_VX_VY_MASK, &Opcodes::opLoadXORVxVy},
            {0xF00F, OP_LOAD_ADD_VX_VY_MASK, &Opcodes::opLoadADDVxVy},
            {0xF00F, OP_LOAD_SUB_VX_VY_MASK, &Opcodes::opLoadSUBVxVy},
            {0xF00F, OP_LOAD_SHIFT_RIGHT_VX_MASK, &Opcodes::opLoadShiftRightVx},
            {0xF00F, OP_LOAD_SUB_VY_VX_MASK, &Opcodes::opLoadSUBVyVx},
            {0xF00F, OP_LOAD_SHIFT_LEFT_VX_MASK, &Opcodes::opLoadShiftLeftVx},
            {0xF000, OP_SNE_VX_VY_MASK, &Opcodes::opSNEVxVy},
            {0xF000, OP_LOAD_I_MASK, &Opcodes::opLoadI},
            {0xF000, OP_JUMP_ADDR_V0_MASK, &Opcodes::opJumpAddrV0},
            {0xF000, OP_LOAD_VX_RAND_MASK, &Opcodes::opLoadVxRand},
            {0XF000, OP_DRAW_SPRITE_MASK, &Opcodes::opDrawSprite},
            {0xF0FF, OP_SE_KEY_MASK, &Opcodes::opSEKey},
            {0xF0FF, OP_SNE_KEY_MASK, &Opcodes::opSNEKey},
            {0xF00F, OP_LOAD_VX_DELAY_MASK, &Opcodes::opLoadVxDelay},
            {0xF00F, OP_LOAD_VX_KEY_MASK, &Opcodes::opLoadVxKey},
            {0xF0FF, OP_LOAD_DELAY_TO_VX_MASK, &Opcodes::opLoadDelayToVx},
            {0xF0FF, OP_LOAD_SOUND_TO_VX_MASK, &Opcodes::opLoadSoundToVx},
            {0xF0FF, OP_LOAD_I_VX_MASK, &Opcodes::opLoadIVx},
            {0xF0FF, OP_LOAD_I_SPRITE_ADDR_MASK, &Opcodes::opLoadISpriteAddr},
            {0xF0FF, OP_BCD_VX_MASK, &Opcodes::opLoadBCDVx},
            {0xF0FF, OP_STORE_ALL_REGISTER_VALUES_MASK, &Opcodes::opStoreAllRegisterValues},
            {0xF0FF, OP_LOAD_ALL_REGISTER_VALUES_MASK, &Opcodes::opLoadAllRegisterValues}
        }};

};

#endif
