//
// Created by kubag on 16.01.2025.
//

#ifndef GAME_H
#define GAME_H
#include <string>
#include <vector>
#include "font.h"

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

	// Zwraca indeks fontu, który można użyć (limit < 7),
	// w kolejności "round-robin".
	// Zwraca -1, jeśli brak dostępnego fontu.
	int getNextFontIndex();

	// zwalnia font (decrementuje licznik użycia)
	void releaseFont(int fontIndex);

	// dostęp do fotu
	Font& getFont(int index);

	// Funkcja do wczytywania słów z pliku
	static std::vector<std::string> loadWordsFromFile(const std::string &filePath);
	// Funkcja do wczytywania 16 fontów
	static std::vector<Font> loadAllFonts();

	static auto checkWordOnScreen(const std::string& typedWord, const std::vector<std::string>& activeWords) -> void;



private:
	std::vector<std::string> words;
	std::vector<Font> fonts;
	std::vector<int> fontUsageCount; // licznik użycia każdego fontu max 7

	// indeks fontu od którego zaczynamy przy kolejnym zapytaniu
	int currentFontIndex;
};

#endif // GAME_H
