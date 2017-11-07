#ifndef ANIMATION_H
#define ANIMATION_H

#include <SFML/Graphics.hpp>

class Animation {
protected:
    int cuadros, cuadros_contador;
    bool hecho;
public:
    Animation(int cuadros);
    virtual sf::Vector2f next();
    sf::Vector2f finish();
    bool get_hecho();
    int get_cuadros();
};

#endif // ANIMATION_H
