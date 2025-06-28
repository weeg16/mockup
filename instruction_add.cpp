#include "instruction_add.h"
Instruction generateAdd(const std::string& dest, const std::string& src1, const std::string& src2) {
    return {InstructionType::ADD, {dest, src1, src2}};
}