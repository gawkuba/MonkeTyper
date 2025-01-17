#include "window.h"
#include <iostream>
#include "../include/game.h" // Funkcja loadWordsFromFile
#include "font.h" // Twój wrapper Font (OpenSans, AntonSC itd.)
#include <algorithm>
#include <ranges>

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
Window::Window(const int width, const int height, const std::string &title, const int frameRate)
	: width(static_cast<int>(width))
	, height(static_cast<int>(height))
	, title(title)
	, frameRate(frameRate)
	, running(true)
	, phase(GamePhase::Title)
	, score(0)
	, lives(6)
	, isPaused(false)
{
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






	// 2. Wczytaj fonty
	auto loadedFonts = Game::loadAllFonts();
	std::cout << "Loaded " << loadedFonts.size() << " fonts.\n";

	// fonty uzytkowe z okna
	Font antonsc = loadedFonts[1];
	Font orangeKid = loadedFonts[2];

	// Tekst do ekranu Title
	sf::Text monkey("Monke Typer", antonsc.getSfFont(), 90);
	monkey.setFillColor(sf::Color::White);
	sf::Vector2f monkeyTextPosition(400.f, 250.f);
	monkey.setPosition(monkeyTextPosition);

	sf::Text pressToStart("PRESS MOUSE LEFT BUTTON TO START", orangeKid.getSfFont(), 30);
	pressToStart.setFillColor(sf::Color::White);
	pressToStart.setPosition(435.f, 360.f);
	bool isVisible = true;
	sf::Clock blinkClock;
	float xvel = 4.f;

	// Tekst do Game Over
	sf::Text gameOverText("GAME OVER", antonsc.getSfFont(), 90);
	gameOverText.setFillColor(sf::Color::White);
	gameOverText.setPosition(430.f, 250.f);

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

	// tekst "PRESS ALT TO GO TO TITLE"
	sf::Text altToTitle;
	altToTitle.setFont(orangeKid.getSfFont());
	altToTitle.setCharacterSize(30);
	altToTitle.setString("PRESS ALT TO GO TO TITLE");
	altToTitle.setFillColor(sf::Color::White);
	altToTitle.setPosition(480.f, 360.f);


	sf::Text volUpText("Volume up (+)", orangeKid.getSfFont(), 15);
	sf::Text volDownText("Volume down (-)", orangeKid.getSfFont(), 15);
	sf::Text volMuteText("Mute (/)", orangeKid.getSfFont(), 15);
	sf::Text vol100Text("Volume Max (*)", orangeKid.getSfFont(), 15);
	sf::Vector2f volControl(1280.f-volUpText.getLocalBounds().width-10.f, 10.f);
	volUpText.setPosition(volControl);
	volDownText.setPosition(volControl.x, volControl.y + volUpText.getLocalBounds().height + 5.f);
	volMuteText.setPosition(volControl.x, volControl.y + volUpText.getLocalBounds().height + volDownText.getLocalBounds().height + 10.f);
	vol100Text.setPosition(volControl.x, volControl.y + volUpText.getLocalBounds().height + volDownText.getLocalBounds().height + volMuteText.getLocalBounds().height + 15.f);
	volUpText.setFillColor(sf::Color::White); volDownText.setFillColor(sf::Color::White); volMuteText.setFillColor(sf::Color::White); vol100Text.setFillColor(sf::Color::White);


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
			// Wyjście klawiszem ESC
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
				window.close();
				running = false;
				loop.stop();
			}
			// Pauza
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::LControl) {
				isPaused = !isPaused;
			}

			// Efekt dźwiękowy przy naciśnięciu klawisza
			if (event.type == sf::Event::KeyPressed) {
				clickSound.play();
			}

			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Multiply) {
				loop.setVolume(100.f);
			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Add) {
				loop.setVolume(loop.getVolume() + 10.f);
			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Subtract) {
				if (loop.getVolume() <= 10.f) {
					loop.setVolume(0.f);
				} else {
					loop.setVolume(loop.getVolume() - 10.f);
				}
			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Divide) {
				loop.setVolume(0.f);
			}

			// wpisywanie tekstu (tylko w fazie Playing, jeśli nie jest pauza)
			if (phase == GamePhase::Playing && !isPaused) {
				if (event.type == sf::Event::TextEntered) {
					if (event.text.unicode == 8) {
						// backspace
						if (!playerInput.empty()) playerInput.pop_back();
					}
					else if (event.text.unicode == 13) {
						// enter
						Game::checkWordOnScreen(playerInput, game.activeWords, game);
						for (auto &gw: game.activeWords) {
							if (!gw.isAlive) score++;
						}
						playerInput.clear();
					}
					else if (event.text.unicode < 128) {
						if (char typed = static_cast<char>(event.text.unicode); std::isprint(typed)) {
							playerInput += typed;
						}
					}
				}
			}
		} // koniec pollEvent

		// Czyszczenie ekranu
		window.clear();

		// ========== PHASE HANDLING ==========

		// ----------------- TITLE PHASE -----------------
		if (phase == GamePhase::Title) {
			// Rysowanie animowanego napisu
			if (loop.getStatus() != sf::Music::Playing) {
				loop.play();
			}
			window.draw(monkey);
			if (monkeyTextPosition.x < 360.f || monkeyTextPosition.x > 440.f) {
				xvel = -xvel;
			}
			monkeyTextPosition.x += xvel;
			monkey.setPosition(monkeyTextPosition);

			// miganie PressToStart
			if (blinkClock.getElapsedTime().asMilliseconds() >= 350) {
				isVisible = !isVisible;
				blinkClock.restart();
			}
			if (isVisible) {
				window.draw(pressToStart);
			}

			// Klik mysz -> start gry:
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				phase = GamePhase::Playing;

				// reset stanu:
				score = 0;
				lives = 6;
				playerInput.clear();
				// Kasujemy activeWords i ponownie initInactiveWords:
				game.activeWords.clear();
				game.initInactiveWords();

				spawnClock.restart();
			}
		}

		// ----------------- PLAYING PHASE -----------------
		else if (phase == GamePhase::Playing) {

			if (!isPaused) {
				// spawn co 1.5s
				if (spawnClock.getElapsedTime().asSeconds() >= 1.5f) {
					game.spawnWord();
					spawnClock.restart();
				}

				// Update logic
				for (auto &gw: game.activeWords) {
					gw.update(dt);

					float x = gw.sfText.getPosition().x;
					float ww = static_cast<float>(window.getSize().x); // faktyczna szerokość okna

					// kolor zależnie od x
					if (x < ww * 0.5f) {
						gw.sfText.setFillColor(sf::Color::Green);
					} else if (x < ww * 0.75f) {
						gw.sfText.setFillColor(sf::Color::Yellow);
					} else {
						gw.sfText.setFillColor(sf::Color::Red);
					}

					// wychodzi poza ekran
					if (x > ww) {
						gw.isAlive = false;
						lives--;
					}
				}
				// recykling
				std::erase_if(game.activeWords, [&](Game::GameWord &w) -> bool {
					if (!w.isAlive) {
						game.releaseFont(w.fontIndex);
						w.fontIndex = -1;
						w.isAlive = false;
						game.inActiveWords.push_back(w);
						return true;
					}
					return false;
				});
			} // koniec if(!isPaused)

			// sprawdz czy koniec gry
			if (lives <= 0) {
				phase = GamePhase::GameOver;
			}

			// Rysowanie activeWords
			for (auto &gw: game.activeWords) {
				window.draw(gw.sfText);
			}
			// Rysowanie playerInput
			userInputText.setString(playerInput);
			float textWidth = userInputText.getLocalBounds().width;
			float xPos = (static_cast<float>(window.getSize().x) - textWidth) / 2.f;
			float yPos = static_cast<float>(window.getSize().y) - 50.f;
			userInputText.setPosition(xPos, yPos);
			window.draw(userInputText);

			// Score, lives
			scoreText.setString("Score: " + std::to_string(score));
			livesText.setString("Lives: " + std::to_string(lives));
			window.draw(scoreText);
			window.draw(livesText);

			// Pauza
			if (isPaused) {
				float pw = pauseText.getLocalBounds().width;
				float ph = pauseText.getLocalBounds().height;
				pauseText.setPosition((static_cast<float>(width) - pw) / 2.f, (static_cast<float>(height) - ph) / 2.f);
				window.draw(pauseText);
				window.draw(volUpText);
				window.draw(volDownText);
				window.draw(volMuteText);
				window.draw(vol100Text);


			}
		}
		// ----------------- GAMEOVER PHASE -----------------
		else if (phase == GamePhase::GameOver) {
			// Rysujemy napis gameOverText
			loop.stop();
			window.draw(gameOverText);

			if (blinkClock.getElapsedTime().asMilliseconds() >= 350) {
				isVisible = !isVisible;
				blinkClock.restart();
			}
			if (isVisible) {
				window.draw(altToTitle);
			}

			// Możemy czekać np. na lewy alt, żeby wrócić do Title
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt)) {
				phase = GamePhase::Title;
			}
		}

		// ========== KONIEC PHASE HANDLING ==========

		// Prezentacja klatki
		window.display();
	}

	// 7. Po wyjściu z pętli okna dołącz wątek muzyki (jeśli jeszcze nie dołączony)
	if (loopThread.joinable()) {
		loopThread.join();
	}
}
