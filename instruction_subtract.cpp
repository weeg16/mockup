#include "instruction_subtract.h"
Instruction generateSubtract(const std::string& dest, const std::string& src1, const std::string& src2) {
    return {InstructionType::SUBTRACT, {dest, src1, src2}};
}