#include "types.hpp"

#include <vector>

class RAM
{
public:
	byte readByte(address a) { return mem[a & 0xFFFF]; }
	word readWord(address a) { return (mem[a & 0xFFFF] << 8) + mem[a+1 & 0xFFFF]; }

	void writeByte(address a, byte b) { mem[a & 0xFFFF] = b & 0xFF; }
	void writeWord(address a, word b) { mem[a & 0xFFFF] = (b & 0xFF00) >> 8; mem[a+1 & 0xFFFF] = b & 0x00FF; }

	void writeBuffer(std::vector<byte> buffer, address offset = 0x0400) // 0x0400 is the starting position for the pc
	{
		for (address i = 0; i < buffer.size(); i++)
			writeByte(offset + i, buffer[i]);
	}

private:
	byte mem[0x10000] = { 0 }; // Memory, 64k is the maximum amount of ram, that is available with an addressing width of 16 bytes. no idea if that's the right terminology
};