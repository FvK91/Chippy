#include "chip8.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

namespace chip8 {
    using namespace std::chrono;

    const Display &Interpreter::GetDisp() const {
        return display_;
    }

    // Run interpreter a certain ips (instructions per second)
    int Interpreter::Run(const std::filesystem::path& path, const u_int16_t ips) {
        // Load ROM-file into memory
        if (LoadROM(path) != 0) {
            return 1;
        }

        const auto delay = 1000ms / ips;

        auto next = std::chrono::steady_clock::now();
        auto prev = next - delay;

        bool quit = false;
        SDL_Event e;
        while(!quit) {
            auto now = std::chrono::steady_clock::now();
            // std::cout << ((now-prev) / 1ms) << '\n';
            prev = now;

            // Poll for SDL events
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                }
            }

            // Main 3 tasks: Fetch -> Decode -> Execute
            ExecuteInstruction();

            // Render display
            display_.Render();

            // Sleep until next
            next += delay;
            std::this_thread::sleep_until(next);
        }

        return 0;
    }


    int Interpreter::LoadROM(const std::filesystem::path& path) {
        std::fstream rom;
        rom.open(path, std::ios::in);

        if(!rom) {
            return 1;
        }

        auto mem_start = 0x200;
        while(!rom.eof()) {
            rom >> RAM_[mem_start++];
        }

        // Set PC
        PC_ = 0x200;

        rom.close();
        return 0;
    }


    void Interpreter::ExecuteInstruction() {
        // Fetch the instruction from memory at the current PC (program counter)
        // Read two successive bytes from memory and combine them into one 16-bit instruction
        const auto nibble_1 = RAM_[PC_] & 0xf;
        const auto nibble_2 = (RAM_[PC_] >> 4) & 0xf;
        const auto nibble_3 = RAM_[PC_ + 1] & 0xf;
        const auto nibble_4 = (RAM_[PC_ + 1] >> 4) & 0xf;

        const auto first_byte = RAM_[PC_];
        const auto second_byte = RAM_[PC_ + 1];

        // Increment with 2 bytes
        PC_ += 2;

        // Combine the two into an instruction
        if (first_byte == 0x00) {
            switch (second_byte) {
                case 0xE0: {
                    display_.Clear();
                    return;
                }
                case 0xEE: {
                    PC_ = stack_.Pop();
                    return;
                }
            }
        }

        switch (nibble_1) {
            case 0x1:
            {
                PC_ = RAM_[PC_] >> 4 & 0xfff; // Jump
                break;
            }
            case 0x2:
            {
                stack_.Push(PC_);
                PC_ = RAM_[PC_] >> 4 & 0xfff; // Jump
                break;
            }
            case 0x3:
            {
                if (registers_[nibble_2] == second_byte) {
                    PC_ += 2; // Skip
                }
                break;
            }
            case 0x4:
            {
                if (registers_[nibble_2] != second_byte) {
                    PC_ += 2; // Skip
                }
                break;
            }
            case 0x5:
            {
                if (registers_[nibble_2] == registers_[nibble_3]) {
                    PC_ += 2; // Skip
                }
                break;
            }
            case 0x6:
            {
                registers_[nibble_2] = second_byte; // Set
                break;
            }
            case 0x7:
            {
                registers_[nibble_2] += second_byte; // Add
                break;
            }
            case 0x9:
            {
                if (registers_[nibble_2] != registers_[nibble_3]) {
                    PC_ += 2; // Skip
                }
                break;
            }
            case 0xA:
            {
                I_ = RAM_[PC_] >> 4 & 0xfff;
                break;
            }
        }
    }
} // chip8