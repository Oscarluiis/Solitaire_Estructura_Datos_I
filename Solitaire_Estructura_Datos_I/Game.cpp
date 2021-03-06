#include "Game.h"

Game::Game() {
    kill = false;
    srand(time(NULL));
    if (!texture.loadFromFile("/Users/LuiizMusic11/Downloads/car_.png")) { //Aqui se indica donde estan las cartas con todo y la direccion.
        
        kill = true; //Por si falla la carga esto detiene el juego.
        
        freopen("error.txt", "w", stdout);
        std::cout << "Error al cargar las cartas, revisa la direccion" << "\n";
        return;
    }
    //Texturas del juego para el inicio
    
    texture.setSmooth(true);
    backside.set_texture(texture);
    three_card_rule = true;
    init_rect();
    init_text();
    reset_game();
}

void Game::reset_game() {
    //state vars
    master_state = STATE_ANIMATION;
    detailed_state = STATE_ANIMATION_INITIALIZING_DECK;
    //field handlers
    ace_locations.clear();
    ace_locations = std::vector<int>(4, -1);
    field.clear();
    std::vector<Card> populated_deck = populate_deck();
    std::vector<std::vector<Card> > deck;
    deck.push_back(populated_deck); //a
    deck.push_back(std::vector<Card>()); //add flipped deck
    std::vector<std::vector<Card> > home(4, std::vector<Card>()); //add empty homes
    std::vector<std::vector<Card> > play_field(7, std::vector<Card>()); //add empty fields
    
    field.push_back(deck);
    field.push_back(home);
    field.push_back(play_field);

    transit.clear();
    cursor.clear();

    frame_counter = card_counter = 0;
    col_tracker = sf::Vector2i(0, 0);
    frame_delay = 0.05f;
    skip = mouse_down = false;
    won = solvable = false;
}

std::vector<Card> Game::populate_deck() {
    std::vector<Card> shuffle_deck, ret;
    //add all the cards
    for (int i = SUIT_CLUBS; i <= SUIT_SPADES; i++) {
        for (int j = 1; j <= 13; j++) {
            shuffle_deck.push_back(Card(i, j, texture));
            shuffle_deck.back().set_center_position(field_rect[DECK][0].getPosition());
        }
    }
    //randomly pull cards from sotrted deck
    while (shuffle_deck.size() != 0) {
        int index = rand() % shuffle_deck.size();
        ret.push_back(shuffle_deck[index]);
        shuffle_deck.erase(shuffle_deck.begin() + index);
    }
    return ret;
}

void Game::init_rect() {
    float spacer = 17.643f;
    float top_spacer = 30.179f;
    
    //Espacio entre las cartas y el campo de juego
    
    float card_spacer = 114.286f;
    sf::RectangleShape r(sf::Vector2f(CARD_SPRITE_WIDTH, CARD_SPRITE_HEIGHT));
    r.setOrigin(CARD_SPRITE_WIDTH / 2.0f, CARD_SPRITE_HEIGHT / 2.0f);
    r.setFillColor(sf::Color::Transparent);
    r.setOutlineThickness(-1.0f);
    r.setOutlineColor(sf::Color::Black);
    std::vector<sf::RectangleShape> deck, home, play_field;
    for (int i = 0; i < 2; i++) {
        r.setPosition(sf::Vector2f(spacer + CARD_SPRITE_WIDTH / 2.0f + card_spacer * i,
                                    spacer + CARD_SPRITE_HEIGHT / 2.0f));
        deck.push_back(sf::RectangleShape(r));
    }
    for (int i = 3; i < 7; i++) {
        r.setPosition(sf::Vector2f(spacer + CARD_SPRITE_WIDTH / 2.0f + card_spacer * i,
                                    spacer + CARD_SPRITE_HEIGHT / 2.0f));
        home.push_back(sf::RectangleShape(r));
    }
    for (int i = 0; i < 7; i++) {
        r.setPosition(sf::Vector2f(spacer + CARD_SPRITE_WIDTH / 2.0f + card_spacer * i,
            spacer + CARD_SPRITE_HEIGHT * 1.5f + top_spacer));
        play_field.push_back(sf::RectangleShape(r));
    }
    field_rect.push_back(deck);
    field_rect.push_back(home);
    field_rect.push_back(play_field);
}

