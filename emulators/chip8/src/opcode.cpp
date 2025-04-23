#include "opcode.h"
#include "chip8.h"

#define GET_VX_FROM_OP(opcode)      ((opcode & 0xF00) >> 8)
#define GET_VY_FROM_OP(opcode)      ((opcode & 0x00F0) >> 4)

// Opcode class functionality

// Executes the opcodes
void Opcodes::executeOpcode(unsigned short opcode, Chip8& chip8) {
    // TODO: O(N) lookup -> Not efficient ; Hashmap implementation?
    for(const auto &entry : opcodeLookup) {
        if((opcode & entry.mask) == entry.opcode)
        {
            entry.opcodeHandler(opcode, chip8);
            // TODO: Find better way to check edge cases. This looks ugly lol. 
            //       Also sucks for readability
            if(entry.opcode != OP_JUMP_ADDR_MASK && entry.opcode != OP_CALL_SUB_MASK
                && entry.opcode != OP_RETURN_FROM_SUB_MASK)
                chip8.addProgramCounter(2);

            break;
        }
    }

    return;
}

// Clears the screen
void Opcodes::opClearScreen(unsigned short opcode, Chip8& chip8) {

    for (size_t height_index = 0; height_index < pixels::DISPLAY_HEIGHT; height_index++) {
        for (size_t width_index = 0; width_index < pixels::DISPLAY_WIDTH; width_index++) {
            chip8.pixels[height_index][width_index] = pixels::BLACK_PIXEL;
        }
    }

    return;
}

// Returns from subroutine
void Opcodes::opReturnFromSub(unsigned short opcode, Chip8& chip8) {
    chip8.setProgramCounter(chip8.stack[chip8.SP-1]);
    chip8.SP--;

    return;
}

// This opcode is kinda wack. Not sure how this would affect stack usage
void Opcodes::opCallMchnCode(unsigned short opcode, Chip8& chip8) {
    unsigned short addr {};
    unsigned short nextOpcode {};
    addr = (opcode & 0xFFF);
    nextOpcode = chip8.getMachineCode(addr);

    // TODO: Fix this being called as a public function
    //       Could result in corrupt data?
    executeOpcode(nextOpcode, chip8);

    return;
}

// Jumps to address
void Opcodes::opJumpAddr(unsigned short opcode, Chip8& chip8) {
    chip8.setProgramCounter(opcode & 0xFFF);

    return;
}

// Calls subroutine
void Opcodes::opCallSub(unsigned short opcode, Chip8& chip8) {
    chip8.stack[chip8.SP] = chip8.PC + 2;
    chip8.SP++;
    chip8.setProgramCounter(opcode & 0xFFF);

    return;
}

// Skips next instruction if Vx == NN
void Opcodes::opSEVx(unsigned short opcode, Chip8& chip8) {
    if(chip8.getRegisterValue((opcode & 0x0F00) >> 8) == (opcode & 0xFF))
        chip8.addProgramCounter(2);
    
    return;
}

// Skips next instruction if Vx != NN
void Opcodes::opSNEVx(unsigned short opcode, Chip8& chip8) {
    if(chip8.getRegisterValue((opcode & 0x0F00) >> 8) != (opcode & 0xFF))
        chip8.addProgramCounter(2);

    return;
}

// Skips next instruction if Vx == Vy
void Opcodes::opSEVxVy(unsigned short opcode, Chip8& chip8) {
    if(chip8.getRegisterValue((opcode & 0x0F00) >> 8) == chip8.getRegisterValue(GET_VY_FROM_OP(opcode)))
        chip8.addProgramCounter(2);

    return;
}

// Loads NN into Vx
void Opcodes::opLoadVx(unsigned short opcode, Chip8& chip8) {
    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), opcode & 0xFF);
    
    return;
}

// Adds (opcodoe & 0xFF) to Vx
void Opcodes::opAddVx(unsigned short opcode, Chip8& chip8) {
    // Consider removing this char for optimization
    // Decreases readability but will marginally increase speed
    unsigned char registerNumber { static_cast<unsigned char>(GET_VX_FROM_OP(opcode)) };
    chip8.setRegisterValue(registerNumber, chip8.getRegisterValue(registerNumber) + (opcode & 0xFF));
    
    return;
}

// Loads Vx to Vy
void Opcodes::opLoadVxVy(unsigned short opcode, Chip8& chip8) {
    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), chip8.getRegisterValue(GET_VY_FROM_OP(opcode)));
    
    return;
}

