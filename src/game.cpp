#include "../include/game.h"

// Game::Game



#include <fstream>
#include <iostream>
#include <string>
#include <vector>

Game::Game(const std::string& wordsFilePath)
	: currentFontIndex(0)
{
	words = loadWordsFromFile(wordsFilePath);
	fonts = loadAllFonts();

	// Inicjalizacja licznika użycia fontów
	fontUsageCount.resize(fonts.size(), 0);

	initInactiveWords();

	std::cout << "[Game] Loaded " << words.size() << " words, " << fonts.size() << " fonts.\n";
}

auto Game::getNextFontIndex()-> int {

	// Przechodzimy maksymalnie przez 16 fontów (fonts.size()),
	// zaczynając od currentFontIndex. Jeśli znajdziemy wolny (<7),
	// to zwracamy go i przesuwamy currentFontIndex o 1 do przodu.
	//
	// Uwaga: pętla i = 0..fonts.size()-1, bo w najgorszym wypadku
	// musimy sprawdzić wszystkie fonty jeden po drugim.

	for (int i = 0; static_cast<int>(fonts.size()); ++i) {
		// sprawdzamy font o indeksie currentFontIndex
		int idx = currentFontIndex;

		// jeśli jeszcze nie osiągnął limitu 7 przydzielamy i wychodzimy
		if (fontUsageCount[idx] < 7) {
			fontUsageCount[idx]++; // zwiększamy licznik
			// przesuwamy currentFontIndex o 1 do przodu
			currentFontIndex = (currentFontIndex + 1) % fonts.size();
			return idx;
		}
		// przechodzimy do następnego fontu
		currentFontIndex = (currentFontIndex + 1) % fonts.size();
	}
	// Jeśli pętla się zakończyła (sprawdziliśmy wszystkie fonty) i nigdzie
	// nie było < 7, to znaczy brak wolnych fontów (wszystkie po 7 użyć).
	return -1;
}

// zwalnianie fontu
auto Game::releaseFont(int fontIndex) -> void {
	if (fontIndex >= 0 && fontIndex < static_cast<int>(fontUsageCount.size())) {
		fontUsageCount[fontIndex]--;
		if (fontUsageCount[fontIndex] < 0) {
			fontUsageCount[fontIndex] = 0;
		}
	}
}

// dostęp do fontu
auto Game::getFont(int index) -> Font& {
	return fonts[index];
}


// Funkcja do wczytywania słów z pliku
auto Game::loadWordsFromFile(const std::string& filePath) -> std::vector<std::string> {
    std::vector<std::string> words;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Cannot open words file: " << filePath << "\n";
        return words;  // zwróć pusty wektor, żeby uniknąć crasha
    }

    std::string line;
    while (std::getline(file, line)) {
        // Pomijamy puste linie
        if (!line.empty()) {
            words.push_back(line);
        }
    }

    return words;
}