void Game::init_text() { //Esto como opciones del juego
    if (!font.loadFromFile("/Users/LuiizMusic11/Desktop/Solitaire-master/times.ttf")) {
        freopen("error.txt", "w", stdout);
        std::cout << "No se puede cargar la fuente, revisa direccion" << "\n";
        return;
    }
    info_text.setFont(font);
    won_text.setFont(font);
    solvable_text.setFont(font);
    won_text.setString("Felicidades! Ganaste!");
    info_text.setColor(sf::Color::Black);
    won_text.setColor(sf::Color::Black);
    solvable_text.setColor(sf::Color::Black);
    info_text.setCharacterSize(15);
    won_text.setCharacterSize(30);
    solvable_text.setCharacterSize(15);
    info_text.setPosition(10, S_HEIGHT - 20);
    won_text.setOrigin(won_text.getGlobalBounds().width / 2, won_text.getGlobalBounds().height / 2);
    won_text.setPosition(S_WIDTH / 2, S_HEIGHT / 2);
    solvable_text.setPosition(10, info_text.getPosition().y - 20);
}

void Game::update() { //Se actualizan las animaciones para cuando se destapa
    if (master_state == STATE_ANIMATION) {
        frame_counter++;
        if (skip || frame_counter >= to_frame(frame_delay)) {
            frame_counter = 0;
            if (detailed_state == STATE_ANIMATION_INITIALIZING_DECK) {
                if (card_counter <= 27) {
                    //destination of card
                    int col = col_tracker.x;
                    sf::Vector2f destination = field_rect[PLAY_FIELD][col].getPosition();
                    destination.y += VERT_CARD_SPACING * col_tracker.y;
                    int animation_type = STATE_ANIMATION_MOVING_CARD;
                    if (col == col_tracker.y) {
                        animation_type = STATE_ANIMATION_MOVE_AND_FLIP_CARD;
                    }
                    Position p(destination, sf::Vector3i(DECK, 0, field[DECK][0].size()), sf::Vector3i(PLAY_FIELD, col, col_tracker.y));
                    field[DECK][0].back().init_animation(animation_type, p, to_frame(0.3f));
                    transit.push_back(field[DECK][0].back());
                    field[DECK][0].pop_back();
                    card_counter++;
                    col_tracker.y++;
                    if (col_tracker.y > col_tracker.x) {
                        col_tracker.x++;
                        col_tracker.y = 0;
                    }
                }
            } else if (detailed_state == STATE_ANIMATION_RETURN_CARD) {
                if (!cursor.empty()) {
                    Card c = cursor.front();
                    Position p = c.get_position_data();
                    p.coord.second = p.coord.first;
                    p.loc.second = p.loc.first;
                    c.init_animation(STATE_ANIMATION_MOVING_CARD, p, to_frame(0.2f));
                    transit.push_back(c);
                    cursor.erase(cursor.begin());
                }
            } else if (detailed_state == STATE_ANIMATION_FLIP_DECK) {
                if (!field[DECK][1].empty()) {
                    Card c = field[DECK][1].back();
                    Position p = c.get_position_data();
                    p.coord.second = field_rect[DECK][0].getPosition();
                    p.loc.second = sf::Vector3i(DECK, 0, field[DECK][0].size());
                    c.init_animation(STATE_ANIMATION_MOVING_CARD, p, to_frame(0.3f));
                    transit.push_back(c);
                    field[DECK][1].pop_back();
                }
            } else if (detailed_state == STATE_ANIMATION_SOLVE_DECK) {
                
                //Aqui es para identificar la carta con el valor mas bajo en la izquierda
                
                Position p(sf::Vector3i(-1, -1, -1));
                int min_card_val = -1;
                for (int i = 0; i < field[PLAY_FIELD].size(); i++) {
                    if (field[PLAY_FIELD][i].empty()) continue;
                    if (min_card_val == -1 || field[PLAY_FIELD][i].back().get_value() < min_card_val) {
                        p.loc.first = sf::Vector3i(PLAY_FIELD, i, field[PLAY_FIELD][i].size() - 1);
                        p.coord.first = field[PLAY_FIELD][i].back().get_center();
                        min_card_val = field[PLAY_FIELD][i].back().get_value();
                    }
                }
                
                //Cuando el mazo se queda sin cartas
                //Revisar si encontramos la carta
                
                if (p.loc.first.x != -1) {
                    
                    //Carta que vamos a mover
                    
                    Card c = field[PLAY_FIELD][p.loc.first.y].back();
                    
                    //Ubicamos donde esta el origen para moverla
                    
                    int col;
                    for (col = 0; col < field[HOME].size(); col++) {
                        
                        //Revision si la columna esta vacia
                        
                        if (field[HOME][col].empty() && ace_locations[col] == -1) {
                            if (c.get_value() == 1) {
                                
                                //Si tenemos una carta AS poder colocarla----- REVISAR OSCAR (PREGUNTAR)
                                
                                ace_locations[col] = c.get_suit();
                                break;
                            }
                        } else if (ace_locations[col] == c.get_suit()) {
                            
                            //Encontrar una pila con el mismo mazo.
                            
                            break;
                        }
                    }
                    //launch moving animation
                    p.loc.second = sf::Vector3i(HOME, col, field[HOME][col].size() - 1);
                    p.coord.second = field_rect[HOME][col].getPosition();
                    c.init_animation(STATE_ANIMATION_MOVING_CARD, p, to_frame(0.5f));
                    transit.push_back(c);
                    field[PLAY_FIELD][p.loc.first.y].pop_back();
                }
            }
        }
        //Actualizar animaciones... ----> Buscar porque el error Thread 01. :/
        
        for (int i = 0; i < transit.size(); i++) {
            
            //Revisar si se quiere quitar la animacion
            
            if (skip) {
                transit[i].finish();
            } else {
                transit[i].next();
            }
            
            //Eliminar animaciones que están terminadas, agregarlo a su nueva ubicación la carta seleccionada
            
            if (transit[i].animation_finish()) {
                Position p = transit[i].get_position_data();
                sf::Vector3i loc = p.loc.second;
                field[loc.x][loc.y].push_back(transit[i]);
                transit.erase(transit.begin() + i--);
                
                //Cuando la animacion ha terminado
                
                if (transit.empty()) {
                    if (detailed_state == STATE_ANIMATION_INITIALIZING_DECK) {
                        if (card_counter > 27) {
                            master_state = STATE_PLAYING;
                        }
                    } else if (detailed_state == STATE_ANIMATION_FLIP_DECK) {
                        if (field[DECK][1].empty()) {
                            master_state = STATE_PLAYING;
                        }
                    } else if (detailed_state == STATE_ANIMATION_SOLVE_DECK) {
                        if (has_won()) {
                            master_state = STATE_PLAYING;
                        }
                    } else if (cursor.empty()) {
                        master_state = STATE_PLAYING;
                    }
                    
                    /*  Esto está aquí porque cualquier acción de movimiento de la tarjeta requiere animación, durante la transición,y el arrastre (requiere la libreria)
                     */
                    
                    if (!solvable && is_solvable()) {
                        solvable = true;
                    }
                }
            }
        }
    }
    if (!won && has_won()) {
        won = true;
    }
}

