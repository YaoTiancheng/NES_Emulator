#include "stdafx.h"
#include "CPU.h"
#include "Console.h"

BEGIN_NES_EMULATOR_NAMESPACE

const uint8_t kInstructionModes[] =
{
    6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
    10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
    1, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
    10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
    6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 1, 1, 1, 1,
    10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
    6, 7, 6, 7, 11, 11, 11, 11, 6, 5, 4, 5, 8, 1, 1, 1,
    10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
    5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
    10, 9, 6, 9, 12, 12, 13, 13, 6, 3, 6, 3, 2, 2, 3, 3,
    5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
    10, 9, 6, 9, 12, 12, 13, 13, 6, 3, 6, 3, 2, 2, 3, 3,
    5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
    10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
    5, 7, 5, 7, 11, 11, 11, 11, 6, 5, 6, 5, 1, 1, 1, 1,
    10, 9, 6, 9, 12, 12, 12, 12, 6, 3, 6, 3, 2, 2, 2, 2,
};

const uint8_t kInstructionSizes[] =
{
    1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
    3, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
    1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
    1, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 1, 0, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 0, 3, 0, 0,
    2, 2, 2, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 1, 2, 1, 0, 3, 3, 3, 0,
    2, 2, 0, 0, 2, 2, 2, 0, 1, 3, 1, 0, 3, 3, 3, 0,
};

// Cycle cost for each instruction.
// Pipling is taken into account. Conditional cycles are not included.
const uint8_t kInstructionCycles[] =
{
    7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
    6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
    6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
    6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
    2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
    2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
    2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
    2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
    2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
    2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
    2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
};

// Additional cycle cost for each instruction if page cross happened.
const uint8_t kInstructionPageCycles[] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
};

const char* kInstructionNames[] =
{
    "BRK", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO",
    "PHP", "ORA", "ASL", "ANC", "NOP", "ORA", "ASL", "SLO",
    "BPL", "ORA", "KIL", "SLO", "NOP", "ORA", "ASL", "SLO",
    "CLC", "ORA", "NOP", "SLO", "NOP", "ORA", "ASL", "SLO",
    "JSR", "AND", "KIL", "RLA", "BIT", "AND", "ROL", "RLA",
    "PLP", "AND", "ROL", "ANC", "BIT", "AND", "ROL", "RLA",
    "BMI", "AND", "KIL", "RLA", "NOP", "AND", "ROL", "RLA",
    "SEC", "AND", "NOP", "RLA", "NOP", "AND", "ROL", "RLA",
    "RTI", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE",
    "PHA", "EOR", "LSR", "ALR", "JMP", "EOR", "LSR", "SRE",
    "BVC", "EOR", "KIL", "SRE", "NOP", "EOR", "LSR", "SRE",
    "CLI", "EOR", "NOP", "SRE", "NOP", "EOR", "LSR", "SRE",
    "RTS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA",
    "PLA", "ADC", "ROR", "ARR", "JMP", "ADC", "ROR", "RRA",
    "BVS", "ADC", "KIL", "RRA", "NOP", "ADC", "ROR", "RRA",
    "SEI", "ADC", "NOP", "RRA", "NOP", "ADC", "ROR", "RRA",
    "NOP", "STA", "NOP", "SAX", "STY", "STA", "STX", "SAX",
    "DEY", "NOP", "TXA", "XAA", "STY", "STA", "STX", "SAX",
    "BCC", "STA", "KIL", "AHX", "STY", "STA", "STX", "SAX",
    "TYA", "STA", "TXS", "TAS", "SHY", "STA", "SHX", "AHX",
    "LDY", "LDA", "LDX", "LAX", "LDY", "LDA", "LDX", "LAX",
    "TAY", "LDA", "TAX", "LAX", "LDY", "LDA", "LDX", "LAX",
    "BCS", "LDA", "KIL", "LAX", "LDY", "LDA", "LDX", "LAX",
    "CLV", "LDA", "TSX", "LAS", "LDY", "LDA", "LDX", "LAX",
    "CPY", "CMP", "NOP", "DCP", "CPY", "CMP", "DEC", "DCP",
    "INY", "CMP", "DEX", "AXS", "CPY", "CMP", "DEC", "DCP",
    "BNE", "CMP", "KIL", "DCP", "NOP", "CMP", "DEC", "DCP",
    "CLD", "CMP", "NOP", "DCP", "NOP", "CMP", "DEC", "DCP",
    "CPX", "SBC", "NOP", "ISC", "CPX", "SBC", "INC", "ISC",
    "INX", "SBC", "NOP", "SBC", "CPX", "SBC", "INC", "ISC",
    "BEQ", "SBC", "KIL", "ISC", "NOP", "SBC", "INC", "ISC",
    "SED", "SBC", "NOP", "ISC", "NOP", "SBC", "INC", "ISC",
};

