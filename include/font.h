#ifndef FONT_H
#define FONT_H

#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>

class  Font {
public:
    std::string name;
    std::string fontPath;
    sf::Font sfFont;


    Font(std::string name, std::string fontPath);

    bool load();
    sf::Font& getSfFont() ;
    std::string& getName() ;
};

#endif //FONT_H
