#include <iostream>

#include "types.hpp"
#include "ram.hpp"

#define START_ADDRESS 0x0400
#define STACK_ADDRESS 0x0100

class CPU
{
public:
	void update()
	{
		if (!running) return;

		// this assumes that this function is updated 60 times every second
		// 1 MHz, or ~1 000 000 instructions per second
		for (int i = 0; i < 16667; i++)
			step();
	}

	void step()
	{
		// Splices instruction and executes
		byte fullInstruction = mem.readByte(pc);
		word data = mem.readWord(pc + 1);

		byte addressingMode = (fullInstruction & 0b11000000) >> 6;
		byte instruction = (fullInstruction & 0b00111100) >> 2;

		switchData = fullInstruction & 0b00000011; // Variable definied above the wall of instruction definitions

		(this->*addressingModePointers[addressingMode])(data);
		(this->*instructionPointers[instruction])();

		// Updates program counter
		pc = (pc + length) & 0xFFFF;
	}

	RAM* getMemory() { return &mem; }
	bool isRunning() { return running; }

private:
	byte reg[4] = { 0 };
	bool flags[3] = { false, false, false };

	address pc = START_ADDRESS;
	byte sp = 0x00;
	RAM mem;

	bool running = true;

	// Sets up a table of instructions that are used to quickly execute instructions
	// Alternatively you could use a massive switch statement, but that's less efficient (probably idk)
	typedef void (CPU::*AP)(word data);
	typedef void (CPU::*IP)();

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
		&CPU::instCmp,
		&CPU::instHlt
	};

	void affectFlags(int res)
	{
		flags[0] = false; flags[1] = false; flags[2] = false;

		if (res > 0xFF) flags[0] = true;
		if (res == 0) flags[1] = true;
		if (res < 0) flags[2] = true;
	}

	// These variables are used during execution, there definied here and not passed as arguments because it's easier this way
	byte length = 0; // Temp, instruction length in bytes
	byte higherByte = 0; // Data
	byte lowerByte = 0; // Reserved for JMP and SBR routines. Since they require another byte in addition to the byte that the addressing modes returns.
	byte switchData = 0; // Last 2 bits of instruction

	// Addressing modes
	void addrReg(word data)
	{
		length = 2;

		higherByte =  reg[(data >> 8) & 0b11];
		lowerByte = 0;
	}

	void addrImm(word data)
	{
		length = 2;

		lowerByte = data & 0xFF;
		higherByte = data >> 8;
	}

	void addrDir(word data)
	{
		length = 3;

		word d = mem.readWord(data);

		lowerByte = d & 0xFF;
		higherByte = d >> 8;
	}

	void addrInd(word data)
	{
		length = 3;

		word d = mem.readWord(mem.readWord(data));

		lowerByte = d & 0xFF;
		higherByte = d >> 8;
	}

	// Instructions
	void instMov()
	{
		reg[switchData] = higherByte;
	}

	void instSto()
	{
		mem.writeByte(higherByte, reg[switchData]);
	}

	void instSps()
	{
		mem.writeByte(STACK_ADDRESS + sp++, higherByte);
	}

	void instSpl()
	{
		reg[0] = mem.readByte(STACK_ADDRESS + --sp);
	}

	void instJmp()
	{
		if (switchData == 0b00 || flags[switchData - 1]) //todo explain
		{
			length = 0;
			pc = (higherByte << 8) + lowerByte;
		}
	}

	void instSbr()
	{
		if (switchData == 0b00 || flags[switchData - 1])
		{
			mem.writeWord(STACK_ADDRESS + sp, pc);
			sp += 2;

			length = 0;
			pc = (higherByte << 8) + lowerByte;
		}
	}

	void instRet()
	{
		length = 1; // Short instruction, and will therefore only have a length of 1.

		sp -= 2;
		pc = mem.readWord(STACK_ADDRESS + sp);
	}

	void instAdd()
	{
		int res = reg[0] + higherByte + flags[0];

		affectFlags(res);
		reg[0] = res & 0xFFFF;
	}

	void instSub()
	{
		int res = reg[0] - higherByte - flags[1];

		affectFlags(res);
		reg[0] = res & 0xFFFF;
	}

	void instAnd()
	{
		int res = reg[0] & higherByte;

		affectFlags(res);
		reg[0] = res & 0xFFFF;
	}

	void instIor()
	{
		int res = reg[0] | higherByte;

		affectFlags(res);
		reg[0] = res & 0xFFFF;
	}

	void instXor()
	{
		int res = reg[0] ^ higherByte;

		affectFlags(res);
		reg[0] = res & 0xFFFF;
	}

	void instShl()
	{
		int res = reg[0] << higherByte;

		affectFlags(res);
		reg[0] = res & 0xFFFF;
	}

	void instShr()
	{
		int res = reg[0] >> higherByte;

		affectFlags(res);
		reg[0] = res & 0xFFFF;
	}

	void instCmp()
	{
		int res = reg[0] - higherByte;

		flags[0] = false; flags[1] = false; flags[2] = false;

		if (res > 0) flags[0] = true;
		if (res == 0) flags[1] = true;
		if (res < 0) flags[2] = true;
	}

	void instHlt()
	{
		length = 0;
		pc = START_ADDRESS;

		running = false;
	}
};