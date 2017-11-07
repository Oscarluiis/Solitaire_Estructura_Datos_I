#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <vector>
#include "Meta.h"
#include "Game.h"
#include <SFML/Audio.hpp>

void handle_events(sf::RenderWindow &window, Game &game);

int main() {
    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(S_WIDTH, S_HEIGHT), "Estructura De Datos I @Oscarluiis ", sf::Style::Close, settings);

    
    sf::Music music;
    if (!music.openFromFile("/Users/LuiizMusic11/Downloads/song.ogg")) {
        return EXIT_FAILURE;
    }

    music.play();
    
    window.setKeyRepeatEnabled(false);
    window.setFramerateLimit(FPS);
   
    sf::Image icon;
    if (!icon.loadFromFile("/Users/LuiizMusic11/Desktop/cartas.png")) {
        return EXIT_FAILURE;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    
    // Load a sprite to display

    Game game;

    if (game.get_kill()) {
        return 1;
    }

    while (window.isOpen()) {
        game.update();

        window.clear(sf::Color(14, 110, 50));
        game.draw(window);
        window.display();

        handle_events(window, game);
    }
}

void handle_events(sf::RenderWindow &window, Game &game) {
    sf::Event eve;
    while (window.pollEvent(eve)) {
        switch(eve.type) {
        case sf::Event::Closed:
            window.close();
            break;
        case sf::Event::KeyPressed:
            if (eve.key.code == sf::Keyboard::Q) {
                window.close();
            } else if (eve.key.code == sf::Keyboard::R) {
                game.reset_game();
            } else if (eve.key.code == sf::Keyboard::C) {
                game.flip_draw_mode();
            } else if (eve.key.code == sf::Keyboard::S) {
                game.auto_solve();
            }
            break;
        case sf::Event::MouseButtonPressed:
            if (eve.mouseButton.button == sf::Mouse::Left) {
                game.mouse_pressed(sf::Vector2f(eve.mouseButton.x, eve.mouseButton.y));
            }
            break;
        case sf::Event::MouseButtonReleased:
            if (eve.mouseButton.button == sf::Mouse::Left) {
                game.mouse_released(sf::Vector2f(eve.mouseButton.x, eve.mouseButton.y));
            }
            break;
        case sf::Event::MouseMoved:
            game.mouse_moved(sf::Vector2f(eve.mouseMove.x, eve.mouseMove.y));
            break;
        case sf::Event::MouseLeft:
            game.mouse_left();
            break;
        }
    }
}