inline bool ArePageDifferent(uint16_t address1, uint16_t address2)
{
    return (address1 & 0xFF00) != (address2 & 0xFF00);
}

CPU::CPU(Console* console)
    : m_Console(console),
    m_kExecutionTable {
    &CPU::BRK, &CPU::ORA, &CPU::INV, &CPU::INV, &CPU::NOP, &CPU::ORA, &CPU::ASL, &CPU::INV,
    &CPU::PHP, &CPU::ORA, &CPU::ASL, &CPU::INV, &CPU::NOP, &CPU::ORA, &CPU::ASL, &CPU::INV,
    &CPU::BPL, &CPU::ORA, &CPU::INV, &CPU::INV, &CPU::NOP, &CPU::ORA, &CPU::ASL, &CPU::INV,
    &CPU::CLC, &CPU::ORA, &CPU::NOP, &CPU::INV, &CPU::NOP, &CPU::ORA, &CPU::ASL, &CPU::INV,
    &CPU::JSR, &CPU::AND, &CPU::INV, &CPU::INV, &CPU::BIT, &CPU::AND, &CPU::ROL, &CPU::INV,
    &CPU::PLP, &CPU::AND, &CPU::ROL, &CPU::INV, &CPU::BIT, &CPU::AND, &CPU::ROL, &CPU::INV,
    &CPU::BMI, &CPU::AND, &CPU::INV, &CPU::INV, &CPU::NOP, &CPU::AND, &CPU::ROL, &CPU::INV,
    &CPU::SEC, &CPU::AND, &CPU::NOP, &CPU::INV, &CPU::NOP, &CPU::AND, &CPU::ROL, &CPU::INV,
    &CPU::RTI, &CPU::EOR, &CPU::INV, &CPU::INV, &CPU::NOP, &CPU::EOR, &CPU::LSR, &CPU::INV,
    &CPU::PHA, &CPU::EOR, &CPU::LSR, &CPU::INV, &CPU::JMP, &CPU::EOR, &CPU::LSR, &CPU::INV,
    &CPU::BVC, &CPU::EOR, &CPU::INV, &CPU::INV, &CPU::NOP, &CPU::EOR, &CPU::LSR, &CPU::INV,
    &CPU::CLI, &CPU::EOR, &CPU::NOP, &CPU::INV, &CPU::NOP, &CPU::EOR, &CPU::LSR, &CPU::INV,
    &CPU::RTS, &CPU::ADC, &CPU::INV, &CPU::INV, &CPU::NOP, &CPU::ADC, &CPU::ROR, &CPU::INV,
    &CPU::PLA, &CPU::ADC, &CPU::ROR, &CPU::INV, &CPU::JMP, &CPU::ADC, &CPU::ROR, &CPU::INV,
    &CPU::BVS, &CPU::ADC, &CPU::INV, &CPU::INV, &CPU::NOP, &CPU::ADC, &CPU::ROR, &CPU::INV,
    &CPU::SEI, &CPU::ADC, &CPU::NOP, &CPU::INV, &CPU::NOP, &CPU::ADC, &CPU::ROR, &CPU::INV,
    &CPU::NOP, &CPU::STA, &CPU::NOP, &CPU::INV, &CPU::STY, &CPU::STA, &CPU::STX, &CPU::INV,
    &CPU::DEY, &CPU::NOP, &CPU::TXA, &CPU::INV, &CPU::STY, &CPU::STA, &CPU::STX, &CPU::INV,
    &CPU::BCC, &CPU::STA, &CPU::INV, &CPU::INV, &CPU::STY, &CPU::STA, &CPU::STX, &CPU::INV,
    &CPU::TYA, &CPU::STA, &CPU::TXS, &CPU::INV, &CPU::INV, &CPU::STA, &CPU::INV, &CPU::INV,
    &CPU::LDY, &CPU::LDA, &CPU::LDX, &CPU::INV, &CPU::LDY, &CPU::LDA, &CPU::LDX, &CPU::INV,
    &CPU::TAY, &CPU::LDA, &CPU::TAX, &CPU::INV, &CPU::LDY, &CPU::LDA, &CPU::LDX, &CPU::INV,
    &CPU::BCS, &CPU::LDA, &CPU::INV, &CPU::INV, &CPU::LDY, &CPU::LDA, &CPU::LDX, &CPU::INV,
    &CPU::CLV, &CPU::LDA, &CPU::TSX, &CPU::INV, &CPU::LDY, &CPU::LDA, &CPU::LDX, &CPU::INV,
    &CPU::CPY, &CPU::CMP, &CPU::NOP, &CPU::INV, &CPU::CPY, &CPU::CMP, &CPU::DEC, &CPU::INV,
    &CPU::INY, &CPU::CMP, &CPU::DEX, &CPU::INV, &CPU::CPY, &CPU::CMP, &CPU::DEC, &CPU::INV,
    &CPU::BNE, &CPU::CMP, &CPU::INV, &CPU::INV, &CPU::NOP, &CPU::CMP, &CPU::DEC, &CPU::INV,
    &CPU::CLD, &CPU::CMP, &CPU::NOP, &CPU::INV, &CPU::NOP, &CPU::CMP, &CPU::DEC, &CPU::INV,
    &CPU::CPX, &CPU::SBC, &CPU::NOP, &CPU::INV, &CPU::CPX, &CPU::SBC, &CPU::INC, &CPU::INV,
    &CPU::INX, &CPU::SBC, &CPU::NOP, &CPU::SBC, &CPU::CPX, &CPU::SBC, &CPU::INC, &CPU::INV,
    &CPU::BEQ, &CPU::SBC, &CPU::INV, &CPU::INV, &CPU::NOP, &CPU::SBC, &CPU::INC, &CPU::INV,
    &CPU::SED, &CPU::SBC, &CPU::NOP, &CPU::INV, &CPU::NOP, &CPU::SBC, &CPU::INC, &CPU::INV }
{
}

