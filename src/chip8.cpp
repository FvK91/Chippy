#include "chip8.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

namespace chip8 {
    Instruction::Instruction(u_int8_t first_byte, u_int8_t second_byte) : byte1_(first_byte), byte2_(second_byte) {}

    u_int8_t Instruction::B2() const {
        return byte2_;
    }

    u_int8_t Instruction::N1() const {
        return (byte1_ >> 4) & 0xF;
    }

    u_int8_t Instruction::N2() const {
        return byte1_ & 0xF;
    }

    u_int8_t Instruction::N3() const {
        return (byte2_ >> 4) & 0xF;
    }

    u_int8_t Instruction::N4() const {
        return byte2_ & 0xF;
    }

    u_int16_t Instruction::N234() const {
        return N2() << 8 | N3() << 4 | N4();
    }

    u_int16_t Instruction::operator()() const {
        return byte1_ << 8 | byte2_;
    }

    using namespace std::chrono;

    const Display &Interpreter::GetDisplay() const {
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

        const auto delay_ips = 1000ms / ips;
        constexpr auto delay = 1000ms / 60; // delay timer

        auto next_ips = std::chrono::steady_clock::now();
        auto next = std::chrono::steady_clock::now(); // delay timer

        bool quit = false;
        while (true) {
            auto now = std::chrono::steady_clock::now();

            // Poll for SDL events
            keypad_.Update(quit);
            if (quit) {
                break;
            }

            // Fetch
            const auto i = FetchInstruction();

            // We increment PC_ here already: next instruction
            PC_ += 2;

            // For debugging purposes
            // std::cout << "Handling instruction: " << "0x" << std::hex << i() << '\n';

            // Execute instruction
            ExecuteInstruction(i);

            // Render display
            display_.Render();

            // Update delay timer
            if (now > next) {
                ++delay_timer_;
                ++sound_timer_;
                next += delay;
            }

            // Sleep until next
            next_ips += delay_ips;
            std::this_thread::sleep_until(next_ips);
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
            case 0x00EE: {
                PC_ = stack_.Pop();
                return;
            }
        }

        switch (i.N1()) {
            case 0x0: {
                return;
            }
            case 0x1: // Jump
            {
                PC_ = i.N234();
                return;
            }
            case 0x2: // Push PC to stack + jump
            {
                stack_.Push(PC_);
                PC_ = i.N234();
                return;
            }
            case 0x3: {
                if (V_[i.N2()] == i.B2()) {
                    PC_ += 2; // Skip
                }
                return;
            }
            case 0x4: {
                if (V_[i.N2()] != i.B2()) {
                    PC_ += 2; // Skip
                }
                return;
            }
            case 0x5: {
                if (V_[i.N2()] == V_[i.N3()]) {
                    PC_ += 2; // Skip
                }
                return;
            }
            case 0x6: // Set VX
            {
                V_[i.N2()] = i.B2();
                return;
            }
            case 0x7: // Add to VX
            {
                V_[i.N2()] += i.B2();
                return;
            }
            case 0x8: {
                switch (i.N4()) {
                    case 0x0: // Set
                    {
                        V_[i.N2()] = V_[i.N3()];
                        return;
                    }
                    case 0x1: // Binary OR
                    {
                        V_[i.N2()] |= V_[i.N3()];
                        return;
                    }
                    case 0x2: // Binary AND
                    {
                        V_[i.N2()] &= V_[i.N3()];
                        return;
                    }
                    case 0x3: // Logical XOR
                    {
                        V_[i.N2()] ^= V_[i.N3()];
                        return;
                    }
                    case 0x4: // Add
                    {
                        V_[i.N2()] += V_[i.N3()];
                        return;
                    }
                    case 0x5: // Subtract 1
                    {
                        if (V_[i.N2()] > V_[i.N3()]) {
                            V_[0xF] = 1;
                        } else {
                            V_[0xF] = 0;
                        }
                        V_[i.N2()] -= V_[i.N3()];
                        return;
                    }
                    case 0x6: {
                        if (config_.shift_set_VY_) {
                            V_[i.N2()] = V_[i.N3()];
                        }
                        V_[0xF] = (1 & V_[i.N2()]) ? 1 : 0;
                        V_[i.N2()] = V_[i.N2()] >> 1;
                        return;
                    }
                    case 0x7: // Subtract 2
                    {
                        if (V_[i.N3()] > V_[i.N2()]) {
                            V_[0xF] = 1;
                        } else {
                            V_[0xF] = 0;
                        }
                        V_[i.N2()] = V_[i.N3()] - V_[i.N2()];
                        return;
                    }
                    case 0xE: {
                        if (config_.shift_set_VY_) {
                            V_[i.N2()] = V_[i.N3()];
                        }
                        V_[0xF] = (0b10000000 & V_[i.N2()]) ? 1 : 0;
                        V_[i.N2()] = V_[i.N2()] << 1;
                        return;
                    }
                }
                break;
            }
            case 0x9: {
                if (V_[i.N2()] != V_[i.N3()]) {
                    PC_ += 2; // Skip
                }
                return;
            }
            case 0xA: // Set I;
            {
                I_ = i.N234();
                return;
            }
            case 0xB: {
                PC_ = i.N234() + V_[0];
                return;
            }
            case 0xC: {
                V_[i.N2()] = (rand() % 256) & i.B2();
                return;
            }
            case 0xD: {
                // Wrap when going over the edge of screen
                const auto X = V_[i.N2()] % PIXELS_X;
                auto y = V_[i.N3()] % PIXELS_Y;
                V_[0xF] = 0;

                for (auto n = 0; n < i.N4(); ++n) {
                    auto x = X;
                    const auto sprite = RAM_[I_ + n];
                    for (auto bit = 7; bit >= 0; --bit) {
                        if (sprite & (1 << bit)) {
                            if (display_.FlipPixel(x, y)) {
                                V_[0xF] = 1;
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
            case 0xE: {
                switch (i.B2()) {
                    case 0x9E: {
                        if (keypad_.KeyDown(V_[i.N2()])) {
                            PC_ += 2; // Skip
                        }
                        return;
                    }
                    case 0xA1: {
                        if (!keypad_.KeyDown(V_[i.N2()])) {
                            PC_ += 2; // Skip
                        }
                        return;
                    }
                }
                return;
            }
            case 0xF: {
                switch (i.B2()) {
                    case 0x07: {
                        V_[i.N2()] = delay_timer_;
                        return;
                    }
                    case 0x15: {
                        delay_timer_ = V_[i.N2()];
                        return;
                    }
                    case 0x18: {
                        sound_timer_ = V_[i.N2()];
                        return;
                    }
                    case 0x1E: {
                        I_ += V_[i.N2()];
                        return;
                    }
                    case 0x0A: {
                        const auto key = keypad_.KeyPressed();
                        if (!keypad_.KeyPressed(key)) {
                            PC_ -= 2; // Loop
                        } else {
                            V_[i.N2()] = key;
                        }
                        return;
                    }
                    case 0x29: {
                        I_ = font_address + V_[i.N2()] * 5;
                        return;
                    }
                    case 0x33: {
                        RAM_[I_] = V_[i.N2()] / 100 % 10;
                        RAM_[I_ + 1] = V_[i.N2()] / 10 % 10;
                        RAM_[I_ + 2] = V_[i.N2()] % 10;
                        return;
                    }
                    case 0x55: {
                        for (auto n = 0; n <= i.N2() && n != sizeof(V_); ++n) {
                            RAM_[I_ + n] = V_[n];
                        }
                        if (config_.fx55_incr_I_) {
                            I_ += i.N2() + 1;
                        }
                        return;
                    }
                    case 0x65: {
                        for (auto n = 0; n <= i.N2() && n != sizeof(V_); ++n) {
                            V_[n] = RAM_[I_ + n];
                        }
                        return;
                    }
                }
            }
        }

        std::cerr << "Unsupported instruction: " << "0x" << std::hex << i() << '\n';
    }
} // chip8