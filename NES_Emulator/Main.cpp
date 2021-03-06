// NES_Emulator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Console.h"

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("Too few arguments.\n");
        return 0;
    }

    std::ifstream ifstream;
    ifstream.open(argv[1], std::ifstream::binary);
    if (!ifstream)
    {
        printf("Unable to open file!\n");
        return 0;
    }

    uint32_t addr = 0;
    sscanf_s(argv[2], "%x", &addr);

    NesEmu::Console console;
    NesEmu::RAM* ram = console.GetRAM();
    int len = 0;
    ifstream.seekg(0, ifstream.end);
    len = ifstream.tellg();
    ifstream.seekg(ifstream.beg);
    ifstream.read((char*)ram->GetMemoryPtr(addr), len);
    ifstream.close();

    printf("File %s is loaded at address 0x%04x\n", argv[1], addr);
    
    console.Reset();
    console.Run();

    return 0;
}