// wczyywanie wszystkich fontów
auto Game::loadAllFonts() -> std::vector<Font>{

    std::vector<Font> fonts;
    fonts.reserve(16);  // wiemy, że będzie 16 fontów

    // Utwórz obiekty Font
    Font openSans("OpenSans", "../assets/fonts/OpenSans.ttf");
    Font antonsc("AntonSC", "../assets/fonts/AntonSC.ttf");
    Font orangeKid("OrangeKid", "../assets/fonts/OrangeKid.ttf");
    Font caveatsb("CaveatSB", "../assets/fonts/CaveatSB.ttf");
    Font cbyg("CBYG", "../assets/fonts/CBYG.ttf");
    Font ebgaramond("EBGaramond", "../assets/fonts/EBGaramond.ttf");
    Font edu("Edu", "../assets/fonts/Edu.ttf");
    Font indieFlower("IndieFlower", "../assets/fonts/IndieFlower.ttf");
    Font lobster("Lobster", "../assets/fonts/Lobster.ttf");
    Font mali("Mali", "../assets/fonts/Mali.ttf");
    Font manslava("Mansalva", "../assets/fonts/Mansalva.ttf");
    Font oswald("Oswald", "../assets/fonts/Oswald.ttf");
    Font pacifico("Pacifico", "../assets/fonts/Pacifico.ttf");
    Font permanentMarker("PermanentMarker", "../assets/fonts/PermanentMarker.ttf");
    Font roboto("Roboto", "../assets/fonts/Roboto.ttf");
    Font yujiMai("YujiMai", "../assets/fonts/YujiMai.ttf");

    // Załaduj pliki .ttf i sprawdź ewentualne błędy
    if (!openSans.load())          { std::cerr << "Failed to load OpenSans\n"; }
    if (!antonsc.load())           { std::cerr << "Failed to load AntonSC\n"; }
    if (!orangeKid.load())         { std::cerr << "Failed to load OrangeKid\n"; }
    if (!caveatsb.load())          { std::cerr << "Failed to load CaveatSB\n"; }
    if (!cbyg.load())              { std::cerr << "Failed to load CBYG\n"; }
    if (!ebgaramond.load())        { std::cerr << "Failed to load EBGaramond\n"; }
    if (!edu.load())               { std::cerr << "Failed to load Edu\n"; }
    if (!indieFlower.load())       { std::cerr << "Failed to load IndieFlower\n"; }
    if (!lobster.load())           { std::cerr << "Failed to load Lobster\n"; }
    if (!mali.load())              { std::cerr << "Failed to load Mali\n"; }
    if (!manslava.load())          { std::cerr << "Failed to load Manslava\n"; }
    if (!oswald.load())            { std::cerr << "Failed to load Oswald\n"; }
    if (!pacifico.load())          { std::cerr << "Failed to load Pacifico\n"; }
    if (!permanentMarker.load())   { std::cerr << "Failed to load PermanentMarker\n"; }
    if (!roboto.load())            { std::cerr << "Failed to load Roboto\n"; }
    if (!yujiMai.load())           { std::cerr << "Failed to load YujiMai\n"; }

    // Dopiero po załadowaniu (nawet jeśli któryś font się nie wczyta)
    // wrzucamy je do wektora:
    fonts.push_back(openSans);
    fonts.push_back(antonsc);
    fonts.push_back(orangeKid);
    fonts.push_back(caveatsb);
    fonts.push_back(cbyg);
    fonts.push_back(ebgaramond);
    fonts.push_back(edu);
    fonts.push_back(indieFlower);
    fonts.push_back(lobster);
    fonts.push_back(mali);
    fonts.push_back(manslava);
    fonts.push_back(oswald);
    fonts.push_back(pacifico);
    fonts.push_back(permanentMarker);
    fonts.push_back(roboto);
    fonts.push_back(yujiMai);

    return fonts;
}

auto Game::GameWord::update(float dt)-> void {
	auto pos = sfText.getPosition();
	pos.x += speedX * dt;
	sfText.setPosition(pos);
}

// sprawdzanie słów z ekranu
auto Game::checkWordOnScreen(const std::string &typedWord, std::vector<GameWord> &activeWords, Game &game) -> void {
	for (auto &gw: activeWords) {
		if (typedWord == gw.originalString) {
			gw.isAlive = false;
			game.releaseFont(gw.fontIndex);
#ifdef DEBUG_WORDS
			std::cout << "[Game::checkWordOnScreen] Word '"
					  << gw.originalString
					  << "' matched -> setting isAlive=false, releasing font\n";
#endif
		}
	}
}

auto Game::initInactiveWords() -> void {
	inActiveWords.clear();
	inActiveWords.reserve(words.size());

	for (const auto &w: words) {
		// tworzymy obiekt GameWord jako nieaktywny
		GameWord gw;
		gw.originalString = w;
		gw.isAlive = false;
		gw.fontIndex = -1;
		gw.speedX = 0.f;
		// sfText.setFont() nie ustawiamy bo potrzebuje index getNExtFontIndex()
		inActiveWords.push_back(gw);
	}
}
auto Game::spawnWord() -> void {
	// Sprawdź, czy mamy jakiekolwiek nieaktywne słowo
	if (inActiveWords.empty()) {
		std::cout << "All words are active!\n";
		return; // <-- KONIEC, nie wywołujemy pop_back()!
	}

	// Bierzemy ostatni element z inActiveWords
	GameWord gw = inActiveWords.back();
	inActiveWords.pop_back();

	// pobieramy index fontu
	int idx = getNextFontIndex();
	if (idx == -1) {
		std::cout << "No more fonts available!\n";
		inActiveWords.push_back(gw);
		return; // <-- KONIEC, nie ustawiamy gw.fontIndex = -1 itp.
	}

	// Ustawiamy parametry GameWord
	gw.fontIndex = idx;
	gw.sfText.setFont(fonts[idx].getSfFont());
	gw.sfText.setCharacterSize(80);
	gw.sfText.setFillColor(sf::Color::Green);
	gw.sfText.setPosition(100, 400);  // poza ekranem
	gw.isAlive = true;
	gw.speedX = 70.f;

	// dodajemy do aktywnych
	activeWords.push_back(gw);

#ifdef DEBUG_WORDS
	std::cout << "[Game::spawnWord] Spawned word:\n"
			  << gw.debugInfo() << "\n";
#endif
}