bool Game::valid_placement(int status, int group, int column, int row) { //Validar colocacion
    if (status == -1)
        return false;
    if (row != field[group][column].size() - 1)
        return false;
    Card c = cursor.front();
    if (status == EMPTY_SPOT) {
        
        //En caso que la columna este vacia...
        
        if (group == HOME && c.get_value() == 1 && cursor.size() == 1) { //Lugar de una AS
            ace_locations[column] = cursor.front().get_suit();
            return true;
        } else {
            if (group == PLAY_FIELD && c.get_value() == 13) {
                return true;
            }
        }
    } else {
        Card above = field[group][column][row];
        if (status == HOME) {
            if (above.get_value() + 1 == c.get_value()) {
                return above.get_suit() == c.get_suit();
            }
        } else if (status == PLAY_FIELD) {
            if (above.get_value() == c.get_value() + 1) {
                return above.get_color() ^ c.get_color();
            }
        }
    }
    return false;
}

int Game::locate_card(sf::Vector2f coord, int &group, int &column, int &row) {
    group = column = row = -1;
    for (int i = DECK; i <= PLAY_FIELD; i++) {
        for (int j = 0; j < field[i].size(); j++) {
            for (int k = field[i][j].size() - 1; k >= 0; k--) {
                Card c = field[i][j][k];
                if (c.get_sprite().getGlobalBounds().contains(coord)) {
                    
                    //Ahora esto aqui es para grabar donde se quedo la carta sin que se regrese o borre (Modificar Drag porque me tira eliminado) :/
                    
                    group = i;
                    
                    //Cartas boca abajo
                    
                    row = k;
                    column = j;
                    return i; //Regresar al grupo
                }
            }
        }
    }
    for (int i = 0; i < field_rect.size(); i++) {
        for (int j = 0; j < field_rect[i].size(); j++) {
            sf::RectangleShape r = field_rect[i][j];
            if (r.getGlobalBounds().contains(coord)) {
                group = i;
                column = j;
                row = -1;
                return EMPTY_SPOT; //Esta vacio
            }
        }
    }
    return -1;
    //Localizacion donde queremos es erronea
}