// Loads Vx to Vx |= Vy
void Opcodes::opLoadORVxVy(unsigned short opcode, Chip8& chip8) {
    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), 
                            chip8.getRegisterValue(GET_VX_FROM_OP(opcode)) | chip8.getRegisterValue(GET_VY_FROM_OP(opcode)));
    
    return;
}

// Loads Vx to Vx &= Vy
void Opcodes::opLoadANDVxVy(unsigned short opcode, Chip8& chip8) {
    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), 
                            chip8.getRegisterValue(GET_VX_FROM_OP(opcode)) & chip8.getRegisterValue(GET_VY_FROM_OP(opcode)));

    return;
}

// Loads Vx to Vx ^= Vy
void Opcodes::opLoadXORVxVy(unsigned short opcode, Chip8&chip8) {
    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), 
                            chip8.getRegisterValue(GET_VX_FROM_OP(opcode)) ^ chip8.getRegisterValue(GET_VY_FROM_OP(opcode)));
    return;
}

// Loads Vx to Vx += Vy. Vf = 1 if an overflow is detected, 0 otherwise
// TODO: Ignore addition when overflow? or not?
void Opcodes::opLoadADDVxVy(unsigned short opcode, Chip8& chip8) {
    // Not the most memory efficient if using continually on embedded system
    unsigned char vxValue = chip8.getRegisterValue(GET_VX_FROM_OP(opcode));
    unsigned char vyValue = chip8.getRegisterValue(GET_VY_FROM_OP(opcode));
    int registerValue = static_cast<int>(vxValue) + static_cast<int>(vyValue);

    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), registerValue);

    (vyValue > vxValue) ? chip8.setOverflowRegister(OVERFLOW_OCCURED) : chip8.setOverflowRegister(OVERFLOW_DID_NOT_OCCUR);
    
    return;
}

// Loads Vx to Vx -= Vy. Vf = 0 if an underflow is detected, 1 otherwise
void Opcodes::opLoadSUBVxVy(unsigned short opcode, Chip8& chip8) {
    // Not the most memory efficient if using continually on embedded system
    unsigned char vxValue = chip8.getRegisterValue(GET_VX_FROM_OP(opcode));
    unsigned char vyValue = chip8.getRegisterValue(GET_VY_FROM_OP(opcode));
    int registerValue = static_cast<int>(vxValue) - static_cast<int>(vyValue);

    // Kind of weird how negative values are interpreted as unsigned chars in Chip 8
    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), static_cast<unsigned char>(registerValue));

    (vxValue < vyValue) ? chip8.setOverflowRegister(UNDERFLOW_OCCURED) : chip8.setOverflowRegister(UNDERFLOW_DID_NOT_OCCUR);

    return;
}

// Shifts Vx right by one. Stores the least significant bit prior to shift in Vf
void Opcodes::opLoadShiftRightVx(unsigned short opcode, Chip8& chip8) {
    unsigned char registerValue = chip8.getRegisterValue(GET_VX_FROM_OP(opcode));

    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), registerValue >> 1);
    chip8.setOverflowRegister(registerValue & 0x1); // Store least significant bit

    return;
}

// Loads Vx to Vy - Vx. Vf = 0 if an underflow is detected, 1 otherwise
void Opcodes::opLoadSUBVyVx(unsigned short opcode, Chip8& chip8) {
    // Not the most memory efficient if using continually on embedded system
    unsigned char vxValue = chip8.getRegisterValue(GET_VX_FROM_OP(opcode));
    unsigned char vyValue = chip8.getRegisterValue(GET_VY_FROM_OP(opcode));
    char registerValue = vyValue - vxValue;

    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), registerValue);

    (vyValue < vxValue) ? chip8.setOverflowRegister(UNDERFLOW_OCCURED) : chip8.setOverflowRegister(UNDERFLOW_DID_NOT_OCCUR);

    return;
}

// Shifts Vx left by one. Stores the most significant bit prior to shift in Vf
void Opcodes::opLoadShiftLeftVx(unsigned short opcode, Chip8& chip8) {
    unsigned char registerValue = chip8.getRegisterValue(GET_VX_FROM_OP(opcode));

    chip8.setRegisterValue(GET_VX_FROM_OP(opcode), static_cast<char>(registerValue << 1));
    chip8.setOverflowRegister((registerValue & 0x80) >> 7); // Store most significant bit
    
    return;
}

