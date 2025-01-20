#ifndef GAME_H
#define GAME_H
#pragma once

#include <string>
#include <vector>
#include <deque>
#include "font.h"



class Game {
public:
	// explicit constructor
	explicit Game(const std::string& wordsFilePath);

	// game word struct
	struct GameWord {
		std::string originalString;
		sf::Text sfText;
		bool isAlive;
		int fontIndex;
		float speedX;

		// default constructor
		GameWord()
			: isAlive(false),
			  fontIndex(-1),
			  speedX(0.f) {}

		// custom constructor
		GameWord(const std::string& word,
				 const sf::Font& font, const int fontIndex, const float speed)
				:originalString(word),
				 isAlive(true),
				 fontIndex(fontIndex),
				 speedX(speed)
		{
			// text settings
			sfText.setString(word);
			sfText.setFont(font);
			sfText.setCharacterSize(20);
			sfText.setFillColor(sf::Color::Green);
		}
		// update function
		auto update(float dt)-> void;
	};

	// return next font index
	auto getNextFontIndex() -> int;

	// freeing the font
	auto releaseFont(int fontIndex) -> void;

	// access to font
	auto getFont(int index) -> Font&;

	// loading words from file
	static auto loadWordsFromFile(const std::string &filePath) -> std::vector<std::string>;
	// loading all fonts from folder
	static auto loadAllFonts() -> std::vector<Font>;
	// checking word on screen
	static auto checkWordOnScreen(const std::string& typedWord, std::vector<GameWord>& activeWords, Game &game) -> void;

	// initialize inactive words
	auto initInactiveWords() -> void;
	// word spawn
	auto spawnWord() -> void;

	std::vector<std::string> words;      // all words
	std::deque<GameWord> inActiveWords;  // words ready to be spawned
	std::vector<GameWord> activeWords;  // words on the screen

private:
	// fonts
	std::vector<Font> fonts;
	// counter for font usage
	std::vector<int> fontUsageCount;
	// font index
	int currentFontIndex;
};
#endif // GAME_H
