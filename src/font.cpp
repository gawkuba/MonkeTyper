#include "../include/font.h"
#include <iostream>
#include <utility>

// konstruktor
Font::Font(std::string name, std::string fontPath)
    :name(std::move(name)), fontPath(std::move(fontPath)) {
}
// funkcja flaga
bool Font::load() {
    if (!sfFont.loadFromFile(fontPath)) {
        std::cout << "Failed to load font from '" << fontPath << "'";
        return false;
    }
    return true;
}
// dostęp do fontu
sf::Font &Font::getSfFont()  {
    return sfFont;
}