#include "chip8.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

namespace chip8 {
    Instruction::Instruction(u_int8_t first_byte, u_int8_t second_byte) :
            first_byte_(first_byte),
            second_byte_(second_byte) {}

    u_int8_t Instruction::FirstByte() const {
        return first_byte_;
    }

    u_int8_t Instruction::SecondByte() const {
        return second_byte_;
    }

    u_int8_t Instruction::Nibble1() const {
        return (first_byte_ >> 4) & 0xF;
    }

    u_int8_t Instruction::Nibble2() const {
        return first_byte_ & 0xF;
    }

    u_int8_t Instruction::Nibble3() const {
        return (second_byte_ >> 4) & 0xF;
    }

    u_int8_t Instruction::Nibble4() const {
        return second_byte_ & 0xF;
    }

    u_int16_t Instruction::Nibble234() const {
        return Nibble2() << 8 | Nibble3() << 4 | Nibble4();
    }

    u_int16_t Instruction::operator()() const {
        return first_byte_ << 8 | second_byte_;
    }

    using namespace std::chrono;

    const Display &Interpreter::GetDisp() const {
        return display_;
    }

    // Run interpreter a certain ips (instructions per second)
    int Interpreter::Run(const std::filesystem::path &path, const u_int16_t ips) {
        // Load ROM-file into memory
        if (LoadROM(path) != 0) {
            return 1;
        }

        // Set PC
        PC_ = 0x200;

        const auto delay = 1000ms / ips;

        auto next = std::chrono::steady_clock::now();
        auto prev = next - delay;

        bool quit = false;
        SDL_Event e;
        while (!quit) {
            auto now = std::chrono::steady_clock::now();
            //std::cout << ((now-prev) / 1ms) << '\n';
            prev = now;

            // Poll for SDL events
            while (SDL_PollEvent(&e) != 0) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                }
            }

            // Fetch
            const auto i = FetchInstruction();

            // We increment PC_ here already: next instruction
            PC_ += 2;

            std::cout << "Handling instruction: " << "0x" << std::hex << i() << '\n';
            std::cout << "Nibble1 " << std::to_string(i.Nibble1()) << '\n';

            // Execute instruction
            ExecuteInstruction(i);

            // Render display
            display_.Render();

            // Sleep until next
            next += delay;
            std::this_thread::sleep_until(next);
        }

        return 0;
    }


    int Interpreter::LoadROM(const std::filesystem::path &path) {
        std::ifstream rom(path, std::ios::in);

        if (!rom) {
            return 1;
        }

        auto addr = 0x200;
        while (!rom.eof()) {
            RAM_[addr++] = rom.get();
        }
        rom.close();

        return 0;
    }


    Instruction Interpreter::FetchInstruction() const {
        return {RAM_[PC_], RAM_[PC_ + 1]};
    }


    void Interpreter::ExecuteInstruction(const Instruction i) {
        switch (i()) {
            case 0x00E0: // Clear display
            {
                display_.Clear();
                return;
            }
        }

        switch (i.Nibble1()) {
            case 0x1: // Jump
            {
                PC_ = i.Nibble234();
                return;
            }
            case 0x6: // Set VX
            {
                registers_[i.Nibble2()] = i.SecondByte();
                return;
            }
            case 0x7: // Add to VX
            {
                registers_[i.Nibble2()] += i.SecondByte();
                return;
            }
            case 0xA: // Set I;
            {
                I_ = i.Nibble234();
                return;
            }
            case 0xD: {
                // Wrap when going over the edge of screen
                const auto X = registers_[i.Nibble2()] % PIXELS_X;
                auto y = registers_[i.Nibble3()] % PIXELS_Y;
                VF_ = 0;

                for (auto n = 0; n < i.Nibble4(); ++n) {
                    auto x = X;
                    const auto sprite = RAM_[I_ + n];
                    for (auto bit = 7; bit >= 0; --bit) {
                        if (sprite & (1 << bit)) {
                            // std::cout << "Flip pixel: " << x << " " << y << '\n';
                            if (display_.FlipPixel(x, y)) {
                                VF_ = 1;
                            }
                        }
                        ++x;

                        // End of screen check
                        if (x > PIXELS_X - 1) {
                            break;
                        }
                    }
                    ++y;

                    // End of screen check
                    if (y > PIXELS_Y - 1) {
                        return;
                    }
                }
                return;
            }
        }

        std::cerr << "Unsupported instruction: " << "0x" << std::hex << i() << '\n';

//        // Combine the two into an instruction
//        if (first_byte == 0x00) {
//            switch (second_byte) {
//                case 0xE0: {
//                    display_.Clear();
//                    return;
//                }
////                case 0xEE: {
////                    PC_ = stack_.Pop();
////                    return;
////                }
//            }
//        }
//
//        switch (nibble_1) {
//            case 0x1:
//            {
//                PC_ = (RAM_[PC] & 0xf) << 8 | RAM_[PC + 1]; // Jump
//                break;
//            }
////            case 0x2:
////            {
////                stack_.Push(PC);
////                PC_ = (RAM_[PC] & 0xf) << 8 | RAM_[PC + 1]; // Jump
////                break;
////            }
////            case 0x3:
////            {
////                if (registers_[nibble_2] == second_byte) {
////                    PC_ += 2; // Skip
////                }
////                break;
////            }
////            case 0x4:
////            {
////                if (registers_[nibble_2] != second_byte) {
////                    PC_ += 2; // Skip
////                }
////                break;
////            }
////            case 0x5:
////            {
////                if (registers_[nibble_2] == registers_[nibble_3]) {
////                    PC_ += 2; // Skip
////                }
////                break;
////            }
//            case 0x6:
//            {
//                registers_[nibble_2] = second_byte; // Set
//                break;
//            }
//            case 0x7:
//            {
//                registers_[nibble_2] += second_byte; // Add
//                break;
//            }
////            case 0x9:
////            {
////                if (registers_[nibble_2] != registers_[nibble_3]) {
////                    PC_ += 2; // Skip
////                }
////                break;
////            }
//            case 0xA:
//            {
//                I_ = nibble_2 << 8 | second_byte;
//                break;
//            }
//
//        }
    }
} // chip8