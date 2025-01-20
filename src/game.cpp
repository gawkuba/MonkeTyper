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

	// Initialize font usage counters
	fontUsageCount.resize(fonts.size(), 0);

	// Initialize inactive words
	initInactiveWords();
}

auto Game::getNextFontIndex() -> int {
	// Retrieve the next font with usage less than 7 times
	// This ensures balanced distribution across fonts
	for (int i = 0; static_cast<int>(fonts.size()); ++i) {
		if (const int idx = currentFontIndex; fontUsageCount[idx] < 7) {
			fontUsageCount[idx]++;
			currentFontIndex = (currentFontIndex + 1) % static_cast<int>(fonts.size());
			return idx;
		}
		currentFontIndex = (currentFontIndex + 1) % static_cast<int>(fonts.size());
	}
	// With a limit of 16 words, there shouldn't be a situation where no font is available,
	// but returning -1 as a safeguard.
	return -1;
}

// Decrement font usage counter
auto Game::releaseFont(const int fontIndex) -> void {
	if (fontIndex >= 0 && fontIndex < static_cast<int>(fontUsageCount.size())) {
		fontUsageCount[fontIndex]--;
		if (fontUsageCount[fontIndex] < 0) {
			fontUsageCount[fontIndex] = 0;
		}
	}
}

// Access font by index
auto Game::getFont(const int index) -> Font& {
	return fonts[index];
}

// Function to load words from a file
auto Game::loadWordsFromFile(const std::string& filePath) -> std::vector<std::string> {
    std::vector<std::string> words;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Cannot open words file: " << filePath << "\n";
        return words;  // Return empty vector to avoid crashes
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines
        if (!line.empty()) {
            words.push_back(line);
        }
    }

    return words;
}

// Load all fonts into the program
auto Game::loadAllFonts() -> std::vector<Font> {
    std::vector<Font> fonts;
    fonts.reserve(16);  // There are exactly 16 fonts

    // Create font objects
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

    // Attempt to load each font file and log errors if loading fails
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
    if (!manslava.load())          { std::cerr << "Failed to load Mansalva\n"; }
    if (!oswald.load())            { std::cerr << "Failed to load Oswald\n"; }
    if (!pacifico.load())          { std::cerr << "Failed to load Pacifico\n"; }
    if (!permanentMarker.load())   { std::cerr << "Failed to load PermanentMarker\n"; }
    if (!roboto.load())            { std::cerr << "Failed to load Roboto\n"; }
    if (!yujiMai.load())           { std::cerr << "Failed to load YujiMai\n"; }

    // Add fonts to the vector even if some failed to load
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

    // Return the vector of fonts
    return fonts;
}

// Update word position
auto Game::GameWord::update(const float dt) -> void {
	auto pos = sfText.getPosition();
	pos.x += speedX * dt;
	sfText.setPosition(pos);
}

// Check if the typed word matches any active word
auto Game::checkWordOnScreen(const std::string &typedWord, std::vector<GameWord> &activeWords, Game &game) -> void {
	for (auto &gw : activeWords) {
		if (typedWord == gw.originalString) {
			gw.isAlive = false;
			game.releaseFont(gw.fontIndex);
		}
	}
}

// Initialize inactive words list
auto Game::initInactiveWords() -> void {
	// Clear any existing data
	inActiveWords.clear();

	for (const auto &w : words) {
		// Create GameWord objects and set them as inactive
		GameWord gw;
		gw.originalString = w;
		gw.isAlive = false;
		gw.fontIndex = -1;
		gw.speedX = 0.f;
		inActiveWords.push_back(gw);
	}
}

// Spawn a new word on the screen
auto Game::spawnWord() -> void {
    // Limit the number of active words to 16
    if (activeWords.size() >= 16) {
        std::cout << "Max 16 active words!\n";
        return;
    }

    // No inactive words available
    if (inActiveWords.empty()) {
        std::cout << "All words are active!\n";
        return;
    }

    // Randomly select a word from the inactive words
    static std::random_device rd;
    static std::mt19937 gen(rd());

    // Prepare candidate indices and shuffle
    std::vector<size_t> candidates;
    candidates.reserve(inActiveWords.size());
    for (size_t i = 0; i < inActiveWords.size(); i++) {
        candidates.push_back(i);
    }
    std::ranges::shuffle(candidates, gen);

    // Select the first candidate
	const size_t idx = candidates.front();

	// Retrieve the word and remove it from inactive list
    GameWord gw = inActiveWords[idx];
	inActiveWords.erase(inActiveWords.begin() + static_cast<std::deque<GameWord>::difference_type>(idx));

    // Get the next font index
    const int fontIdx = getNextFontIndex();
    if (fontIdx == -1) {
        std::cout << "No more fonts available!\n";
        inActiveWords.push_front(gw);
        return;
    }

    // Set parameters for the word
    gw.fontIndex = fontIdx;
    gw.sfText.setFont(fonts[fontIdx].getSfFont());
    gw.sfText.setCharacterSize(20);
    gw.sfText.setString(gw.originalString);
    gw.sfText.setFillColor(sf::Color::Green);
    gw.isAlive = true;

    // Adjust speed based on word length
    const auto velDiff = static_cast<float>(gw.originalString.size()) * 1.15f;

    gw.speedX = 75.f - velDiff; // Longer words move slower

    // Generate a Y-position that avoids collision with other words
    std::uniform_real_distribution<float> distY(100.f, 550.f);
    bool foundY = false;
    float spawnY{};
	constexpr int maxTries = 20;
    for (int i = 0; i < maxTries; i++) {
		const float candidateY = distY(gen);
        // Check for collisions with other active words
        bool collision = false;
        for (auto &aw : activeWords) {
            if (const float dy = std::fabs(aw.sfText.getPosition().y - candidateY); dy < 15.f) {
                collision = true;
                break;
            }
        }
        if (!collision) {
            spawnY = candidateY;
            foundY = true;
            break;
        }
    }
    if (!foundY) {
        // If no valid Y-position is found after max tries
        std::cout << "Cannot spawn due to Y-collision after 20 tries\n";
        inActiveWords.push_front(gw); // Return word to inactive list
        return;
    }

    // Set final X and Y positions for the word
    gw.sfText.setPosition(-100.f, spawnY);

    // Add the word to the list of active words
    activeWords.push_back(gw);
}
