#include "chip8.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

namespace chip8 {
    Instruction::Instruction(u_int8_t first_byte, u_int8_t second_byte) : first_byte_(first_byte),
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

        const auto delay_ips = 1000ms / ips;
        constexpr auto delay = 1000ms / 60; // delay timer

        auto next_ips = std::chrono::steady_clock::now();
        auto next = std::chrono::steady_clock::now(); // delay timer

        bool quit = false;
        while (!quit) {
            auto now = std::chrono::steady_clock::now();

            // Poll for SDL events
            keypad_.Update();

            // Fetch
            const auto i = FetchInstruction();

            // We increment PC_ here already: next instruction
            PC_ += 2;

            std::cout << "Handling instruction: " << "0x" << std::hex << i() << '\n';

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

        switch (i.Nibble1()) {
            case 0x0:
            {
                return;
            }
            case 0x1: // Jump
            {
                PC_ = i.Nibble234();
                return;
            }
            case 0x2: // Push PC to stack + jump
            {
                stack_.Push(PC_);
                PC_ = i.Nibble234();
                return;
            }
            case 0x3: {
                if (registers_[i.Nibble2()] == i.SecondByte()) {
                    PC_ += 2; // Skip
                }
                return;
            }
            case 0x4: {
                if (registers_[i.Nibble2()] != i.SecondByte()) {
                    PC_ += 2; // Skip
                }
                return;
            }
            case 0x5: {
                if (registers_[i.Nibble2()] == registers_[i.Nibble3()]) {
                    PC_ += 2; // Skip
                }
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
            case 0x8: {
                switch (i.Nibble4()) {
                    case 0x0: // Set
                    {
                        registers_[i.Nibble2()] = i.Nibble3();
                        return;
                    }
                    case 0x1: // Binary OR
                    {
                        registers_[i.Nibble2()] |= registers_[i.Nibble3()];
                        return;
                    }
                    case 0x2: // Binary AND
                    {
                        registers_[i.Nibble2()] &= registers_[i.Nibble3()];
                        return;
                    }
                    case 0x3: // Logical XOR
                    {
                        registers_[i.Nibble2()] ^= registers_[i.Nibble3()];
                        return;
                    }
                    case 0x4: // Add
                    {
                        registers_[i.Nibble2()] += registers_[i.Nibble3()];
                        return;
                    }
                    case 0x5: // Subtract 1
                    {
                        if (registers_[i.Nibble2()] > registers_[i.Nibble3()]) {
                            registers_[0xF] = 1;
                        } else {
                            registers_[0xF] = 0;
                        }
                        registers_[i.Nibble2()] -= registers_[i.Nibble3()];
                        return;
                    }
                    case 0x6: {
                        if (config_.shift_set_VY_) {
                            registers_[i.Nibble2()] = registers_[i.Nibble3()];
                        }
                        registers_[0xF] = (1 & registers_[i.Nibble2()]) ? 1 : 0;
                        registers_[i.Nibble2()] = registers_[i.Nibble2()] >> 1;
                        return;
                    }
                    case 0x7: // Subtract 2
                    {
                        if (registers_[i.Nibble3()] > registers_[i.Nibble2()]) {
                            registers_[0xF] = 1;
                        } else {
                            registers_[0xF] = 0;
                        }
                        registers_[i.Nibble2()] = registers_[i.Nibble3()] - registers_[i.Nibble2()];
                        return;
                    }
                    case 0xE: {
                        if (config_.shift_set_VY_) {
                            registers_[i.Nibble2()] = registers_[i.Nibble3()];
                        }
                        registers_[0xF] = (0b10000000 & registers_[i.Nibble2()]) ? 1 : 0;
                        registers_[i.Nibble2()] = registers_[i.Nibble2()] << 1;
                        return;
                    }
                }
                break;
            }
            case 0x9: {
                if (registers_[i.Nibble2()] != registers_[i.Nibble3()]) {
                    PC_ += 2; // Skip
                }
                return;
            }
            case 0xA: // Set I;
            {
                I_ = i.Nibble234();
                return;
            }
            case 0xB: {
                PC_ = i.Nibble234() + registers_[0];
                return;
            }
            case 0xC:
            {
                registers_[i.Nibble2()] = (rand() % 256) & i.SecondByte();
                return;
            }
            case 0xD: {
                // Wrap when going over the edge of screen
                const auto X = registers_[i.Nibble2()] % PIXELS_X;
                auto y = registers_[i.Nibble3()] % PIXELS_Y;
                registers_[0xF] = 0;

                for (auto n = 0; n < i.Nibble4(); ++n) {
                    auto x = X;
                    const auto sprite = RAM_[I_ + n];
                    for (auto bit = 7; bit >= 0; --bit) {
                        if (sprite & (1 << bit)) {
                            // std::cout << "Flip pixel: " << x << " " << y << '\n';
                            if (display_.FlipPixel(x, y)) {
                                registers_[0xF] = 1;
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
            case 0xE:
            {
                switch(i.SecondByte())
                {
                    case 0x9E:
                    {
                        if(keypad_.KeyDown(registers_[i.Nibble2()]))
                        {
                            PC_ += 2; // Skip
                        }
                        return;
                    }
                    case 0xA1:
                    {
                        if(!keypad_.KeyDown(registers_[i.Nibble2()]))
                        {
                            PC_ += 2; // Skip
                        }
                        return;
                    }
                }
                return;
            }
            case 0xF:
            {
                switch(i.SecondByte()){
                    case 0x07:
                    {
                        registers_[i.Nibble2()] = delay_timer_;
                        return;
                    }
                    case 0x15:
                    {
                        delay_timer_ = registers_[i.Nibble2()];
                        return;
                    }
                    case 0x18:
                    {
                        sound_timer_ = registers_[i.Nibble2()];
                        return;
                    }
                    case 0x1E:
                    {
                        I_ += registers_[i.Nibble2()];
                        return;
                    }
                    case 0x0A:
                    {
                        const auto key = keypad_.KeyPressed();
                        if (!keypad_.KeyPressed(key)) {
                            PC_ -= 2; // Loop
                        }
                        else {
                            registers_[i.Nibble2()] = key;
                        }
                        return;
                    }
                    case 0x29:
                    {
                        I_ = font_address + registers_[i.Nibble2()] * 5;
                        return;
                    }
                    case 0x33:
                    {
                        RAM_[I_] = registers_[i.Nibble2()] / 100 % 10;
                        RAM_[I_ + 1] = registers_[i.Nibble2()] / 10 % 10;
                        RAM_[I_ + 2] = registers_[i.Nibble2()] % 10;
                        return;
                    }
                    case 0x55:
                    {
                        for (auto n = 0; n <= i.Nibble2() && n != sizeof(registers_); ++n) {
                            RAM_[I_ + n] = registers_[n];
                        }
                        if (config_.fx55_incr_I_) {
                            I_ += i.Nibble2() + 1;
                        }
                        return;
                    }
                    case 0x65:
                    {
                        for (auto n = 0; n <= i.Nibble2() && n != sizeof(registers_); ++n) {
                            registers_[n] = RAM_[I_ + n];
                        }
                        return;
                    }
                }
            }
        }

        std::cerr << "Unsupported instruction: " << "0x" << std::hex << i() << '\n';
    }
} // chip8