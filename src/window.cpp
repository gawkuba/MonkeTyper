#include "window.h"
#include <algorithm>
#include <iostream>
#include <ranges>
#include <random>
#include "../include/game.h"
#include "font.h"

// Helper function to loop background music in a separate thread
auto loopThreadFn(sf::Music &loop, const std::atomic<bool> &running) -> void {
	if (running) {
		loop.setLoop(true);
		loop.play();
	}
}

// Helper function to generate a random bright color (ensures color is not too dark)
auto generateRandomColor() -> sf::Color {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, 255);
	sf::Color color;
	do {
		color = sf::Color(dist(gen), dist(gen), dist(gen));
	} while (color.r + color.g + color.b < 200); // Ensure brightness is above a threshold
	return color;
}

// Constructor for the Window class
Window::Window(const int width, const int height, const std::string &title, const int frameRate) :
	width(static_cast<int>(width)), height(static_cast<int>(height)), title(title), frameRate(frameRate), running(true),
	phase(GamePhase::Title) {
	window.create(sf::VideoMode(width, height), title, sf::Style::Close);
	window.setFramerateLimit(frameRate);
}

// Destructor for the Window class
Window::~Window() {
	// Stop music and join the music thread
	running = false;
	loop.stop();
	if (loopThread.joinable()) {
		loopThread.join();
	}
}

