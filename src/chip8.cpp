#include "chip8.h"

#include <iostream>
#include <chrono>
#include <thread>

namespace chip8 {
    using namespace std::chrono;

    // Run interpreter a certain ips (instructions per second)
    int Interpreter::Run(const u_int16_t ips) {
        const auto delay = 1000ms / ips;

        auto next = std::chrono::steady_clock::now();
        auto prev = next - delay;

        while(true) {
            auto now = std::chrono::steady_clock::now();
            std::cout << ((now-prev) / 1ms) << '\n';
            prev = now;

            // Main 3 tasks: Fetch -> Decode -> Execute
            Fetch();

            // Sleep until next
            next += delay;
            std::this_thread::sleep_until(next);
        }

        return 0;
    }

    void Interpreter::Fetch() {
        // Fetch the instruction from memory at the current PC (program counter)
        // Read two successive bytes from memory and combine them into one 16-bit instruction
        const auto nibble_1 = PC_ & 0xf;
        const auto nibble_2 = (PC_ >> 4) & 0xf;
        const auto nibble_3 = (PC_ >> 8) & 0xf;
        const auto nibble_4 = (PC_ >> 12) & 0xf;

        const auto low_byte = PC_ & 0xff;
        const auto high_byte = (PC_>> 8) & 0xff;

        // Combine the two into an instruction
        if (low_byte == 0x00 && high_byte == 0xE0) { display_.Clear(); }

        switch (nibble_1) {
            case 0x1:
            {
                PC_ = (PC_ >> 4) & 0xfff; // Jump
            }
            case 0x6:
            {
                registers_[nibble_2] = (PC_ >> 8) & 0xFF; // Set
            }
            case 0x7:
            {
                registers_[nibble_2] += (PC_ >> 8) & 0xFF; // Add
            }
            case 0xA:
            {

            }
            case 0xD:
            {

            }
        }


        // Increment with 2 bytes
        PC_ += 2;
    }
} // chip8