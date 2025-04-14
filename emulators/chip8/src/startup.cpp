#include "Config.h"
#include "chip8.h"
#include "display.h"
#include "rom.h"

int main(int argc, char* argv[]) {
    // TODO: Exclude this in embedded platform
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <ROM path>" << std::endl;
        return -1;
    }

    Chip8 chip8interpreter;
    Display chip8display;
    FileRomManager RomManager;

    RomManager.loadRom(argv[1], chip8interpreter);

    // Test Memory Space
    chip8interpreter.printMemory();
    
    while(chip8display.closeDisplayCheck()) {
        chip8interpreter.readNextInstruction();
        chip8interpreter.printDebug();
        chip8display.renderDisplay(chip8interpreter.getPixels());
        SDL_Delay(1000);
    };

    return 0;
}