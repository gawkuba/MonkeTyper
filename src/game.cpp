#include "../include/game.h"
#include "window.h"
#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

Game::Game(const std::string& wordsFilePath)
	: currentFontIndex(0)
{
	words = loadWordsFromFile(wordsFilePath);
	fonts = loadAllFonts();

	// inicjalizacja licznika użycia fontów
	fontUsageCount.resize(fonts.size(), 0);

	// bardzo wazne raz zapomnialem
	initInactiveWords();
}

auto Game::getNextFontIndex()-> int {
	// pomysł miałem taki że biere pierwszy font od currentFontIndex i sprawdzam czy ma mniej niż 7 użyc
	// bo 112 / 16 = 7 i potem przechodze do nastepnego fontu
	// tez fajnie jakby nie bylo 7 słow z jednym fontem a potem 7 z nastepnym
	// więc zrobiłem taką pętlę
	for (int i = 0; static_cast<int>(fonts.size()); ++i) {
		if (const int idx = currentFontIndex; fontUsageCount[idx] < 7) {
			fontUsageCount[idx]++;
			currentFontIndex = (currentFontIndex + 1) % static_cast<int>(fonts.size());
			return idx;
		}
		currentFontIndex = (currentFontIndex + 1) % static_cast<int>(fonts.size());
	}
	// przez ustwienie limitu słów na 16 nie powinno być sytuacji, że nie ma fontu
	// ale dla pewności zwracamy -1
	return -1;
}

// zwalnianie fontu
auto Game::releaseFont(const int fontIndex) -> void {
	if (fontIndex >= 0 && fontIndex < static_cast<int>(fontUsageCount.size())) {
		fontUsageCount[fontIndex]--;
		if (fontUsageCount[fontIndex] < 0) {
			fontUsageCount[fontIndex] = 0;
		}
	}
}

// dostęp do fontu
auto Game::getFont(const int index) -> Font& {
	return fonts[index];
}


// funkcja do wczytywania słów z pliku
auto Game::loadWordsFromFile(const std::string& filePath) -> std::vector<std::string> {
    std::vector<std::string> words;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Cannot open words file: " << filePath << "\n";
        return words;  // zwracam pusty wektor, żeby uniknąć crasha , zdarzylo sie
    }

    std::string line;
    while (std::getline(file, line)) {
        // pomijam puste linie
        if (!line.empty()) {
            words.push_back(line);
        }
    }

    return words;
}

// wczyywanie wszystkich fontów
auto Game::loadAllFonts() -> std::vector<Font>{

    std::vector<Font> fonts;
    fonts.reserve(16);  // wiemy, że jest aż 16 fontów

    // tworzymy obiekty Font
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

    // ładujemy pliki .ttf i sprawdzamy ewentualne błędy
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

    // po załadowaniu (nawet jeśli któryś font się nie wczyta)
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

    // zwracam wektor fontów
    return fonts;
}

// aktualizacja słowa
auto Game::GameWord::update(const float dt)-> void {
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
		}
	}
}

// inicjalizuje nieaktywne słowa
auto Game::initInactiveWords() -> void {
	// czyszczenie jak coś było
	inActiveWords.clear();

	for (const auto &w: words) {
		// tworzymy obiekt GameWord jako nieaktywny
		GameWord gw;
		gw.originalString = w;
		gw.isAlive = false;
		gw.fontIndex = -1;
		gw.speedX = 0.f;
		// dodajemy do nieaktywnych
		inActiveWords.push_back(gw);
	}
}
// pojawiają sie słowa
auto Game::spawnWord() -> void {
    // max 16 aktywnych słów
    if (activeWords.size() >= 16) {
        std::cout << "Max 16 active words!\n";
        return;
    }

    // brak nieaktywnych słów
    if (inActiveWords.empty()) {
        std::cout << "All words are active!\n";
        return;
    }

    // losowanie słowa z nieaktywnych
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // wektor indeksów "kandydatów"
    std::vector<size_t> candidates;
    candidates.reserve(inActiveWords.size());
    for (size_t i = 0; i < inActiveWords.size(); i++) {
        candidates.push_back(i);
    }
    // tasujemy, super jest to shuffle
    std::ranges::shuffle(candidates, gen);

    // biore pierwszy po przetasowaniu
	const size_t idx = candidates.front();

	// pobieram słowo
    GameWord gw = inActiveWords[idx];
	// usuwam z nieaktywnych
    inActiveWords.erase(inActiveWords.begin() + static_cast<std::deque<GameWord>::difference_type>(idx));

    // pobieramy index fontu
    const int fontIdx = getNextFontIndex();
    if (fontIdx == -1) {
        std::cout << "No more fonts available!\n";
        inActiveWords.push_front(gw);
        return;
    }

    // ustawiam parametry GameWord
    gw.fontIndex = fontIdx;
    gw.sfText.setFont(fonts[fontIdx].getSfFont());
    gw.sfText.setCharacterSize(20);
    gw.sfText.setString(gw.originalString);
    gw.sfText.setFillColor(sf::Color::Green);
    gw.isAlive = true;

    // prędkość zależna od długości
    const auto velDiff = static_cast<float>(gw.originalString.size()) * 1.15f;

    gw.speedX = 75.f - velDiff; // dłuższe słowo = wolniejsze

    // trzeba ustawić pozycję y tak, żeby nie kolidowała z innymi słowami więc
	// używam `std::uniform_real_distribution`
	// generuje on sobie jakas tam liczbe zmienno przecinkowa z danego przedzialu
	// tutaj od 100 do 550 czyli pole gry
	// a potem sprawdzam czy nie ma kolizji z innymi słowami
    std::uniform_real_distribution<float> distY(100.f, 550.f);
    bool foundY = false;
    float spawnY{};
	constexpr int maxTries = 20;
    for (int i = 0; i < maxTries; i++) {
		const float candidateY = distY(gen);
        // sprawdzam kolizję z innymi aktywnymi słowami
        bool collision = false;
        for (auto &aw : activeWords) {
			// jeżeli mniejsza niż 15 to bum
            if (const float dy = std::fabs(aw.sfText.getPosition().y - candidateY); dy < 15.f) {
                collision = true;
                break;
            }
        }
		// jeżeli nie ma kolizji to ustawiam
        if (!collision) {
            spawnY = candidateY;
            foundY = true;
            break;
        }
    }
    if (!foundY) {
        // jesli nie udało się znaleźć wolnego miejsca
        // zrezygnuje ze spawnu
        std::cout << "Cannot spawn due to Y-collision after 20 tries\n";
        inActiveWords.push_front(gw); // oddaj
        return;
    }

    // finalnie ustawiam pozycję x i y słowa
    gw.sfText.setPosition(-100.f, spawnY);

    // dodaje do aktywnych
    activeWords.push_back(gw);
}