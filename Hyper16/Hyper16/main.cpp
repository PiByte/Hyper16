#include <iostream>
#include "cpu.hpp"

int main()
{
    CPU* c = new CPU();

    byte code[6] = { 0b01000000, 68, 0b01100000, 1, 0b01001000, 0 };

    c->getMemory()->writeBuffer(code, 6);

    c->step();
    c->step();
    c->step();

    std::cout << c->getMemory()->readByte(0x0000) << std::endl;

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