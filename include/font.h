#ifndef FONT_H
#define FONT_H
#pragma once

#include <SFML/Graphics.hpp>
#include <string>

// klasa fontu
class  Font {
public:
    std::string name;
    std::string fontPath;
    sf::Font sfFont;

	// konstruktor
    Font(std::string name, std::string fontPath);

	// publiczne metody
    bool load();
    sf::Font& getSfFont() ;
};

#endif //FONT_H
