#include "instruction_print.h"

Instruction generatePrint(const std::string& processName) {
    return {InstructionType::PRINT, {"Hello world from " + processName + "!"}};
}
