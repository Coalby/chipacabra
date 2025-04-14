#ifndef ROM_H

#define ROM_H
#include <sstream>
#include "chip8.h"

// Is an abstact class the best way to implement embedded/desktop functionality?
class RomManager {
    public:
        RomManager() {};
        ~RomManager() {};
        virtual char loadRom(std::string_view filename, Chip8& chip8) = 0; // Reads entire ROM into memory
};  

// Embedded/Desktop functionality classes
class FileRomManager : RomManager {
    public:
        FileRomManager() {};
        ~FileRomManager() {};
        char loadRom(std::string_view filename, Chip8& chip8);
};

#endif
