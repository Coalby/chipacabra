#include "opcode.h"
#include "chip8.h"

// Opcode class functionality

// Executes the opcodes
void Opcodes::executeOpcode(unsigned short opcode, Chip8& chip8) {
    // TODO: O(N) lookup -> Not efficient ; Hashmap implementation?
    for(const auto &entry : opcodeLookup) {
        if((opcode & entry.mask) == entry.opcode)
            entry.opcodeHandler(opcode, chip8);
    }

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

// Clears the screen
void Opcodes::opClearScreen(unsigned short opcode, Chip8& chip8) {
    // TODO: index types could lead to incorrect values 
    //       if preprocessor defines change beyond appropriate range
    unsigned char width_index {};
    unsigned char height_index {};

    while(width_index < SCREEN_WIDTH) {
        height_index = 0;
        while(height_index < SCREEN_HEIGHT) {
            chip8.pixels[width_index][height_index] = pixels::BLACK_PIXEL;
            height_index++;
        }
        width_index++;
    }

    return;
}

// Returns from subroutine
void Opcodes::opReturnFromSub(unsigned short opcode, Chip8& chip8) {
    return;
}

// Jumps to address
void Opcodes::opJumpAddr(unsigned short opcode, Chip8& chip8) {
    chip8.setProgramCounter(opcode & 0xFFF);
    return;
}

// Calls subroutine
void Opcodes::opCallSub(unsigned short opcode, Chip8& chip8) {
    return;
}

// Skips next instruction if Vx == (opcode & 0xFF)
void Opcodes::opSEvxByte(unsigned short opcode, Chip8& chip8) {
    if(chip8.getRegisterValue(opcode & 0x0F00) == (opcode && 0xFF))
        chip8.addProgramCounter(2);
    return;
}

// Loads (opcode & 0xFF) into Vx
void Opcodes::opLoadVx(unsigned short opcode, Chip8& chip8) {
    chip8.setRegisterValue(opcode & 0xF00, opcode & 0xFF);
    
    return;
}

// Adds (opcodoe & 0xFF) to Vx
void Opcodes::opAddVx(unsigned short opcode, Chip8& chip8) {
    // Consider removing this char for optimization
    // Decreases readability but will increase speed marginally
    unsigned char registerNumber { static_cast<unsigned char>((opcode & 0xF00) >> 8) };
    chip8.setRegisterValue(registerNumber, chip8.getRegisterValue(registerNumber) + (opcode & 0xFF));
    
    return;
}

// Loads (opcode & 0xFFF) to I
void Opcodes::opLoadI(unsigned short opcode, Chip8& chip8) {
    chip8.setI(opcode & 0xFFF);
    return;
}

// Draws a sprite at (Vx,Vy) that is N pixels tall
// TODO: Optimize this, it sucks lol
void Opcodes::opDrawSprite(unsigned short opcode, Chip8& chip8) {
    unsigned char x { static_cast<unsigned char>(chip8.getRegisterValue(opcode & 0x0F00) % 64) };
    unsigned char y { static_cast<unsigned char>(chip8.getRegisterValue(opcode & 0x00F0) % 64) };
    unsigned char xTmp { x };
    unsigned char spriteHeight { static_cast<unsigned char>(opcode & 0xF) };
    unsigned short spriteAddr { chip8.I };
    unsigned char spriteByte {};

    while(y != SCREEN_HEIGHT && spriteHeight) {
        spriteByte = chip8.memory[spriteAddr];
        x = xTmp;

        // Each sprite is a byte (8 bits) long
        while(x < SCREEN_WIDTH && x < (xTmp + 8)) {
            chip8.pixels[y][x] = (spriteByte & (1 << (x - xTmp)));
            x++;
        }

        spriteAddr++;
        y++;
    }

    return;
}
