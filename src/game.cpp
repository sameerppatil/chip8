#include <game.hpp>

#include <direct.h>
#define GetCurrentDir _getcwd

std::float_t scalingFactor = 16.f;

Game::Game()
: mWindow(sf::VideoMode(static_cast<std::uint32_t>(64 * scalingFactor), static_cast<std::uint32_t>(32 * scalingFactor)), "Chip8-Emulator", sf::Style::Close),
rectanglePixel(sf::Vector2f(scalingFactor, scalingFactor)),
debugWindow(sf::VideoMode(static_cast<std::uint32_t>(60 * scalingFactor), static_cast<std::uint32_t>(60 * scalingFactor)), "DebugWindow", sf::Style::Close) {
    
}

void Game::initialize(std::string& fs, bool debugEnable) {
    mWindow.setFramerateLimit(5);
    // create SFML window for display
    chip8.loadMemory(fs);

    char buff[FILENAME_MAX]; //create string buffer to hold path
   	GetCurrentDir( buff, FILENAME_MAX );
	std::cout << "current cwd is " << std::string(buff) << std::endl;
    
    chip8.initialize();

    
    if(!debugEnable)
        debugWindow.close();

    mWindow.display();
}
void Game::run(void) {
    
    while(mWindow.isOpen()) {
        sf::Event event;
        
		while (mWindow.pollEvent(event))
		{
            chip8.emulateCycle();
			if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
				std::cout << __func__ << ": Keypressed or event closed" << std::endl;\
                mWindow.close();
            }
            
            else {
                if(chip8.screen_update == true) {
                    for(uint32_t i = 0; i < 32; i++) {
                        for(uint32_t j = 0; j < 64; j++) {
                            uint32_t pixIndex = (i * 64) + j;
                            if(chip8.display[pixIndex] == 1) {
                                rectanglePixel.setPosition(scalingFactor * j, scalingFactor * i);
                                mWindow.draw(rectanglePixel);
                                // std::cout << __func__ << "-debug1: setting rectanglePixel at i: " << i << ", j " << j <<
                                    // ", piindex: " << pixIndex << std::endl;
                            }
                            
                        }
                    }
                    chip8.screen_update = false;
                    mWindow.display();
                }
            }                
		}
    }
}
