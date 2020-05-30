#include <iostream>
#include <vector>
#include <fstream>

#include "types.hpp"
#include "cpu.hpp"

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "(Emulator) Please specify binary to run!" << std::endl;
        return 0;
    }

    std::fstream inputFile(argv[1], std::fstream::in);
    std::vector<byte> binary;

    if (inputFile.is_open())
    {
        binary = std::vector<byte>(std::istreambuf_iterator<char>(inputFile), std::istreambuf_iterator<char>());
    }
    else
    {
        std::cout << "(Emulator) File not found!" << std::endl;
        return 0;
    }
    inputFile.close();
    
    CPU* c = new CPU(); // Pointer is defined here in order to move this object to the heap. This class alone allocates ~65k of mem, because of ram.hpp
    c->getMemory()->writeBuffer(binary);

    while (c->isRunning())
    {
        c->step();
    }

    delete c;

    return 0;
}

/*
#include <SFML/Graphics.hpp>

int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }

    return 0;
}*/