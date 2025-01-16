#ifndef WINDOW_H
#define WINDOW_H

#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <atomic>
#include <thread>
#include <string>
#include <iostream>

class Window {
public:
    Window(int width, int height, const std::string& title, int frameRate);
    ~Window();
	std::string playerInput;
    void run();

private:
    int width;
    int height;
    std::string title;
    int frameRate;

    // Zmienna kontrolująca działanie programu i wątku muzyki:
    std::atomic<bool> running;
    // Flaga startu gry:
    bool isGameStarted;

    // Główne okno SFML:
    sf::RenderWindow window;

    // Muzyka w tle + wątek, który ją uruchamia:
    sf::Music loop;
    std::thread loopThread;
};

// Funkcja uruchamiana w osobnym wątku do odtwarzania muzyki
auto loopThreadFn(sf::Music& loop, const std::atomic<bool>& running) -> void;

#endif // WINDOW_H
