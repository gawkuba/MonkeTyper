#include "../include/font.h"
#include <iostream>
#include <utility>


Font::Font(std::string name, std::string fontPath)
    :name(std::move(name)), fontPath(std::move(fontPath)) {
}

bool Font::load() {
    if (!sfFont.loadFromFile(fontPath)) {
        std::cout << "Failed to load font from '" << fontPath << "'";
        return false;
    }
    return true;
}

sf::Font &Font::getSfFont()  {
    return sfFont;
}

std::string &Font::getName()  {
    return name;
}