bool CPU::Step(char* statusStr)
{
    if (m_BranchCycles != 0)
    {
        m_BranchCycles--;
        return false;
    }
    else
    {
        m_Cycles--;
        if (m_Cycles != 0)
            return false;
    }

    // Look-up and call the instruction function
    (*this.*m_kExecutionTable[m_OpCode])();

    if (statusStr != nullptr)
        GetStatusStr(statusStr);

    // Handle interrupt
    switch (m_Interrupt)
    {
    case InterruptType::NMI:
        NMIInterrupt();
        m_Cycles = 7;
        break;
    case InterruptType::IRQ:
        IRQInterrupt();
        m_Cycles = 7;
        break;
    default:
        m_Cycles = 0;
        break;
    }
    m_Interrupt = InterruptType::None;

    FetchNextOpCode();

    return true;
}

void CPU::Init(uint16_t PC)
{
    m_A = 0;
    m_X = 0;
    m_Y = 0;
    m_PC = PC;
    m_SP = 0xFF;
    m_P = 0;
    m_Cycles = 0;
    m_BranchCycles = 0;
}

void CPU::GetStatusStr(char* str) const
{
    sprintf_s(str, kMaxStatusStrLength, "%s A=%-3d X=%-3d Y=%-3d PC=0x%04x SP=0x%02x N=%1d V=%1d B=%1d D=%1d I=%1d Z=%1d C=%1d\n", 
        kInstructionNames[m_OpCode], m_A, m_X, m_Y, m_PC, m_SP, 
        (m_P & 0x80) >> 7, (m_P & 0x40) >> 6, (m_P & 0x10) >> 4, (m_P & 0x08) >> 3, (m_P & 0x04) >> 2, (m_P & 0x02) >> 1, m_P & 0x01);
}