// Main game loop and logic
auto Window::run() -> void {
	// Initialize the game
	Game game("../assets/words.txt");

	// Load fonts used in the game
	auto loadedFonts = Game::loadAllFonts();

	// Fonts used for various texts
	Font antonsc = loadedFonts[1];
	Font orangeKid = loadedFonts[2];

	// Title screen texts
	sf::Text monkey("Monke Typer", antonsc.getSfFont(), 90);
	monkey.setFillColor(sf::Color::White);
	sf::Vector2f monkeyTextPosition(400.f, 250.f);
	monkey.setPosition(monkeyTextPosition);

	sf::Text pressToStart("PRESS MOUSE LEFT BUTTON TO START", orangeKid.getSfFont(), 30);
	pressToStart.setFillColor(sf::Color::White);
	pressToStart.setPosition(435.f, 360.f);
	bool isVisible = true; // Controls blinking visibility of text
	sf::Clock blinkClock; // Timer for blinking effect
	float xvel = 4.f; // Speed for horizontal movement of title text

	// Game Over screen text
	sf::Text gameOverText("GAME OVER", antonsc.getSfFont(), 90);
	gameOverText.setFillColor(sf::Color::White);
	gameOverText.setPosition(430.f, 250.f);

	// Player input text
	sf::Text userInputText;
	userInputText.setFont(orangeKid.getSfFont());
	userInputText.setCharacterSize(30);
	userInputText.setFillColor(sf::Color::White);

	// Score display text
	sf::Text scoreText;
	scoreText.setFont(orangeKid.getSfFont());
	scoreText.setCharacterSize(20);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setPosition(20.f, 20.f);

	// Lives display text
	sf::Text livesText;
	livesText.setFont(orangeKid.getSfFont());
	livesText.setCharacterSize(20);
	livesText.setFillColor(sf::Color::White);
	livesText.setPosition(20.f, 50.f);

	// Pause text
	sf::Text pauseText;
	pauseText.setFont(orangeKid.getSfFont());
	pauseText.setCharacterSize(40);
	pauseText.setString("PAUSED");

	// Instruction to return to the title screen
	sf::Text altToTitle;
	altToTitle.setFont(orangeKid.getSfFont());
	altToTitle.setCharacterSize(30);
	altToTitle.setString("PRESS ALT TO GO TO TITLE");
	altToTitle.setFillColor(sf::Color::White);
	altToTitle.setPosition(480.f, 360.f);

	// Shortcut instructions
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

	// Set colors for shortcut instructions
	volUpText.setFillColor(sf::Color::White);
	volDownText.setFillColor(sf::Color::White);
	volMuteText.setFillColor(sf::Color::White);
	vol100Text.setFillColor(sf::Color::White);
	pauseInfo.setFillColor(sf::Color::White);
	exitInfo.setFillColor(sf::Color::White);

	// Load and set the window icon
	if (sf::Image icon; !icon.loadFromFile("../assets/img/jp2.png")) {
		std::cout << "Failed to load icon ../assets/img/jp2.png\n";
	} else {
		window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	}

	// Background music setup in a separate thread
	if (!loop.openFromFile("../assets/audio/loop.ogg")) {
		std::cout << "Failed to open loop music ../assets/audio/loop.ogg\n";
	}
	loopThread = std::thread(loopThreadFn, std::ref(loop), std::ref(running));

	// Click sound effect setup
	sf::SoundBuffer clickSoundBuffer;
	if (!clickSoundBuffer.loadFromFile("../assets/audio/click.ogg")) {
		std::cout << "Failed to load click sound ../assets/audio/click.ogg\n";
	}
	sf::Sound clickSound;
	clickSound.setBuffer(clickSoundBuffer);

	// Timers for delta time and word spawning
	sf::Clock clock; // Timer for delta time
	sf::Clock spawnClock; // Timer for word spawning intervals

	// Flag to skip the next character input if a numpad key is used (for volume controls)
	bool skipNumpadNextChar = false;

	// main game loop
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

			// pause and resume game
			if (event.type == sf::Event::KeyPressed &&
				(event.key.code == sf::Keyboard::LControl ||
				event.key.code == sf::Keyboard::RControl)) {
				isPaused = !isPaused;
	}

			// shortcuts handling
			if (event.type == sf::Event::KeyPressed) {
				clickSound.play(); // your SFX

				// volume max
				if (event.key.code == sf::Keyboard::Multiply) {
					loop.setVolume(100.f);
					skipNumpadNextChar = true;
				}
				// volume up
				else if (event.key.code == sf::Keyboard::Add) {
					loop.setVolume(std::min(loop.getVolume() + 10.f, 100.f));
					skipNumpadNextChar = true;
				}
				// volume down
				else if (event.key.code == sf::Keyboard::Subtract) {
					float newVol = loop.getVolume() - 10.f;
					if (newVol < 0.f)
						newVol = 0.f;
					loop.setVolume(newVol);
					skipNumpadNextChar = true;
				}
				// muting
				else if (event.key.code == sf::Keyboard::Divide) {
					loop.setVolume(0.f);
					skipNumpadNextChar = true;
				} else {
					skipNumpadNextChar = false;
				}
			}

			// text event handling
			if (phase == GamePhase::Playing && !isPaused) {
				if (event.type == sf::Event::TextEntered) {
					// when num pad key is pressed, skip the next character
					if (skipNumpadNextChar) {
						skipNumpadNextChar = false;
						continue;
					}

					// handling player entered text
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
						// adding player input to the string that will be drawn on to the screen
					} else if (event.text.unicode < 128) {
						if (char typed = static_cast<char>(event.text.unicode); std::isprint(typed)) {
							playerInput += typed;
						}
					}
				}
			}
		}
		// window clearing
		window.clear();

		// ========== game phase handling ==========

		// ----------------- tittle phase -----------------
		if (phase == GamePhase::Title) {

			if (loop.getStatus() != sf::Music::Playing) {
				loop.play();
			}
			// animated title text
			window.draw(monkey);
			if (monkeyTextPosition.x < 360.f || monkeyTextPosition.x > 440.f) {
				xvel = -xvel;
				monkey.setFillColor(generateRandomColor());

			}
			monkeyTextPosition.x += xvel;
			monkey.setPosition(monkeyTextPosition);

			// blinking PressToStart
			if (blinkClock.getElapsedTime().asMilliseconds() >= 350) {
				isVisible = !isVisible;
				blinkClock.restart();
			}
			if (isVisible) {
				window.draw(pressToStart);
			}

			// drawing shortcut instructions
			window.draw(volUpText);
			window.draw(volDownText);
			window.draw(volMuteText);
			window.draw(vol100Text);
			window.draw(pauseInfo);
			window.draw(exitInfo);

			// start of the game when left mouse button is pressed
			if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				phase = GamePhase::Playing;

				// game parameters reset
				score = 0;
				lives = 6;
				playerInput.clear();

				// reset of active words and init of inactive words
				game.activeWords.clear();
				game.initInactiveWords();

				spawnClock.restart();
			}
		}

		// ----------------- game phase -----------------
		else if (phase == GamePhase::Playing) {

			if (!isPaused) {
				// spawn for every 1.5s
				if (spawnClock.getElapsedTime().asSeconds() >= 1.5f) {
					game.spawnWord();
					spawnClock.restart();
				}

				// logika aktualizacji stanu gry
				for (auto &gw: game.activeWords) {
					gw.update(dt);

					float x = gw.sfText.getPosition().x;
					float ww = static_cast<float>(window.getSize().x); // factual window width

					// color change based on position
					if (x < ww * 0.5f) {
						gw.sfText.setFillColor(sf::Color::Green);
					} else if (x < ww * 0.75f) {
						gw.sfText.setFillColor(sf::Color::Yellow);
					} else {
						gw.sfText.setFillColor(sf::Color::Red);
					}

					// word goes out of screen = lose life
					if (x > ww) {
						gw.isAlive = false;
						lives--;
					}
				}
				// font recycling
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
			} // end of if(!isPaused)

			// check if game is over
			if (lives <= 0) {
				phase = GamePhase::GameOver;
			}

			// drawing activeWords
			for (auto &gw: game.activeWords) {
				window.draw(gw.sfText);
			}
			// drawing shortcut instructions
			window.draw(volUpText);
			window.draw(volDownText);
			window.draw(volMuteText);
			window.draw(vol100Text);
			window.draw(pauseInfo);
			window.draw(exitInfo);

			// drawing playerInput
			userInputText.setString(playerInput);
			float textWidth = userInputText.getLocalBounds().width;
			float xPos = (static_cast<float>(window.getSize().x) - textWidth) / 2.f;
			float yPos = static_cast<float>(window.getSize().y) - 50.f;
			userInputText.setPosition(xPos, yPos);
			window.draw(userInputText);

			// lives and score
			scoreText.setString("Score: " + std::to_string(score));
			livesText.setString("Lives: " + std::to_string(lives));
			window.draw(scoreText);
			window.draw(livesText);

			// what to do when paused
			if (isPaused) {
				float pw = pauseText.getLocalBounds().width;
				pauseText.setPosition(((static_cast<float>(width) - pw) / 2.f)-20.f, 200);
				window.draw(pauseText);

			}
		}
		// ----------------- game over phase -----------------
		else if (phase == GamePhase::GameOver) {
			// draw game over text
			loop.stop();
			window.draw(gameOverText);

			// drrawing shortcut instructions
			window.draw(volUpText);
			window.draw(volDownText);
			window.draw(volMuteText);
			window.draw(vol100Text);
			window.draw(pauseInfo);
			window.draw(exitInfo);

			// blinking effect
			if (blinkClock.getElapsedTime().asMilliseconds() >= 350) {
				isVisible = !isVisible;
				blinkClock.restart();
			}
			if (isVisible) {
				window.draw(altToTitle);
			}

			// alt to title
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::RAlt)) {
				phase = GamePhase::Title;
			}
		}

		// ========== end of phase handling ==========

		// window display
		window.display();
	}

	// stop music and join the music thread if destructor fails to do so
	if (loopThread.joinable()) {
		loopThread.join();
	}
}
