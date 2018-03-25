#pragma once

BEGIN_NES_EMULATOR_NAMESPACE

class RAM
{
public:

	uint8_t Read(uint16_t address) const;

	void Write(uint16_t address, uint8_t value);
};

END_NES_EMULATOR_NAMESPACE
