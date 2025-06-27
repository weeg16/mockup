#include "instruction_declare.h"
Instruction generateDeclare(const std::string& varName, int value) {
    return {InstructionType::DECLARE, {varName, std::to_string(value)}};
}
