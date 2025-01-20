#ifndef WINDOW_H
#define WINDOW_H
#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <atomic>
#include <thread>
#include <string>

// enum class with game phases
enum class GamePhase { Title, Playing, GameOver };

class Window {
// public
public:
	// constructor
    Window(int width,
		int height,
		const std::string& title,
		int frameRate);

	// destructor
    ~Window();
	std::string playerInput;
    auto run() -> void;


// private
private:
    int width;
    int height;
    std::string title;
    int frameRate;

    // atomic flag for the main loop
    std::atomic<bool> running;

    // game phase
	GamePhase phase;

    // window object
    sf::RenderWindow window;

    // music loop
    sf::Music loop;
    std::thread loopThread;

	// player resources and score
	int score = 0;
	int lives = 6;
	bool isPaused = false;
};

// helper functions
auto loopThreadFn(sf::Music& loop, const std::atomic<bool>& running) -> void;
auto generateRandomColor() -> sf::Color;

#endif // WINDOW_H