void Game::anim_move_card() {
    master_state = STATE_ANIMATION;
    detailed_state = STATE_ANIMATION_MOVING_CARD;
}

void Game::anim_flip_card() {
    master_state = STATE_ANIMATION;
    detailed_state = STATE_ANIMATION_FLIP_CARD;
}

void Game::anim_return_card() {
    master_state = STATE_ANIMATION;
    detailed_state = STATE_ANIMATION_RETURN_CARD;
    frame_delay = 0.05f;
}

void Game::anim_move_and_flip_card() {
    master_state = STATE_ANIMATION;
    detailed_state = STATE_ANIMATION_MOVE_AND_FLIP_CARD;
}

void Game::anim_flip_deck() {
    master_state = STATE_ANIMATION;
    detailed_state = STATE_ANIMATION_FLIP_DECK;
    frame_delay = 0.01f;
}

bool Game::has_won() { //En caso de ganar
    for (int i = 0; i < field[HOME].size(); i++) {
        if (field[HOME][i].size() != 13) {
            return false;
        }
    }
    return true;
}

bool Game::is_solvable() {
    if (!field[DECK][0].empty() || !field[DECK][1].empty()) {
        return false;
    }
    for (int i = 0; i < field[PLAY_FIELD].size(); i++) {
        for (int j = 0; j < field[PLAY_FIELD][i].size(); j++) {
            if (field[PLAY_FIELD][i][j].get_flip_state() == CARD_FACE_DOWN) {
                return false;
            }
        }
    }
    return true;
}

