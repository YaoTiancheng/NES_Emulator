#include "stdafx.h"
#include "CPU.h"
#include "RAM.h"

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

inline bool ArePageDifferent(uint16_t address1, uint16_t address2)
{
	return (address1 & 0xFF00) != (address2 & 0xFF00);
}

CPU::CPU()
	: m_kExecutionTable{ &CPU::PHP }
{
}

void CPU::Step()
{
	if (m_BranchCycles != 0)
	{
		m_BranchCycles--;
		return;
	}
	else
	{
		m_Cycles--;
		if (m_Cycles != 0)
			return;
	}

	// Look-up and call the instruction function
	(*this.*m_kExecutionTable[m_OpCode])();

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

	FetchNextInstruction();
}

void CPU::FetchNextInstruction()
{
	m_OpCode = m_RAM->Read(m_PC);
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
		m_Address = Read16Bug((uint16_t)m_RAM->Read(m_PC + 1) + m_X);
		break;
	case AddressingMode::Indirect:
		m_Address = Read16Bug(Read16(m_PC + 1));
		break;
	case AddressingMode::IndirectIndexed:
		m_Address = Read16Bug((uint16_t)m_RAM->Read(m_PC + 1)) + m_Y;
		pageCrossed = ArePageDifferent(m_Address - m_Y, m_Address);
		break;
	case AddressingMode::Relative:
	{
		uint8_t offset = m_RAM->Read(m_PC + 1);
		m_Address = m_PC + 2 + offset;
		// Subtract 0x100 if offset is negative.
		// No idea why ...
		if (offset >= 0x80u)
			m_Address -= 0x100;
		break;
	}
	case AddressingMode::ZeroPage:
		m_Address = m_RAM->Read(m_PC + 1);
		break;
	case AddressingMode::ZeroPageX:
		m_Address = (m_RAM->Read(m_PC + 1) + m_X) & 0xFF;
		break;
	case AddressingMode::ZeroPageY:
		m_Address = (m_RAM->Read(m_PC + 1) + m_Y) & 0xFF;
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
	return uint16_t();
}

uint16_t CPU::Read16Bug(uint16_t address) const
{
	return uint16_t();
}

inline void CPU::Push16(uint16_t value)
{
}

inline void CPU::Push(uint8_t value)
{
}

inline uint8_t CPU::Pull()
{
	return uint8_t();
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
	m_P = m_P & ~0x02 | (value == 0 ? 0x00 : 0x02);
	m_P = m_P & ~0x80 | value & 0x80;
}

void CPU::Compare(uint8_t a, uint8_t b)
{
	// Set carry bit and subtract
	uint16_t sub = (a | 0x0100) - b;
	SetZN(uint8_t(sub));
	m_P = m_P & 0xFE | (sub & 0x0100) >> 8;
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
	uint16_t b = m_RAM->Read(m_Address);
	uint16_t c = m_P & 0x01;
	uint16_t sum = a + b + c;
	m_A = (uint8_t)sum;
	SetZN(m_A);
	// Set carry flag if sum results a carry
	m_P = m_P & ~0x01 | sum >> 8;
	// Set overflow flag if sum results an overflow
	m_P = m_P & ~0x40 | ((~(a ^ b)) & (a ^ c) & 0x80u) >> 1;
}

// AND - Logical AND
void CPU::AND()
{
	m_A = m_A & m_RAM->Read(m_Address);
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
		uint8_t value = m_RAM->Read(m_Address);
		// Set carry flag if MSB is 1
		m_P = m_P & ~0x01 | value >> 7;
		value <<= 1;
		m_RAM->Write(m_Address, value);
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
	uint8_t value = m_RAM->Read(m_Address);
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
	uint8_t value = m_RAM->Read(m_Address);
	Compare(m_A, value);
}

// CPX - Compare X register
void CPU::CPX()
{
	uint8_t value = m_RAM->Read(m_Address);
	Compare(m_X, value);
}

// CPY - Compare Y register
void CPU::CPY()
{
	uint8_t value = m_RAM->Read(m_Address);
	Compare(m_Y, value);
}

// DEC - Decrement memory
void CPU::DEC()
{
	uint8_t value = m_RAM->Read(m_Address) - 1;
	m_RAM->Write(m_Address, value);
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
	m_A = m_A ^ m_RAM->Read(m_Address);
	SetZN(m_A);
}

// INC - Increment memory
void CPU::INC()
{
	uint8_t value = m_RAM->Read(m_Address) + 1;
	m_RAM->Write(m_Address, value);
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
	m_A = m_RAM->Read(m_Address);
	SetZN(m_A);
}

// LDX - Load register x
void CPU::LDX()
{
	m_X = m_RAM->Read(m_Address);
	SetZN(m_X);
}

// LDY - Load register y
void CPU::LDY()
{
	m_Y = m_RAM->Read(m_Address);
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
		uint8_t value = m_RAM->Read(m_Address);
		// Shift bit 0 to carry flag
		m_P = m_P & 0xFE | value & 0x01;
		value >>= 1;
		SetZN(value);
		m_RAM->Write(m_Address, value);
	}
}

// NOP - No operation
void CPU::NOP()
{
}

// ORA - Inclusive or with accumulator
void CPU::ORA()
{
	m_A |= m_RAM->Read(m_Address);
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
		uint8_t value = m_RAM->Read(m_Address);
		uint8_t c = m_P & 0x01;
		m_P = m_P & 0xFE | value >> 7;
		value <<= 1;
		value |= c;
		SetZN(value);
		m_RAM->Write(m_Address, value);
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
		uint8_t value = m_RAM->Read(m_Address);
		uint8_t c = m_P & 0x01;
		m_P = m_P & 0xFE | value & 0x01;
		value >>= 1;
		value |= c << 7;
		SetZN(value);
		m_RAM->Write(m_Address, value);
	}
}

END_NES_EMULATOR_NAMESPACE


