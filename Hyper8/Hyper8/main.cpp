/*
Loads rom file from disk, and starts the computer.
*/

#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>

#include "types.hpp"
#include "cpu.hpp"

void printCpuInfo(cpuInfo c)
{
    std::cout << "R: [ " << (int)c.reg[0] << ", " << (int)c.reg[1] << ", " << (int)c.reg[2] << ", " << (int)c.reg[3] << " ]" << std::endl;
    std::cout << "F: [ " << c.flags[0] << ", " << c.flags[1] << ", " << c.flags[2] << " ]" << std::endl;
    std::cout << "PC: " << (int)c.pc << std::endl;
    std::cout << "SP: " << (int)c.sp << std::endl;
    std::cout << std::endl;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "(Emulator) Please specify binary to run!" << std::endl;
        return 0;
    }

    std::fstream userFile(argv[1], std::fstream::in);

    std::vector<byte> userBinary;

    if (userFile.is_open())
    {
        userBinary = std::vector<byte>(std::istreambuf_iterator<char>(userFile), std::istreambuf_iterator<char>());
    }
    else
    {
        std::cout << "(Emulator) File not found!" << std::endl;
        return 0;
    }
    userFile.close();

    // Pointer is defined here in order to move this object to the heap. This class alone allocates ~65k of mem, because of ram.hpp
    CPU* c = new CPU();

    c->getMemory()->writeBuffer(userBinary);

    time_t timer = time(0);
    time_t nextStep = time(0) + 1;
    while (c->isRunning())
    {
        if (difftime(timer, nextStep) > 0)
        {
            c->step();
            printCpuInfo(c->getCpuInfo());

            nextStep = time(0) + 1;
        }
        timer = time(0);
    }

    delete c;
    return 0;
}