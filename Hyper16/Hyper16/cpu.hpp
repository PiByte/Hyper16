#include <iostream>

#include "types.hpp"
#include "ram.hpp"

#define START_ADDRESS 0x0400
#define STACK_ADDRESS 0x0100

class CPU
{
public:
	~CPU()
	{
		delete mem;
	}

	void update()
	{
		if (!running) return;

		for (int i = 0; i < 16667; i++)
			step();
	}

	void step()
	{
		if (interrupted)
		{
			interrupted = false;

			// TODO
		}

		// Splices instruction and executes
		byte addressingMode = 0;
		byte instruction = 0;
		byte dip = 0;

		byte fullInstruction = mem->readByte(pc);
		word data = mem->readWord(pc + 1);

		addressingMode = (fullInstruction & 0b11000000) >> 6;
		instruction = (fullInstruction & 0b00111100) >> 2;
		dip = fullInstruction & 0b00000011;

		data = (this->*addressingModePointers[addressingMode])(data);
		(this->*instructionPointers[instruction])(dip, data);

		// Updates program counter
		pc = (pc + length) & 0xFFFF;
	}

	RAM* getMemory()
	{
		return mem;
	}

private:
	byte reg[4] = { 0 };

	bool flags[3] = { false, false, false };

	address pc = 0x0400;
	byte sp = 0x00;
	RAM* mem = new RAM();

	bool running = true;
	bool interrupted = false;

	typedef byte (CPU::*AP)(word data);
	typedef void (CPU::*IP)(byte dip, byte data);

	AP addressingModePointers[4] = {
		&CPU::addrReg,
		&CPU::addrImm,
		&CPU::addrDir,
		&CPU::addrInd
	};

	IP instructionPointers[16] = {
		&CPU::instMov,
		&CPU::instSto,
		&CPU::instSps,
		&CPU::instSpl,
		&CPU::instJmp,
		&CPU::instSbr,
		&CPU::instRet,
		&CPU::instAdd,
		&CPU::instSub,
		&CPU::instAnd,
		&CPU::instIor,
		&CPU::instXor,
		&CPU::instShl,
		&CPU::instShr,
		&CPU::instHlt,
		&CPU::instNop
	};

	void affectFlags(int res)
	{
		flags[0], flags[1], flags[2] = false;

		if (res > 0xFF) flags[0] = true;
		if (res == 0) flags[1] = true;
		if (res < 0) flags[2] = true;
	}

	// These two variables are both definied by the addressing modes, and may be ignored by some instructions.
	byte length = 0; // Temp, instruction length in bytes
	byte lowerByte = 0; // Reserved for JMP and SBR routines. Since they require another byte in addition to the byte that the addressing modes returns.

	// Addressing modes
	byte addrReg(word data)
	{
		length = 2;
		lowerByte = 0;

		return reg[(data >> 8) & 0b11];
	}

	byte addrImm(word data)
	{
		length = 2;
		lowerByte = data & 0xFF;

		return (data >> 8);
	}

	byte addrDir(word data)
	{
		length = 3;

		word d = mem->readWord(data);
		lowerByte = d & 0xFF;

		return (d >> 8);
	}

	byte addrInd(word data)
	{
		length = 3;

		word d = mem->readWord(mem->readWord(data));
		lowerByte = d & 0xFF;

		return (d >> 8);
	}

	// INSTRUCTIONS

	void instMov(byte dip, byte data)
	{
		reg[dip] = data;
	}

	void instSto(byte dip, byte data)
	{
		mem->writeByte(data, reg[dip]);
	}

	void instSps(byte dip, byte data)
	{
		mem->writeByte(STACK_ADDRESS + sp++, data);
	}

	void instSpl(byte dip, byte data)
	{
		reg[dip] = mem->readByte(STACK_ADDRESS + --sp);
	}

	void instJmp(byte dip, byte data)
	{
		if (dip == 0b11 || flags[dip])
		{
			length = 0;
			pc = (data << 8) + lowerByte;
			return;
		}
	}

	void instSbr(byte dip, byte data)
	{
		if (dip == 0b11 || flags[dip])
		{
			mem->writeWord(STACK_ADDRESS + sp, pc);
			sp += 2;

			length = 0;
			pc = (data << 8) + lowerByte;
			return;
		}
	}

	void instRet(byte dip, byte data)
	{
		length = 1;

		sp -= 2;
		pc = mem->readWord(STACK_ADDRESS + sp);
	}

	void instAdd(byte dip, byte data)
	{
		int res = reg[0] + data + flags[0];

		affectFlags(res);
		reg[0] = res & 0xFFFF;
	}

	void instSub(byte dip, byte data)
	{
		int res = reg[0] - data - flags[1];

		affectFlags(res);
		reg[0] = res & 0xFFFF;
	}

	void instAnd(byte dip, byte data)
	{
		int res = reg[0] & data;

		affectFlags(res);
		reg[0] = res & 0xFFFF;
	}

	void instIor(byte dip, byte data)
	{
		int res = reg[0] | data;

		affectFlags(res);
		reg[0] = res & 0xFFFF;
	}

	void instXor(byte dip, byte data)
	{
		int res = reg[0] ^ data;

		affectFlags(res);
		reg[0] = res & 0xFFFF;
	}

	void instShl(byte dip, byte data)
	{
		int res = reg[0] << data;

		affectFlags(res);
		reg[0] = res & 0xFFFF;
	}

	void instShr(byte dip, byte data)
	{
		int res = reg[0] >> data;

		affectFlags(res);
		reg[0] = res & 0xFFFF;
	}

	void instHlt(byte dip, byte data)
	{
		length = 0;

		pc = START_ADDRESS;
		running = false;
	}

	void instNop(byte dip, byte data)
	{
		return;
	}
};