// Skips next instruction if Vx != Vy
void Opcodes::opSNEVxVy(unsigned short opcode, Chip8& chip8) {
    if(chip8.getRegisterValue(GET_VX_FROM_OP(opcode)) != chip8.getRegisterValue(GET_VY_FROM_OP(opcode)))
        chip8.addProgramCounter(2);

    return;
}

// Loads (opcode & 0xFFF) to I
void Opcodes::opLoadI(unsigned short opcode, Chip8& chip8) {
    chip8.setI(opcode & 0xFFF);
    return;
}

void Opcodes::opJumpAddrV0(unsigned short opcode, Chip8& chip8) {
    return;
}

void Opcodes::opLoadVxRand(unsigned short opcode, Chip8& chip8) {
    return;
}

// Draws a sprite at (Vx,Vy) that is N pixels tall
// TODO: Optimize this, it sucks lol
void Opcodes::opDrawSprite(unsigned short opcode, Chip8& chip8) {
    // Extract X and Y coordinates from registers
    unsigned char x { static_cast<unsigned char>(chip8.getRegisterValue((opcode & 0x0F00) >> 8) % pixels::DISPLAY_WIDTH) };
    unsigned char y { static_cast<unsigned char>(chip8.getRegisterValue((opcode & 0x00F0) >> 4) % pixels::DISPLAY_HEIGHT) };
    unsigned char spriteHeight { static_cast<unsigned char>(opcode & 0xF) };
    unsigned short spriteAddr { chip8.I };
    
    // Reset collision flag (VF)
    chip8.setRegisterValue(0xF, 0);
    
    for (unsigned char yOffset = 0; yOffset < spriteHeight; yOffset++) {
        if (y + yOffset >= pixels::DISPLAY_HEIGHT) break; // Don't draw past screen
        
        unsigned char spriteByte = chip8.memory[spriteAddr];
        
        // Draw a byte at a time
        for (unsigned char xOffset = 0; xOffset < 8; xOffset++) {
            if (x + xOffset >= pixels::DISPLAY_WIDTH) break; // Don't draw past right edge
            
            unsigned int& screenPixel = chip8.pixels[y + yOffset][x + xOffset];

            bool pixelSet = (spriteByte & (0x80 >> xOffset));

            // Only mess with pixels that are 1 in the sprite
            if(pixelSet)
            {
                // If pixel was set, then collision has occured (VF = 1)
                if(screenPixel == pixels::WHITE_PIXEL)
                    chip8.setRegisterValue(0xF, 1);
                
                // XOR with the screen
                // TODO: Find a way to utilize bitwise ^= instead
                screenPixel = (screenPixel == pixels::WHITE_PIXEL) ? pixels::BLACK_PIXEL : pixels::WHITE_PIXEL;
            }
        }

        spriteAddr++;
    }
}

void Opcodes::opSEKey(unsigned short opcode, Chip8& chip8) {
    return;
}

void Opcodes::opSNEKey(unsigned short opcode, Chip8& chip8) {
    return;
}

void Opcodes::opLoadVxDelay(unsigned short opcode, Chip8& chip8) {
    return;
}

void Opcodes::opLoadVxKey(unsigned short opcode, Chip8& chip8) {
    return;
}

void Opcodes::opLoadDelayToVx(unsigned short opcode, Chip8& chip8) {
    return;
}

void Opcodes::opLoadSoundToVx(unsigned short opcode, Chip8& chip8) {
    return;
}

void Opcodes::opLoadIVx(unsigned short opcode, Chip8& chip8) {
    return;
}

void Opcodes::opLoadISpriteAddr(unsigned short opcode, Chip8& chip8) {
    return;
}

void Opcodes::opLoadBCDVx(unsigned short opcode, Chip8& chip8) {
    return;
}

void Opcodes::opStoreAllRegisterValues(unsigned short opcode, Chip8& chip8) {
    return;
}

// Load all registers with memory starting at I
void Opcodes::opLoadAllRegisterValues(unsigned short opcode, Chip8& chip8) {
    unsigned char maxRegister = (opcode & 0x0F00) >> 8;

    for (unsigned char index = 0; index <= maxRegister; ++index) {
        chip8.setRegisterValue(index, chip8.memory[chip8.I + index]);
    }

    return;
}
