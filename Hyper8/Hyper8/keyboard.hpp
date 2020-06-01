#pragma once

#include "ram.hpp"
#include "types.hpp"

//#define INPUT_VECTOR_ADDRESS 0xFF00
#define INPUT_VECTOR_ADDRESS 0xA000

class Keyboard
{
public:
	void keyDown(RAM* mem, int key)
	{
		mem->writeByte(INPUT_VECTOR_ADDRESS + key, 255);
	}

	void keyUp(RAM* mem, int key)
	{
		mem->writeByte(INPUT_VECTOR_ADDRESS + key, 0);
	}

private:

};