// NES_Emulator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Console.h"

int main()
{
	char filename[256];
	printf("Memory file:\n");
	scanf_s("%s", filename, sizeof(filename));

	std::ifstream ifstream;
	ifstream.open(filename, std::ifstream::binary);
	if (!ifstream)
	{
		printf("Unable to open file!\n");
		return 0;
	}

	uint32_t addr = 0;
	printf("Load to address:\n");
	scanf_s("%x", &addr, sizeof(addr));

	NesEmu::Console console;
	NesEmu::RAM* ram = console.GetRAM();
	int len = 0;
	ifstream.seekg(0, ifstream.end);
	len = ifstream.tellg();
	ifstream.seekg(ifstream.beg);
	ifstream.read((char*)ram->GetMemoryPtr(addr), len);
	ifstream.close();
	
	console.Reset();
	console.Run();

    return 0;
}

