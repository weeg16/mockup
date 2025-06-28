#include "instruction_sleep.h"
Instruction generateSleep(int ticks) {
    return {InstructionType::SLEEP, {std::to_string(ticks)}};
}