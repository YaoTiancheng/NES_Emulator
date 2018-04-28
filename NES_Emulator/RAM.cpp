#include "stdafx.h"
#include "RAM.h"

BEGIN_NES_EMULATOR_NAMESPACE

uint8_t RAM::Read(uint16_t address) const
{
	return m_Memory[address];
}

void RAM::Write(uint16_t address, uint8_t value)
{
	m_Memory[address] = value;
}

void* RAM::GetMemoryPtr(uint16_t address)
{
	return m_Memory + address;
}

END_NES_EMULATOR_NAMESPACE


