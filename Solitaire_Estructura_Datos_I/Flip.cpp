#include "Flip.h"

Flip::Flip(int cuadros) : Animation(cuadros) {
    /* 2 porque la escala va de -1 a 1
       en una escala positiva indica reverso
       en la escala negativa indica el anverso
     */
    speed = 2.0f / cuadros;
    scale = 1.0f;
}
//Controlar velocidad de los cuadros (animacion)
sf::Vector2f Flip::next() {
    scale -= speed;
    cuadros_contador++;
    if (cuadros_contador >= cuadros) {
        scale = -1.0;
        hecho = true;
    }
    return sf::Vector2f(scale, 1.0f);
}
