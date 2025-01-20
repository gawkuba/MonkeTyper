#ifndef FONT_H
#define FONT_H
#pragma once

#include <SFML/Graphics.hpp>
#include <string>

// Font class
class  Font {
public:
    std::string name;
    std::string fontPath;
    sf::Font sfFont;

	// constructor
    Font(std::string name, std::string fontPath);

	// public functions
    bool load();
    sf::Font& getSfFont() ;
};

#endif //FONT_H
