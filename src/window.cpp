#include "window.h"
#include <iostream>
#include "../include/game.h" // Funkcja loadWordsFromFile
#include "font.h" // Twój wrapper Font (OpenSans, AntonSC itd.)
#include <algorithm>

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
	window.create(sf::VideoMode(width, height), title, sf::Style::Close);
	window.setFramerateLimit(frameRate);

	std::cout << "Created window: "
		  << window.getSize().x << "x"
		  << window.getSize().y << "\n";

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
auto Window::run() -> void {
	// Tworzymy obiekt gry
	Game game("../assets/words.txt");

	int score = 0;
	int lives = 6;
	bool isPaused = false;




	// 2. Wczytaj fonty
	auto loadedFonts = Game::loadAllFonts();
	std::cout << "Loaded " << loadedFonts.size() << " fonts.\n";
	// fonty uzytkowe z okna
	Font antonsc = loadedFonts[1];
	Font orangeKid = loadedFonts[2];

	// Tekst wpisywany przez gracza
	sf::Text userInputText;
	userInputText.setFont(orangeKid.getSfFont());
	userInputText.setCharacterSize(30);
	userInputText.setFillColor(sf::Color::White);

	// Tekst z wynikiem
	sf::Text scoreText;
	scoreText.setFont(orangeKid.getSfFont());
	scoreText.setCharacterSize(20);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setPosition(20.f, 20.f);

	// Tekst z ilością żyć
	sf::Text livesText;
	livesText.setFont(orangeKid.getSfFont());
	livesText.setCharacterSize(20);
	livesText.setFillColor(sf::Color::White);
	livesText.setPosition(20.f, 50.f);

	// Tekst "PAUSED"
	sf::Text pauseText;
	pauseText.setFont(orangeKid.getSfFont());
	pauseText.setCharacterSize(40);
	pauseText.setString("PAUSED");

	sf::Text monkey("Monke Typer", antonsc.getSfFont(), 90);
	monkey.setFillColor(sf::Color::White);
	sf::Vector2f monkeyTextPosition(400.f, 250.f);
	monkey.setPosition(monkeyTextPosition);
	float xvel = 4.f;

	sf::Text pressToStart("PRESS MOUSE LEFT BUTTON TO START", orangeKid.getSfFont(), 30);
	pressToStart.setFillColor(sf::Color::White);
	pressToStart.setPosition(435.f, 360.f);
	bool isVisible = true;

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

	// 4) Zegary do mierzenia czasu
	sf::Clock clock;       // do delta time
	sf::Clock spawnClock;  // jak często spawnujemy nowe słowo
	sf::Clock blinkClock; // miganie napisu "PRESS..."

	// 6. Główna pętla zdarzeń
	while (window.isOpen()) {
		// delta time
		float dt = clock.restart().asSeconds();

		// obsługa zdarzeń
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
			// Pauza
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && sf::Keyboard::isKeyPressed(sf::Keyboard::M)) {
				isPaused = !isPaused;
			}

			// Wpisywanie tekstu
			if (event.type == sf::Event::TextEntered && !isPaused) {
				if (event.text.unicode == 8) {
					if (!playerInput.empty()) {
						playerInput.pop_back();
					}
				}
				else if (event.text.unicode == 13) {
					Game::checkWordOnScreen(playerInput, game.activeWords, game);
					for (auto &gw: game.activeWords) {
						if (!gw.isAlive) {
							score++;
						}
					}
					playerInput.clear();
				}
				else if (event.text.unicode < 128) {
					char typed = static_cast<char>(event.text.unicode);
					if (std::isprint(typed)) {
						playerInput += typed;
					}
				}
			}
		} // koniec poll event

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
		if (isGameStarted && !isPaused) {
			if (spawnClock.getElapsedTime().asSeconds() >= 1.5f) {
				game.spawnWord();
				spawnClock.restart();
			}

			// Aktualizacja słów na ekranie
			for (auto &gw : game.activeWords) {
				gw.update(dt); // aktualizacja pozycji

				// zmiana koloru w zależności od pozycji
				float x = gw.sfText.getPosition().x;
				float ww = static_cast<float>(Window::width);

				if (x < ww * 0.5f) {
					gw.sfText.setFillColor(sf::Color::Green);
				} else if (x < ww * 0.75f) {
					gw.sfText.setFillColor(sf::Color::Yellow);
				} else {
					gw.sfText.setFillColor(sf::Color::Red);
				}

				// jak słowo wyjdzie za ekran to tracimy życie
				if (x > ww) {
					gw.isAlive = false;
					lives--;
				}
			}
			// recykling martwych słów
			// wraca do puli nieaktywnych i zwalnia font
			game.activeWords.erase(
	std::remove_if(
		game.activeWords.begin(),
		game.activeWords.end(),
		[&](Game::GameWord &w) {
			if (!w.isAlive) {
				// zwalniamy font
				game.releaseFont(w.fontIndex);
				w.fontIndex = -1;
				// wrzucamy do inActiveWords
				w.isAlive = false;
				game.inActiveWords.push_back(w);
				return true;
			}
			return false;
		}
	),
	game.activeWords.end()
);


			// sprawdz koniec gry
			if (lives <= 0) {
				isGameStarted = false;
				// wyswietlam game over
				sf::Text gameOver("GAME OVER", antonsc.getSfFont(), 90);
				gameOver.setFillColor(sf::Color::White);
				sf::Vector2f gameOverPosition(400.f, 250.f);
				gameOver.setPosition(gameOverPosition);
				window.draw(gameOver);
			}

			// RYSOWANIE aktywnych słów
			for (auto &gw: game.activeWords) {
#ifdef DEBUG_WORDS
				std::cout << "[DEBUG] After update: " << gw.debugInfo() << "\n";
#endif
				window.draw(gw.sfText);
			}
			// mechanika pisania
			userInputText.setString(playerInput);
			float textWidth = userInputText.getLocalBounds().width;
			float xPos = (static_cast<float>(Window::width) - textWidth) / 2.0f;
			float yPos = 30;
			userInputText.setPosition(xPos, yPos);
			window.draw(userInputText);

			// // rysowanie score i lives
			// scoreText.setString("Score: " + std::to_string(score));
			// livesText.setString("Lives: " + std::to_string(lives));
			// window.draw(scoreText);
			// window.draw(livesText);

			// // jesli pauza to rysuj napis "PAUSED"
			// if (isPaused) {
			// 	float pw = pauseText.getLocalBounds().width;
			// 	float ph = pauseText.getLocalBounds().height;
			// 	pauseText.setPosition((width - pw)/2.f, (height - ph)/2.f);
			// 	window.draw(pauseText);
			// }
		}

		// Prezentacja klatki
		window.display();
	}

	// 7. Po wyjściu z pętli okna dołącz wątek muzyki (jeśli jeszcze nie dołączony)
	if (loopThread.joinable()) {
		loopThread.join();
	}
}
