#pragma once

#include <SFML/Graphics.hpp>

#include "types.hpp"
#include "ram.hpp"

#define SCREEN_ADDRESS 0xA000
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 120

class Graphics
{
public:
	Graphics()
	{
		mainWindow.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Hyper8");

		mainImageBuffer.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	}

	void update(RAM* mem)
	{
		mainWindow.clear();

		for (int y = 0; y < SCREEN_HEIGHT; y++)
		{
			for (int x = 0; x < SCREEN_WIDTH / 8; x++)
			{
				byte pixels = mem->readByte(SCREEN_ADDRESS + y * (SCREEN_WIDTH / 8) + x);
				
				for (int i = 0; i < 8; i++)
				{
					mainImageBuffer.setPixel(x*8 + i, y, ((pixels >> i) & 1) ? sf::Color(0xFFFFFFFF) : sf::Color(0x000000FF));
				}
			}
		}

		mainTexture.loadFromImage(mainImageBuffer);
		mainSprite.setTexture(mainTexture);
		mainWindow.draw(mainSprite);

		mainWindow.display();
	}

	sf::RenderWindow* getWindow() { return &mainWindow; }

private:
	sf::RenderWindow mainWindow;
	sf::Image mainImageBuffer;
	sf::Texture mainTexture;
	sf::Sprite mainSprite;

};