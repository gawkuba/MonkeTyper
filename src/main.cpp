#include "window.h"

int main() {
    // Tworzymy obiekt okna
    Window app(1280, 720, "MonkeTyper", 60);

    // Uruchamiamy pętlę zdarzeń/logikę gry
    app.run();

    return 0;
}
