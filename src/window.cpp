#include "window.h"
#include <iostream>
#include "../include/game.h" // Funkcja loadWordsFromFile
#include "font.h" // Twój wrapper Font (OpenSans, AntonSC itd.)

// ------------------------------------------------------------------
// Funkcja wątku muzyki
// ------------------------------------------------------------------
auto loopThreadFn(sf::Music &loop, const std::atomic<bool> &running) -> void {
	if (running) {
		loop.setLoop(true);
		loop.play();
	}
}

// ------------------------------------------------------------------
// Konstruktor klasy Window
// ------------------------------------------------------------------
Window::Window(const int width, const int height, const std::string &title, const int frameRate) :
	width(width), height(height), title(title), frameRate(frameRate), running(true), isGameStarted(false) {
	// Tworzymy okno SFML – ale NIE wchodzimy tu w pętlę zdarzeń!
	window.create(sf::VideoMode(static_cast<int>(width), static_cast<int>(height)), title, sf::Style::Close);
	window.setFramerateLimit(frameRate);
}

// ------------------------------------------------------------------
// Destruktor klasy Window
// ------------------------------------------------------------------
Window::~Window() {
	// Upewniamy się, że wątek muzyki został zatrzymany.
	running = false;
	loop.stop();
	if (loopThread.joinable()) {
		loopThread.join();
	}
	std::cout << "Window destroyed" << std::endl;
}

// ------------------------------------------------------------------
// Metoda uruchamiająca główną pętlę zdarzeń i logikę gry
// ------------------------------------------------------------------
void Window::run() {


	// -----------------------------------------
	// 1. Wczytujemy listę słów
	// -----------------------------------------
	auto allWords = Game::loadWordsFromFile("../assets/words.txt");
	if (allWords.empty()) {
		std::cout << "No words loaded (file might be empty or missing)!\n";
		// Możesz tu obsłużyć sytuację braku słów np. przerwać grę
	} else {
		std::cout << "Loaded " << allWords.size() << " words from file.\n";
	}

	// Przykładowo wypisz pierwsze słowo (debug):
	if (!allWords.empty()) {
		std::cout << "Example word: " << allWords[0] << std::endl;
	}

	// 2. Wczytaj fonty
	auto fonts = Game::loadAllFonts();
	std::cout << "Loaded " << fonts.size() << " fonts.\n";

	//TODO: implementacja mechaniki gry (np. wyświetlanie słów, sprawdzanie poprawności wpisywanych słów)
	std::vector<std::string> activeWords;

	// fonty uzytkowe z okna
	Font antonsc = fonts[1];
	Font orangeKid = fonts[2];

	// 2. Tworzymy teksty
	std::string playerInput;
	sf::Text userInputText;
	userInputText.setFont(orangeKid.getSfFont());
	userInputText.setCharacterSize(30);
	userInputText.setFillColor(sf::Color::White);
	userInputText.setString("");

	sf::Text monkey("Monke Typer", antonsc.getSfFont(), 90);
	monkey.setFillColor(sf::Color::White);
	sf::Vector2f monkeyTextPosition(400.f, 250.f);
	monkey.setPosition(monkeyTextPosition);
	float xvel = 4.f;

	sf::Text pressToStart("PRESS MOUSE LEFT BUTTON TO START", orangeKid.getSfFont(), 30);
	pressToStart.setFillColor(sf::Color::White);
	pressToStart.setPosition(435.f, 360.f);

	bool isVisible = true;
	sf::Clock blinkClock;

	// 3. Ustawiamy ikonę okna
	if (sf::Image icon; !icon.loadFromFile("../assets/img/jp2.png")) {
		std::cout << "Failed to load icon ../assets/img/jp2.png\n";
	} else {
		window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	}

	// 4. Uruchamiamy muzykę w osobnym wątku
	if (!loop.openFromFile("../assets/audio/loop.ogg")) {
		std::cout << "Failed to open loop music ../assets/audio/loop.ogg\n";
	}
	loopThread = std::thread(loopThreadFn, std::ref(loop), std::ref(running));

	// 5. Ładujemy efekt dźwiękowy
	sf::SoundBuffer clickSoundBuffer;
	if (!clickSoundBuffer.loadFromFile("../assets/audio/click.ogg")) {
		std::cout << "Failed to load click sound ../assets/audio/click.ogg\n";
	}
	sf::Sound clickSound;
	clickSound.setBuffer(clickSoundBuffer);

	// 6. Główna pętla zdarzeń
	while (window.isOpen()) {
		sf::Event event{};
		while (window.pollEvent(event)) {
			// Zamknięcie okna (np. klik 'X')
			if (event.type == sf::Event::Closed) {
				window.close();
				running = false;
				loop.stop();
			}
			// Efekt dźwiękowy przy naciśnięciu klawisza
			if (event.type == sf::Event::KeyPressed) {
				clickSound.play();
			}
			// Wyjście klawiszem ESC
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				window.close();
				running = false;
				loop.stop();
			}
			// Wpisywanie tekstu
			if (event.type == sf::Event::TextEntered) {
				if (event.text.unicode == 8) {
					if (!playerInput.empty()) {
						playerInput.pop_back();
					}
				}
				else if (event.text.unicode == 13) {
					Game::checkWordOnScreen(playerInput, activeWords);
					playerInput.clear();
				}
				else if (event.text.unicode < 128) {
					char typed = static_cast<char>(event.text.unicode);
					if (std::isprint(typed)) {
						playerInput += typed;
					}
				}
			}
		}

		// Czyszczenie ekranu
		window.clear();

		// Jeśli gra jeszcze nie wystartowała, wyświetlamy "Monke Typer" i tekst "PRESS..."
		if (!isGameStarted) {
			window.draw(monkey);

			// Animacja pozioma "wahadło"
			if (monkeyTextPosition.x < 360.f || monkeyTextPosition.x > 440.f) {
				xvel = -xvel;
			}
			monkeyTextPosition.x += xvel;
			monkey.setPosition(monkeyTextPosition);

			// Miganie tekstu "PRESS..."
			if (blinkClock.getElapsedTime().asMilliseconds() >= 350) {
				isVisible = !isVisible;
				blinkClock.restart();
			}
			if (isVisible) {
				window.draw(pressToStart);
			}
		}

		// Kliknięcie myszy -> start gry
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			isGameStarted = true;
		}

		// Jeśli gra wystartowała, tu docelowo wrzucisz logikę rozgrywki
		if (isGameStarted) {
			// TODO: implementacja mechaniki pisania
			userInputText.setString(playerInput);
			float textWidth = userInputText.getLocalBounds().width;
			float xPos = (static_cast<float>(Window::width) - textWidth) / 2.0f;
			float yPos = static_cast<float>(Window::height) - 80.0f;
			userInputText.setPosition(xPos, yPos);
			window.draw(userInputText);

			// todo implementacja mechaniki przesuwania słów
		}

		// Prezentacja klatki
		window.display();
	}

	// 7. Po wyjściu z pętli okna dołącz wątek muzyki (jeśli jeszcze nie dołączony)
	if (loopThread.joinable()) {
		loopThread.join();
	}
}