void CPU::FetchNextOpCode()
{
    m_OpCode = m_Console->Read(m_PC);
    m_Mode = (AddressingMode)kInstructionModes[m_OpCode];
    m_Address = 0;
    bool pageCrossed = false;
    switch (m_Mode)
    {
    case AddressingMode::Absolute:
        m_Address = Read16(m_PC + 1);
        break;
    case AddressingMode::AbsoluteX:
        m_Address = Read16(m_PC + 1) + m_X;
        pageCrossed = ArePageDifferent(m_Address - m_X, m_Address);
        break;
    case AddressingMode::AbsoluteY:
        m_Address = Read16(m_PC + 1) + m_Y;
        pageCrossed = ArePageDifferent(m_Address - m_Y, m_Address);
        break;
    case AddressingMode::Accumulator:
        m_Address = 0;
        break;
    case AddressingMode::Immediate:
        m_Address = m_PC + 1;
        break;
    case AddressingMode::Implied:
        m_Address = 0;
        break;
    case AddressingMode::IndexedIndirect:
        m_Address = Read16Bug((uint16_t)m_Console->Read(m_PC + 1) + m_X);
        break;
    case AddressingMode::Indirect:
        m_Address = Read16Bug(Read16(m_PC + 1));
        break;
    case AddressingMode::IndirectIndexed:
        m_Address = Read16Bug((uint16_t)m_Console->Read(m_PC + 1)) + m_Y;
        pageCrossed = ArePageDifferent(m_Address - m_Y, m_Address);
        break;
    case AddressingMode::Relative:
    {
        uint8_t offset = m_Console->Read(m_PC + 1);
        m_Address = m_PC + 2 + offset;
        if (offset >= 0x80u)
            m_Address -= 0x100;
        break;
    }
    case AddressingMode::ZeroPage:
        m_Address = m_Console->Read(m_PC + 1);
        break;
    case AddressingMode::ZeroPageX:
        m_Address = (m_Console->Read(m_PC + 1) + m_X) & 0xFF;
        break;
    case AddressingMode::ZeroPageY:
        m_Address = (m_Console->Read(m_PC + 1) + m_Y) & 0xFF;
        break;
    default:
        break;
    }

    m_PC += kInstructionSizes[m_OpCode];
    m_Cycles += kInstructionCycles[m_OpCode];
    if (pageCrossed)
        m_Cycles += kInstructionPageCycles[m_OpCode];
}

void CPU::AddBranchCycles()
{
    m_BranchCycles = 1;
    if (ArePageDifferent(m_PC, m_Address))
        m_BranchCycles++;
}

uint16_t CPU::Read16(uint16_t address) const
{
    uint8_t lo = m_Console->Read(address);
    uint8_t hi = m_Console->Read(address + 1);
    return uint16_t(hi) << 8 | lo;
}

uint16_t CPU::Read16Bug(uint16_t address) const
{
    uint8_t lo = m_Console->Read(address);
    uint8_t hi = m_Console->Read(address & 0xFF00 | uint8_t(address) + 1);
    return uint16_t(hi) << 8 | lo;
}

inline void CPU::Push16(uint16_t value)
{
    uint8_t hi = value >> 8;
    uint8_t lo = value & 0xFF;
    Push(hi);
    Push(lo);
}

inline void CPU::Push(uint8_t value)
{
    m_Console->Write(0x0100 | m_SP, value);
    m_SP--;
}

inline uint16_t CPU::Pull16()
{
    uint8_t lo = Pull();
    uint8_t hi = Pull();
    return uint16_t(hi) << 8 | lo;
}

inline uint8_t CPU::Pull()
{
    uint8_t value = m_Console->Read(0x0100 | m_SP);
    m_SP++;
    return value;
}

inline void CPU::SetZ(uint8_t value)
{
    m_P = m_P & ~0x02 | (value == 0 ? 0x00 : 0x02);
}

inline void CPU::SetN(uint8_t value)
{
    m_P = m_P & ~0x80 | value & 0x80;
}

