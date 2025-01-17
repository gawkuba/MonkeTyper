//
// Created by kubag on 16.01.2025.
//
#pragma once


#include <string>
#include <vector>
#include <deque>
#include <sstream>
#include <iostream>
#include "font.h"

#define DEBUG_WORDS
#ifndef GAME_H
#define GAME_H

/*
	Klasa Game przechowuje:
	- wektor wszystkich słów (words),
	- wektor wszystkich fontów (fonts),
	- licznik użycia każdego fontu (fontUsageCount),
	- ewentualnie inne pola (np. generator liczb losowych, jeśli używasz).
*/

class Game {
public:
	explicit Game(const std::string& wordsFilePath);

#ifdef DEBUG_WORDS
 // do tworzenia stringów
#endif
	struct GameWord {
		std::string originalString;
		sf::Text sfText;
		bool isAlive;
		int fontIndex;
		float speedX;

#ifdef DEBUG_WORDS
		// Metoda zwracająca opis obiektu w postaci stringa
		std::string debugInfo() const {
			std::ostringstream oss;
			// Pobierz kolor
			sf::Color c = sfText.getFillColor();
			// Pobierz pozycję
			sf::Vector2f pos = sfText.getPosition();

			oss << "[GameWord Debug] "
				<< "Text=\"" << originalString << "\" "
				<< "fontIndex=" << fontIndex << " "
				<< "color=(" << static_cast<int>(c.r) << ","
							 << static_cast<int>(c.g) << ","
							 << static_cast<int>(c.b) << ","
							 << static_cast<int>(c.a) << ") "
				<< "pos=(" << pos.x << "," << pos.y << ") "
				<< "speedX=" << speedX << " "
				<< "isAlive=" << (isAlive ? "true" : "false");
			return oss.str();
		}
#endif


		// konstruktor bezparametrowy
		GameWord()
			: isAlive(false),
			  fontIndex(-1),
			  speedX(0.f) {}

		// konstruktor z parametrami
		GameWord(const std::string& word,
				 const sf::Font& font, const int fontIndex, const float speed)
				:originalString(word),
				 isAlive(true),
				 fontIndex(fontIndex),
				 speedX(speed)
		{
			sfText.setString(word);
			sfText.setFont(font);
			sfText.setCharacterSize(20);
			sfText.setFillColor(sf::Color::Green);
		}

		auto update(float dt)-> void;
	};
	// Zwraca indeks fontu, który można użyć (limit < 7),
	// w kolejności "round-robin".
	// Zwraca -1, jeśli brak dostępnego fontu.
	auto getNextFontIndex() -> int;

	// zwalnia font (decrementuje licznik użycia)
	auto releaseFont(int fontIndex) -> void;

	// dostęp do fotu
	auto getFont(int index) -> Font&;

	// Funkcja do wczytywania słów z pliku
	static auto loadWordsFromFile(const std::string &filePath) -> std::vector<std::string>;
	// Funkcja do wczytywania 16 fontów
	static auto loadAllFonts() -> std::vector<Font>;

	static auto checkWordOnScreen(const std::string& typedWord, std::vector<GameWord>& activeWords, Game &game) -> void;

	// Metody do inicjalizacji i spawnowania słów (aktywnych/nieaktywnych)
	auto initInactiveWords() -> void;
	auto spawnWord() -> void;

	std::vector<std::string> words;
	std::deque<GameWord> inActiveWords;  // słowa gotowe do przerzucenia na ekran
	std::vector<GameWord> activeWords;

private:

	std::vector<Font> fonts;
	std::vector<int> fontUsageCount; // licznik użycia każdego fontu max 7

	// indeks fontu od którego zaczynamy przy kolejnym zapytaniu
	int currentFontIndex;
};

#endif // GAME_H
