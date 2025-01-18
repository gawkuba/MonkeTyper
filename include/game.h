#ifndef GAME_H
#define GAME_H
#pragma once

#include <string>
#include <vector>
#include <deque>
#include "font.h"



class Game {
public:
	// konstruktor explicit bo tylko jeden argument
	explicit Game(const std::string& wordsFilePath);

	// struktura słowa w grze
	struct GameWord {
		std::string originalString;
		sf::Text sfText;
		bool isAlive;
		int fontIndex;
		float speedX;

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
			// ustawiamy tekst
			sfText.setString(word);
			sfText.setFont(font);
			sfText.setCharacterSize(20);
			sfText.setFillColor(sf::Color::Green);
		}
		// funkcja do aktualizacji stanu słowa
		auto update(float dt)-> void;
	};

	// zwraca indeks fontu do użycia
	auto getNextFontIndex() -> int;

	// zwalnia font
	auto releaseFont(int fontIndex) -> void;

	// dostęp do fotu
	auto getFont(int index) -> Font&;

	// wcztywanie słów z pliku
	static auto loadWordsFromFile(const std::string &filePath) -> std::vector<std::string>;
	// wczytywanie fontów z folderu
	static auto loadAllFonts() -> std::vector<Font>;
	// sprawdzanie czy słowo jest na ekranie
	static auto checkWordOnScreen(const std::string& typedWord, std::vector<GameWord>& activeWords, Game &game) -> void;

	// inicjalizacja nieaktywnych słów
	auto initInactiveWords() -> void;
	// pojawianie się słów
	auto spawnWord() -> void;

	std::vector<std::string> words;      // wszystkie słowa
	std::deque<GameWord> inActiveWords;  // słowa gotowe do przerzucenia na ekran
	std::vector<GameWord> activeWords;  // słowa na ekranie

private:
	// fonty
	std::vector<Font> fonts;
	// licznik użycia fontów
	std::vector<int> fontUsageCount;
	// indeks fontu od którego zaczynamy przy kolejnym zapytaniu
	int currentFontIndex;
};
#endif // GAME_H