// Set zero flag if value equals zero.
// Set negative flag if value is negative.
inline void CPU::SetZN(uint8_t value)
{
    m_P = m_P & ~0x02 | (value == 0 ? 0x02 : 0x00);
    m_P = m_P & ~0x80 | value & 0x80;
}

void CPU::Compare(uint8_t a, uint8_t b)
{
    // Set carry bit and subtract
    uint16_t sub = (a | 0x0100) - b;
    SetZN(uint8_t(sub));
    m_P = m_P & 0xFE | sub >> 8;
}

void CPU::NMIInterrupt()
{
    Push16(m_PC);
    // Push status flags and bit 5 is set to 1
    // https://wiki.nesdev.com/w/index.php/CPU_status_flag_behavior
    Push(m_P | 0x20);
    m_PC = Read16(0xFFFA);
    // Set bit 2 (interrupt flag) to one
    m_P |= 0x04;
}

void CPU::IRQInterrupt()
{
    Push16(m_PC);
    // Push status flags and bit 5 is set to 1
    // https://wiki.nesdev.com/w/index.php/CPU_status_flag_behavior
    Push(m_P | 0x20);
    m_PC = Read16(0xFFFE);
    // Set bit 2 (interrupt flag) to one
    m_P |= 0x04;
}

// ADC - Add with carry
void CPU::ADC()
{
    uint16_t a = m_A;
    uint16_t b = m_Console->Read(m_Address);
    uint16_t c = m_P & 0x01;
    uint16_t sum = a + b + c;
    m_A = (uint8_t)sum;
    SetZN(m_A);
    // Set carry flag if sum results a carry
    m_P = m_P & ~0x01 | sum >> 8;
    // Set overflow flag if sum results an overflow
    m_P = m_P & ~0x40 | ((a ^ sum) & (b ^ sum) & 0x80) >> 1;
}

// AND - Logical AND
void CPU::AND()
{
    m_A = m_A & m_Console->Read(m_Address);
    SetZN(m_A);
}

// ASL - Arithmetic shift left
void CPU::ASL()
{
    if (m_Mode == AddressingMode::Accumulator)
    {
        // Set carry flag if MSB is 1
        m_P = m_P & ~0x01 | m_A >> 7;
        m_A <<= 1;
        SetZN(m_A);
    }
    else
    {
        uint8_t value = m_Console->Read(m_Address);
        // Set carry flag if MSB is 1
        m_P = m_P & ~0x01 | value >> 7;
        value <<= 1;
        m_Console->Write(m_Address, value);
        SetZN(value);
    }
}

// BCC - Branch if carry clear
void CPU::BCC()
{
    if ((m_P & 0x01) == 0)
    {
        m_PC = m_Address;
        AddBranchCycles();
    }
}

// BCS - Branch if carry set
void CPU::BCS()
{
    if ((m_P & 0x01) != 0)
    {
        m_PC = m_Address;
        AddBranchCycles();
    }
}

// BEG - Branch if equal
void CPU::BEQ()
{
    if ((m_P & 0x02) != 0)
    {
        m_PC = m_Address;
        AddBranchCycles();
    }
}

// BIT - Bit test
void CPU::BIT()
{
    uint8_t value = m_Console->Read(m_Address);
    // Bits 7 and 6 of operand are transfered to bit 7 and 6 of status flags
    m_P = m_P & ~0xC0 | value & 0xC0;
    SetZ(value & m_A);
}

// BMI - Branch if minus
void CPU::BMI()
{
    if ((m_P & 0x80) != 0)
    {
        m_PC = m_Address;
        AddBranchCycles();
    }
}

// BNE - Branch if not equal
void CPU::BNE()
{
    if ((m_P & 0x02) == 0)
    {
        m_PC = m_Address;
        AddBranchCycles();
    }
}

// BPL - Branch on plus
void CPU::BPL()
{
    if ((m_P & 0x80) == 0)
    {
        m_PC = m_Address;
        AddBranchCycles();
    }
}

