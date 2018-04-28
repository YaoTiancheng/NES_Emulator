#include "stdafx.h"
#include "Console.h"

BEGIN_NES_EMULATOR_NAMESPACE

Console::Console()
    : m_CPU(this),
    m_CycleCount(0)
{
}

void Console::Reset()
{
    m_CPU.Init(0x0600);
    m_CPU.GetStatusStr(m_CpuStatusStr);
    printf("Init           %s", m_CpuStatusStr);
    m_CPU.FetchNextOpCode();
}

void Console::Run()
{
    for (;;)
    {
        if (m_CPU.Step(m_CpuStatusStr))
        {
            printf("Cycle %-8d %s", m_CycleCount, m_CpuStatusStr);

            char c = 0;
            while (scanf_s(" %c", &c, 1))
            {
                if (c == 'm')
                {
                    int offset = 0, row = 0;
                    scanf_s(" %x %d", &offset, &row);
                    unsigned char* addr = (unsigned char*)m_RAM.GetMemoryPtr(offset);
                    for (int i = 0; i < row && offset + i * 8 < RAM::kSize; i++)
                    {
                        printf("0x%04x ", offset + i * 8);
                        printf("%02x %02x %02x %02x %02x %02x %02x %02x\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6], addr[7]);
                        addr += 8;
                    }
                    printf("\n");
                }
                else if (c == 'q')
                    return;
                else
                    break;
            }
        }
        m_CycleCount++;
    }
}

uint8_t Console::Read(uint16_t address) const
{
    return m_RAM.Read(address);
}

void Console::Write(uint16_t address, uint8_t value)
{
    m_RAM.Write(address, value);
}

END_NES_EMULATOR_NAMESPACE


