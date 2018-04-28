#pragma once

#include "CPU.h"
#include "RAM.h"

BEGIN_NES_EMULATOR_NAMESPACE

class Console
{
    friend class CPU;

public:

    Console();

    void Reset();

    void Run();

    CPU* GetCPU() { return &m_CPU; }

    RAM* GetRAM() { return &m_RAM; };

private:

    uint8_t Read(uint16_t address) const;

    void Write(uint16_t address, uint8_t value);

    CPU m_CPU;
    RAM m_RAM;
    char m_CpuStatusStr[CPU::kMaxStatusStrLength];
    uint32_t m_CycleCount;
};

END_NES_EMULATOR_NAMESPACE