// BRK - Break
void CPU::BRK()
{
    Push16(m_PC);
    // Push status flags, bit 5 & 4 are always set to 1.
    // https://wiki.nesdev.com/w/index.php/CPU_status_flag_behavior
    Push(m_P | 0x30);
    m_PC = Read16(0xFFFE);
    // Set bit 2 (interrupt flag) to one
    m_P |= 0x04;
}

// BVC - Branch if overflow clear
void CPU::BVC()
{
    if ((m_P & 0x40) == 0)
    {
        m_PC = m_Address;
        AddBranchCycles();
    }
}

// BVS - Branch if overflow set
void CPU::BVS()
{
    if ((m_P & 0x40) != 0)
    {
        m_PC = m_Address;
        AddBranchCycles();
    }
}

// CLC - Clear carry flag
void CPU::CLC()
{
    m_P &= 0xFE;
}

// CLD - Clear decimal mode
void CPU::CLD()
{
    m_P &= 0xF7;
}

// CLI - Clear interrupt disable
void CPU::CLI()
{
    m_P &= 0xFB;
}

// CLV - Clear overflow flag
void CPU::CLV()
{
    m_P &= 0xBF;
}

// CMP - Compare
void CPU::CMP()
{
    uint8_t value = m_Console->Read(m_Address);
    Compare(m_A, value);
}

// CPX - Compare X register
void CPU::CPX()
{
    uint8_t value = m_Console->Read(m_Address);
    Compare(m_X, value);
}

// CPY - Compare Y register
void CPU::CPY()
{
    uint8_t value = m_Console->Read(m_Address);
    Compare(m_Y, value);
}

// DEC - Decrement memory
void CPU::DEC()
{
    uint8_t value = m_Console->Read(m_Address) - 1;
    m_Console->Write(m_Address, value);
    SetZN(value);
}

// DEX - Decrement X register
void CPU::DEX()
{
    m_X--;
    SetZN(m_X);
}

// DEY - Decrement Y register
void CPU::DEY()
{
    m_Y--;
    SetZN(m_Y);
}

// EOR - Exclusive or
void CPU::EOR()
{
    m_A = m_A ^ m_Console->Read(m_Address);
    SetZN(m_A);
}

// INC - Increment memory
void CPU::INC()
{
    uint8_t value = m_Console->Read(m_Address) + 1;
    m_Console->Write(m_Address, value);
    SetZN(value);
}

// INX - Increment x register
void CPU::INX()
{
    m_X++;
    SetZN(m_X);
}

// INY - Increment y register
void CPU::INY()
{
    m_Y++;
    SetZN(m_Y);
}

// JMP - Jump
void CPU::JMP()
{
    m_PC = m_Address;
}

// JSR - Jump to subroutine
void CPU::JSR()
{
    Push16(m_PC - 1);
    m_PC = m_Address;
}

// LDA - Load accumulator
void CPU::LDA()
{
    m_A = m_Console->Read(m_Address);
    SetZN(m_A);
}

// LDX - Load register x
void CPU::LDX()
{
    m_X = m_Console->Read(m_Address);
    SetZN(m_X);
}

// LDY - Load register y
void CPU::LDY()
{
    m_Y = m_Console->Read(m_Address);
    SetZN(m_Y);
}

// LSR - Logical shift right
void CPU::LSR()
{
    if (m_Mode == AddressingMode::Accumulator)
    {
        // Shift bit 0 to carry flag
        m_P = m_P & 0xFE | m_A & 0x01;
        m_A >>= 1;
        SetZN(m_A);
    }
    else
    {
        uint8_t value = m_Console->Read(m_Address);
        // Shift bit 0 to carry flag
        m_P = m_P & 0xFE | value & 0x01;
        value >>= 1;
        SetZN(value);
        m_Console->Write(m_Address, value);
    }
}

// NOP - No operation
void CPU::NOP()
{
}

// ORA - Inclusive or with accumulator
void CPU::ORA()
{
    m_A |= m_Console->Read(m_Address);
    SetZN(m_A);
}

// PHA - Push accumulator
void CPU::PHA()
{
    Push(m_A);
}

