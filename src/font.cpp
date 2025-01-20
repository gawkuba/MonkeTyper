#include "../include/font.h"
#include <iostream>
#include <utility>

// constructor
Font::Font(std::string name, std::string fontPath)
    :name(std::move(name)), fontPath(std::move(fontPath)) {
}
// font loading
bool Font::load() {
    if (!sfFont.loadFromFile(fontPath)) {
        std::cout << "Failed to load font from '" << fontPath << "'";
        return false;
    }
    return true;
}
// font access
sf::Font &Font::getSfFont()  {
    return sfFont;
}