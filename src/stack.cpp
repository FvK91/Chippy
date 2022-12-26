#include "stack.h"

namespace chip8 {
    void stack::Push(const u_int16_t addr) {
        if (SP_ == stack_.size()) {
            return; // Stack overflow..
        }
        stack_[SP_++] = addr;
    }

    u_int16_t stack::Pop() {
        if (SP_ == 0) {
            return 0;
        }
        return stack_[--SP_];
    }
} // chip8