void Game::mouse_pressed(sf::Vector2f coord) {
    if (master_state == STATE_ANIMATION) {
        skip = true;
        while (master_state != STATE_PLAYING) {
            update();
        }
        skip = false;
        if (detailed_state != STATE_ANIMATION_FLIP_CARD && detailed_state != STATE_ANIMATION_MOVING_CARD) {
            return;
        }
    }
    int group, column, row;
    int status = locate_card(coord, group, column, row);
    if (status >= HOME && status <= PLAY_FIELD || status == DECK && column == 1) {
        
        //Hacer click en cualquier posicion valida del tablero
        //Revisar si el click se hizo en una carta valida
        
        
        if (status == DECK && row != field[DECK][1].size() - 1) {
            return;
        }
        //Si la carta cumple entonces darle vuelta
        
        
        if (field[group][column][row].get_flip_state() == CARD_FACE_UP) {
            
            //Agregar una carta al curso o varias
            
            mouse_down = true;
            for (int i = row; i < field[group][column].size(); i++) {
                Card c = field[group][column][i];
                Position p = c.get_position_data();
                p.coord.first = field[group][column][i].get_center();
                p.loc.first = sf::Vector3i(group, column, i);
                c.set_position_data(p);
                cursor.push_back(c);
                field[group][column].erase(field[group][column].begin() + i--);
                mouse_moved(coord);
            }
            
            
            //Comprobar si el as se mueve validamente
            
            if (cursor.size() == 1 && cursor.front().get_value() == 1) {
                for (int i = 0; i < ace_locations.size(); i++) {
                    if (ace_locations[i] == cursor.front().get_suit()) {
                        ace_locations[i] = -1;
                        break;
                    }
                }
            }
        } else {
            //Si la carta que esta enfrenete esta oculta (podeer rotar)
            if (field[group][column].size() - 1 == row) {
                anim_flip_card();
                field[group][column][row].init_animation(STATE_ANIMATION_FLIP_CARD, to_frame(0.3f));
                transit.push_back(field[group][column][row]);
                field[group][column].pop_back();
            }
        }
    } else if (status == DECK || status == EMPTY_SPOT && group == DECK) { //if deck group was clicked
        if (field[DECK][0].empty()) {
            
            //Si el mazo esta vacio (se pasaron todas) volver a todas las cartas
            
            if (!field[DECK][1].empty()) {
                anim_flip_deck();
                for (int i = 0; i < field[DECK][1].size(); i++) {
                    field[DECK][1][i].set_flip_state(CARD_FACE_DOWN);
                }
            }
        } else if (status == DECK) { //Mover y arrastrar la carta
            anim_move_card();
            
            //Registrar posiciones anteriores
            
            int flipped_counter = 1;
            if (three_card_rule) {
                flipped_counter = field[DECK][0].size() >= 3 ? field[DECK][1].size() - 3 : 0;
            }
            for (int i = flipped_counter; i < field[DECK][1].size(); i++) {
                Card c = field[DECK][1][i];
                if (c.get_center() == field_rect[DECK][1].getPosition()) continue;
                Position p = c.get_position_data();
                p.coord.second = field_rect[DECK][1].getPosition();
                c.init_animation(STATE_ANIMATION_MOVING_CARD, p, to_frame(0.25f));
                transit.push_back(c);
                field[DECK][1].erase(field[DECK][1].begin() + i--);
            }
            
            
            //Iniciar animacion de cuando se le de click a la carta
            
            flipped_counter = 1;
            if (three_card_rule) {
                flipped_counter = field[DECK][0].size() >= 3 ? 3 : field[DECK][0].size();
            }
            for (int i = 0; i < flipped_counter; i++) {
                Card c = field[DECK][0].back();
                c.set_flip_state(CARD_FACE_UP);
                Position p = c.get_position_data();
                p.coord.second = field_rect[DECK][1].getPosition();
                p.coord.second.x += HORZ_CARD_SPACING * i;
                p.loc.second = sf::Vector3i(DECK, 1, field[DECK][1].size());
                c.init_animation(STATE_ANIMATION_MOVING_CARD, p, to_frame(0.25f));
                transit.push_back(c);
                field[DECK][0].pop_back();
            }
        }
    }
}

