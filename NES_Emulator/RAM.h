#pragma once

BEGIN_NES_EMULATOR_NAMESPACE

class RAM
{
public:

	static const uint32_t kSize = 65536;

	uint8_t Read(uint16_t address) const;

	void Write(uint16_t address, uint8_t value);

	void* GetMemoryPtr(uint16_t address);

private:

	uint8_t m_Memory[kSize];
};

END_NES_EMULATOR_NAMESPACE
