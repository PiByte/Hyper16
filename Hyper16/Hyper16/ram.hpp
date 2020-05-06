#include "types.hpp"

class RAM
{
public:
	byte readByte(address a) { return mem[a & 0xFFFF]; }
	word readWord(address a) { return (mem[a & 0xFFFF] << 8) + mem[a+1 & 0xFFFF]; }

	void writeByte(address a, byte b) { mem[a & 0xFFFF] = b & 0xFF; }
	void writeWord(address a, word b) { mem[a & 0xFFFF] = (b & 0xFF00) >> 8; mem[a+1 & 0xFFFF] = b & 0x00FF; }

	void writeBuffer(byte* buffer, word length, address offset = 0x0400)
	{
		for (address i = 0; i < length; i++)
		{
			writeByte(offset + i, buffer[i]);
		}
	}

private:
	byte mem[0x10000] = { 0 };
};