#include "window.h"
#include <algorithm>
#include <iostream>
#include <ranges>
#include <random>
#include "../include/game.h"
#include "font.h"

//  muzyka funkcja pomocnicza
auto loopThreadFn(sf::Music &loop, const std::atomic<bool> &running) -> void {
	if (running) {
		loop.setLoop(true);
		loop.play();
	}
}

// kolor funkcja pomocnicza ale nie za ciemny
auto generateRandomColor() -> sf::Color {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, 255);
	sf::Color color;
	do {
		color = sf::Color(dist(gen), dist(gen), dist(gen));
	} while (color.r + color.g + color.b < 200);
	return color;
}

// konstruktor okna
Window::Window(const int width, const int height, const std::string &title, const int frameRate) :
	width(static_cast<int>(width)), height(static_cast<int>(height)), title(title), frameRate(frameRate), running(true),
	phase(GamePhase::Title) {
	window.create(sf::VideoMode(width, height), title, sf::Style::Close);
	window.setFramerateLimit(frameRate);
}

// destruktor okna
Window::~Window() {
	// zatrzymaj muzykę i wątek
	running = false;
	loop.stop();
	if (loopThread.joinable()) {
		loopThread.join();
	}
}

// glowna petla gry i logika
auto Window::run() -> void {
	// inicjalizacja gry
	Game game("../assets/words.txt");

	// wczytywanie fontów
	auto loadedFonts = Game::loadAllFonts();

	// fonty uzytkowe z okna
	Font antonsc = loadedFonts[1];
	Font orangeKid = loadedFonts[2];

	// teksty do ekranu Title
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

	// tekst do Game Over
	sf::Text gameOverText("GAME OVER", antonsc.getSfFont(), 90);
	gameOverText.setFillColor(sf::Color::White);
	gameOverText.setPosition(430.f, 250.f);

	// tekst wpisywany przez gracza
	sf::Text userInputText;
	userInputText.setFont(orangeKid.getSfFont());
	userInputText.setCharacterSize(30);
	userInputText.setFillColor(sf::Color::White);

	// tekst z wynikiem
	sf::Text scoreText;
	scoreText.setFont(orangeKid.getSfFont());
	scoreText.setCharacterSize(20);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setPosition(20.f, 20.f);

	// tekst z ilością żyć
	sf::Text livesText;
	livesText.setFont(orangeKid.getSfFont());
	livesText.setCharacterSize(20);
	livesText.setFillColor(sf::Color::White);
	livesText.setPosition(20.f, 50.f);

	// tekst "PAUSED"
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

	// skróty klawiszowe
	sf::Text volUpText( "Volume up      (+)", orangeKid.getSfFont(), 15);
	sf::Text volDownText("Volume down  (-)", orangeKid.getSfFont(), 15);
	sf::Text volMuteText("Mute               (/)", orangeKid.getSfFont(), 15);
	sf::Text vol100Text("Volume Max   (*)", orangeKid.getSfFont(), 15);
	sf::Text pauseInfo( "Pause        (CTRL)", orangeKid.getSfFont(), 15);
	sf::Text exitInfo(  "Exit             (ESC)", orangeKid.getSfFont(), 15);
	float windowHeight = static_cast<float>(window.getSize().y);
	sf::Vector2f volControl(1280.f - volUpText.getLocalBounds().width - 10.f, windowHeight - 10.f - volUpText.getLocalBounds().height);
	volUpText.setPosition(volControl);
	volDownText.setPosition(volControl.x, volControl.y - volDownText.getLocalBounds().height - 5.f);
	volMuteText.setPosition(volControl.x, volDownText.getPosition().y - volMuteText.getLocalBounds().height - 5.f);
	vol100Text.setPosition(volControl.x, volMuteText.getPosition().y - vol100Text.getLocalBounds().height - 5.f);
	pauseInfo.setPosition(volControl.x, vol100Text.getPosition().y - pauseInfo.getLocalBounds().height - 5.f);
	exitInfo.setPosition(volControl.x, pauseInfo.getPosition().y - exitInfo.getLocalBounds().height - 5.f);
	volUpText.setFillColor(sf::Color::White);
	volDownText.setFillColor(sf::Color::White);
	volMuteText.setFillColor(sf::Color::White);
	vol100Text.setFillColor(sf::Color::White);
	pauseInfo.setFillColor(sf::Color::White);
	exitInfo.setFillColor(sf::Color::White);


	// bardzo fajna ikona
	if (sf::Image icon; !icon.loadFromFile("../assets/img/jp2.png")) {
		std::cout << "Failed to load icon ../assets/img/jp2.png\n";
	} else {
		window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	}

	// osobny wątek do muzyki w tle
	if (!loop.openFromFile("../assets/audio/loop.ogg")) {
		std::cout << "Failed to open loop music ../assets/audio/loop.ogg\n";
	}
	loopThread = std::thread(loopThreadFn, std::ref(loop), std::ref(running));

	// click efekt dźwiękowy
	sf::SoundBuffer clickSoundBuffer;
	if (!clickSoundBuffer.loadFromFile("../assets/audio/click.ogg")) {
		std::cout << "Failed to load click sound ../assets/audio/click.ogg\n";
	}
	sf::Sound clickSound;
	clickSound.setBuffer(clickSoundBuffer);

	// zegary do mierzenia czasu
	sf::Clock clock; // do delta time
	sf::Clock spawnClock; // jak często spawnujemy nowe słowo

	// wartosc logiczna, czy pomijac nastepny znak z klawiatury (np. po wcisnieciu numpada pod ktorym są skróty do kontroli głośności)
	bool skipNumpadNextChar = false;

	// główna pętla zdarzeń
	while (window.isOpen()) {
		float dt = clock.restart().asSeconds();
		sf::Event event{};

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
				running = false;
				loop.stop();
			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
				window.close();
				running = false;
				loop.stop();
			}

			// pauza i wznowienie gry
			if (event.type == sf::Event::KeyPressed &&
				(event.key.code == sf::Keyboard::LControl ||
				event.key.code == sf::Keyboard::RControl)) {
				isPaused = !isPaused;
	}

			// obsługa skrótów klawiszowych
			if (event.type == sf::Event::KeyPressed) {
				clickSound.play(); // your SFX

				// max głośność
				if (event.key.code == sf::Keyboard::Multiply) {
					loop.setVolume(100.f);
					skipNumpadNextChar = true;
				}
				// podgłaśnianie
				else if (event.key.code == sf::Keyboard::Add) {
					loop.setVolume(std::min(loop.getVolume() + 10.f, 100.f));
					skipNumpadNextChar = true;
				}
				// zciszanie
				else if (event.key.code == sf::Keyboard::Subtract) {
					float newVol = loop.getVolume() - 10.f;
					if (newVol < 0.f)
						newVol = 0.f;
					loop.setVolume(newVol);
					skipNumpadNextChar = true;
				}
				// wyciszanie
				else if (event.key.code == sf::Keyboard::Divide) {
					loop.setVolume(0.f);
					skipNumpadNextChar = true;
				} else {
					skipNumpadNextChar = false;
				}
			}

			// obsługa zdarzeń tekstowych
			if (phase == GamePhase::Playing && !isPaused) {
				if (event.type == sf::Event::TextEntered) {
					// kiedy wcisniety numpad operator, to pomijamy nastepny znak
					if (skipNumpadNextChar) {
						skipNumpadNextChar = false;
						continue;
					}

					// obsługa tekstu wpisywanego przez gracza
					if (event.text.unicode == 8) {
						// backspace
						if (!playerInput.empty())
							playerInput.pop_back();
					} else if (event.text.unicode == 13) {
						// enter
						Game::checkWordOnScreen(playerInput, game.activeWords, game);
						for (auto &gw: game.activeWords) {
							if (!gw.isAlive)
								score++;
						}
						playerInput.clear();
						// wypisywanie
					} else if (event.text.unicode < 128) {
						if (char typed = static_cast<char>(event.text.unicode); std::isprint(typed)) {
							playerInput += typed;
						}
					}
				}
			}
		}
		// czyszczenie ekranu
		window.clear();

		// ========== obsluga faz gry ==========

		// ----------------- faza tytułowa -----------------
		if (phase == GamePhase::Title) {

			if (loop.getStatus() != sf::Music::Playing) {
				loop.play();
			}
			// rysowanie animowanego napisu
			window.draw(monkey);
			if (monkeyTextPosition.x < 360.f || monkeyTextPosition.x > 440.f) {
				xvel = -xvel;
				monkey.setFillColor(generateRandomColor());

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

			// rysowanie info do skrótów klawiszowych
			window.draw(volUpText);
			window.draw(volDownText);
			window.draw(volMuteText);
			window.draw(vol100Text);
			window.draw(pauseInfo);
			window.draw(exitInfo);

			// start gry po kliknięciu lewego przycisku myszy
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				phase = GamePhase::Playing;

				// reset stanu gry
				score = 0;
				lives = 6;
				playerInput.clear();

				// kasujemy activeWords i ponownie aktywujemy wszystkie słowa nieaktywne
				game.activeWords.clear();
				game.initInactiveWords();

				spawnClock.restart();
			}
		}

		// ----------------- faza gry -----------------
		else if (phase == GamePhase::Playing) {

			if (!isPaused) {
				// spawn co 1.5s
				if (spawnClock.getElapsedTime().asSeconds() >= 1.5f) {
					game.spawnWord();
					spawnClock.restart();
				}

				// logika aktualizacji stanu gry
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

					// wychodzi poza ekran i gracz traci życie
					if (x > ww) {
						gw.isAlive = false;
						lives--;
					}
				}
				// recykling "zielony ład"
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

			// rysowanie activeWords
			for (auto &gw: game.activeWords) {
				window.draw(gw.sfText);
			}
			// rysowanie info do skrótów klawiszowych
			window.draw(volUpText);
			window.draw(volDownText);
			window.draw(volMuteText);
			window.draw(vol100Text);
			window.draw(pauseInfo);
			window.draw(exitInfo);

			// rysowanie playerInput
			userInputText.setString(playerInput);
			float textWidth = userInputText.getLocalBounds().width;
			float xPos = (static_cast<float>(window.getSize().x) - textWidth) / 2.f;
			float yPos = static_cast<float>(window.getSize().y) - 50.f;
			userInputText.setPosition(xPos, yPos);
			window.draw(userInputText);

			// zycia i wynik
			scoreText.setString("Score: " + std::to_string(score));
			livesText.setString("Lives: " + std::to_string(lives));
			window.draw(scoreText);
			window.draw(livesText);

			// co kiedy pauza
			if (isPaused) {
				float pw = pauseText.getLocalBounds().width;
				pauseText.setPosition(((static_cast<float>(width) - pw) / 2.f)-30.f, 200);
				window.draw(pauseText);

			}
		}
		// ----------------- faza game over -----------------
		else if (phase == GamePhase::GameOver) {
			// rysujemy napis gameOver
			loop.stop();
			window.draw(gameOverText);

			// rysowanie info do skrótów klawiszowych
			window.draw(volUpText);
			window.draw(volDownText);
			window.draw(volMuteText);
			window.draw(vol100Text);
			window.draw(pauseInfo);
			window.draw(exitInfo);

			// miganie tak jak w fazie tytułowej
			if (blinkClock.getElapsedTime().asMilliseconds() >= 350) {
				isVisible = !isVisible;
				blinkClock.restart();
			}
			if (isVisible) {
				window.draw(altToTitle);
			}

			// alt do title
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt)) {
				phase = GamePhase::Title;
			}
		}

		// ========== koniec obslugi faz ==========

		// najwazniejsza linijka kodu zaraz po ikonie
		window.display();
	}

	// zatrzymaj muzykę i wątek jesli nie zrobił tego destruktor
	if (loopThread.joinable()) {
		loopThread.join();
	}
}
