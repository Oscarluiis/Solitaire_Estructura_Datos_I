#include "Animation.h"

Animation::Animation(int cuadros) {
    this->cuadros = cuadros;
    cuadros_contador = 0;
    hecho = false;
}

sf::Vector2f Animation::next() {
	return sf::Vector2f(0.0, 0.0);
}

sf::Vector2f Animation::finish() {
    cuadros_contador = cuadros;
    return next();
}

bool Animation::get_hecho() {
    return hecho;
}
int Animation::get_cuadros()
{
    return cuadros;
}
