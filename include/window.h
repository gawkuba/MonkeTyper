#ifndef WINDOW_H
#define WINDOW_H
#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <atomic>
#include <thread>
#include <string>

// enum z fazami gry
enum class GamePhase { Title, Playing, GameOver };

class Window {
// pola publiczne
public:
	// konstruktor
    Window(int width,
		int height,
		const std::string& title,
		int frameRate);

	// destruktor
    ~Window();
	std::string playerInput;
    auto run() -> void;

// pola prywatne
private:
    int width;
    int height;
    std::string title;
    int frameRate;

    // zmienna kontrolująca działanie programu i wątku muzyki atomic bo wątek
    std::atomic<bool> running;

    // stan gry
	GamePhase phase;

    // okno
    sf::RenderWindow window;

    // muza i wątek muzy w tle
    sf::Music loop;
    std::thread loopThread;

	// zasoby gracza i flaga pauzy
	int score = 0;
	int lives = 6;
	bool isPaused = false;
};

// funkcje pomocnicze
auto loopThreadFn(sf::Music& loop, const std::atomic<bool>& running) -> void;
auto generateRandomColor() -> sf::Color;

#endif // WINDOW_H
