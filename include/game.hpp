#include <iostream>
#include <vector>
#include <fstream>

#include <cpu.hpp>
#include <SFML/Graphics.hpp>

class Game {
private:
	// SFML stuff
    sf::RenderWindow mWindow;
    sf::RectangleShape rectanglePixel;

    sf::RenderWindow debugWindow;
    CPU chip8;
public:
    Game();
    void initialize(std::string& fs, bool debugEnable);
    void run(void);
};