void Game::mouse_released(sf::Vector2f coord) {
    if (mouse_down) {
        mouse_down = false;
        int group, column, row;
        int status = locate_card(coord, group, column, row);
        if (valid_placement(status, group, column, row)) {
            anim_move_card();
            for (int i = 0; i < cursor.size(); i++) {
                Position p = cursor[i].get_position_data();
                p.loc.second = sf::Vector3i(group, column, row + i + 1);
                p.coord.second = field_rect[group][column].getPosition();
                if (group != HOME) {
                    p.coord.second.y += VERT_CARD_SPACING * (row + i + 1);
                }
                cursor[i].init_animation(STATE_ANIMATION_MOVING_CARD, p, to_frame(0.1f));
                transit.push_back(cursor[i]);
            }
            cursor.clear();
        } else {
            anim_return_card();
        }
    }
}

void Game::mouse_moved(sf::Vector2f coord) {
    if (mouse_down) {
        for (int i = 0; i < cursor.size(); i++) {
            cursor[i].set_cursor_hold_position(sf::Vector2f(coord.x, coord.y + VERT_CARD_SPACING * i));
        }
    }
}

void Game::mouse_left() {
    if (mouse_down) {
        mouse_down = false;
        anim_return_card();
    }
}

void Game::flip_draw_mode() {
    three_card_rule = !three_card_rule;
    reset_game();
}

void Game::auto_solve() {
    if (solvable) {
        if (master_state == STATE_ANIMATION) {
            skip = true;
            while (master_state == STATE_ANIMATION) {
                update();
            }
            skip = false;
        }
        master_state = STATE_ANIMATION;
        detailed_state = STATE_ANIMATION_SOLVE_DECK;
        frame_delay = 0.1f;
    }
}

bool Game::get_kill() {
    return kill;
}

void Game::draw(sf::RenderWindow &window) {
    
    //Dibujar todos los rectangulos
    
    for (int i = 0; i < field_rect.size(); i++) {
        for (int j = 0; j < field_rect[i].size(); j++) {
            window.draw(field_rect[i][j]);
        }
    }
    
    
    //Si el mazo principal no esta vacio entocnes...
    
    if (!field[DECK][0].empty()) {
        field[DECK][0].back().draw(window, backside);
    }
    //Dibujar las cartas en una baraja para sacar
    int flipped_counter = field[DECK][1].size() - 3; //Aquiiiiiii -->>>>!!!!!!!!!
    if (field[DECK][1].size() < 3) {
        flipped_counter = 0;
    }
    for (int i = flipped_counter; i < field[DECK][1].size(); i++) {
        field[DECK][1][i].draw(window, backside);
    }
    //Dibujar las cartas del jugador...
    
    for (int i = 0; i < field[HOME].size(); i++) {
        if (!field[HOME][i].empty()) {
            field[HOME][i].back().draw(window, backside);
        }
    }
    
    //Ahora aqui se dibujan las cartas en la ventana....
    
    
    for (int i = 0; i < field[PLAY_FIELD].size(); i++) {
        for (int j = 0; j < field[PLAY_FIELD][i].size(); j++) {
            field[PLAY_FIELD][i][j].draw(window, backside);
        }
    }
    //Estado de la animacion...
    
    for (int i = 0; i < transit.size(); i++) {
        transit[i].draw(window, backside);
    }
    
    //Cuando el cursor presione la carta
    
    for (int i = 0; i < cursor.size(); i++) {
        cursor[i].draw(window, backside);
    }
    
    //pODER ver el arrastre
    
    if (won) {
        window.draw(won_text);
    } else if (solvable) {
        window.draw(solvable_text);
    }
    window.draw(info_text);
}

//Investigado de l alibreria ...
float Game::to_frame(float time) {
    //1 second = 60 FPS
    return time * FPS;
}