// PHP - Push processor status
void CPU::PHP()
{
    // Push status flags, bit 5 & 4 are always set to 1.
    // https://wiki.nesdev.com/w/index.php/CPU_status_flag_behavior
    Push(m_P | 0x30);
}

// PLA - Pull accumulator
void CPU::PLA()
{
    m_A = Pull();
    SetZN(m_A);
}

// PLP - Pull status flags
void CPU::PLP()
{
    m_P = Pull() & 0xCF;
}

// ROL - Rotate left
void CPU::ROL()
{
    if (m_Mode == AddressingMode::Accumulator)
    {
        uint8_t c = m_P & 0x01;
        m_P = m_P & 0xFE | m_A >> 7;
        m_A <<= 1;
        m_A |= c;
        SetZN(m_A);
    }
    else
    {
        uint8_t value = m_Console->Read(m_Address);
        uint8_t c = m_P & 0x01;
        m_P = m_P & 0xFE | value >> 7;
        value <<= 1;
        value |= c;
        SetZN(value);
        m_Console->Write(m_Address, value);
    }
}

// ROR - Rotate right
void CPU::ROR()
{
    if (m_Mode == AddressingMode::Accumulator)
    {
        uint8_t c = m_P & 0x01;
        m_P = m_P & 0xFE | m_A & 0x01;
        m_A >>= 1;
        m_A |= c << 7;
        SetZN(m_A);
    }
    else
    {
        uint8_t value = m_Console->Read(m_Address);
        uint8_t c = m_P & 0x01;
        m_P = m_P & 0xFE | value & 0x01;
        value >>= 1;
        value |= c << 7;
        SetZN(value);
        m_Console->Write(m_Address, value);
    }
}

// RTI - Return from interrupt
void CPU::RTI()
{
    m_P = Pull() & 0xCF;
    m_PC = Pull16();
}

// RTS - Return from subroutine
void CPU::RTS()
{
    m_PC = Pull16() + 1;
}

// SBC - Subtract with carry
void CPU::SBC()
{
    // Subtraction is implemented the same as ADC with only one difference is that
    // the ones-complement of number b is added to register A.
    uint16_t a = m_A;
    // Mask out the higher 8 bits since integer promotion will cause zero extending happens before bit flipping.
    uint16_t b = ~m_Console->Read(m_Address) & 0x00FF;
    uint16_t c = m_P & 0x01;
    uint16_t sub = a + b + c;
    m_A = uint8_t(sub);
    SetZN(m_A);
    // Set carry flag if sub results a carry
    m_P = m_P & ~0x01 | sub >> 8;
    // Set overflow flag if sub results an overflow
    m_P = m_P & ~0x40 | ((a ^ sub) & (b ^ sub) & 0x80) >> 1;
}

// SEC - Set carry flag
void CPU::SEC()
{
    m_P |= 0x01;
}

// SED - Set decimal flag
void CPU::SED()
{
    m_P |= 0x08;
}

// SEI - Set interrupt flag
void CPU::SEI()
{
    m_P |= 0x04;
}

// STA - Store accumulator
void CPU::STA()
{
    m_Console->Write(m_Address, m_A);
}

// STX - Store register X
void CPU::STX()
{
    m_Console->Write(m_Address, m_X);
}

// STY - Store register Y
void CPU::STY()
{
    m_Console->Write(m_Address, m_Y);
}

// TAX - Transfer accumulator to register X
void CPU::TAX()
{
    m_X = m_A;
    SetZN(m_X);
}

// TAY - Transfer accumulator to register Y
void CPU::TAY()
{
    m_Y = m_A;
    SetZN(m_Y);
}

// TSX - Transfer stack pointer to X
void CPU::TSX()
{
    m_X = m_SP;
    SetZN(m_X);
}

// TXA - Transfer register X to accumulator
void CPU::TXA()
{
    m_A = m_X;
    SetZN(m_A);
}

// TXS - Transfer register X to stack pointer
void CPU::TXS()
{
    m_SP = m_X;
}

// TYA - Transfer register Y to accumulator
void CPU::TYA()
{
    m_A = m_Y;
    SetZN(m_A);
}

// INV - Invalid instruction
void CPU::INV()
{
}


END_NES_EMULATOR_NAMESPACE


