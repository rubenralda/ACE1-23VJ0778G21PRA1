#include "LedControl.h"
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define DIN 5
#define CS 6
#define CLK 7
#define DEV 1 
LedControl lc=LedControl(DIN,CLK,CS,DEV);

#define HEIGHT 8
#define LENGTH 16

typedef enum{
    MOV_LEFT,
    MOV_RIGHT,
}movement; // dirección del desplazamiento, hacia la izquierda o derecha

int canvas[HEIGHT][LENGTH] = { 0 }; //lienzo x*y
uint8_t global_lives = 3; // Por defecto el juego comienza con 3 vidas
long int global_delay = 1000; // delay global

typedef struct{
    uint8_t     lives;      // vidas
    uint8_t     level;      // nivel del juego
    uint8_t     score;      // puntación
    uint8_t     curr_score; // puntuación en el nivel actual
    movement    direction;  // dirección a la que se mueve el avión
    uint8_t     front;      // parte delantera
    uint8_t     middle;     // parte central
    uint8_t     rear;       // parte trasera
    uint8_t     bottom;     // altura de abajo
    bool        bomb;       // determina si el avión está lanzando un proyectil
}plane;
plane current_plane;

uint8_t towers[LENGTH] = { 0 }; // Torres del juego, el indice es x, el valor es la altura de la torre
uint8_t bombX,bombY;            // posición de una bomba siendo lanzada
bool    is_tower = false;       // marca si la posisión en la que se lanzó un proyectil tiene un torre

uint8_t x_pos(movement,uint8_t,uint8_t,uint8_t);
void clean_canvas();
void render_canvas();
void gameloop();
void newgame();
void next_level();
void generate_towers(uint8_t);
void destroy_tower(uint8_t);
void check_collision();
void add_towers();
void reset_towers();
void reset_plane();
void remove_plane();
void launch_bomb();
void bombloop();
void check_score();

/*
 *  Hace que el lienzo vuelta a estar conformado por sólo 0's
 */
void clean_canvas(){
    for(int y = 0; y < HEIGHT ; y++){
        for(int x = 0 ; x < LENGTH; x++){
            canvas[y][x]=0;
        }
    }
}

void render_canvas(){
    for(int y = 0; y < HEIGHT ; y++){
        for(int x = 0 ; x < LENGTH; x++){
            if(x>7){
                //matriz sin driver
            }else{
                //matriz con driver
            }
        }
    }
}

/*
 * Ejecuta el lanzamiento de una bomba
 */
void launch_bomb(){
    if (!current_plane.bomb){
        current_plane.bomb = true;
        bombX = current_plane.middle;
        bombY = current_plane.bottom;
        if( towers[current_plane.middle] != 0 ){
            is_tower = true;
        }
    }
}

/*
 * Mueve un proyectil hacia abajo una unidad;
 */
void bombloop(){
    if (current_plane.bomb){
        canvas[bombY][bombX] = 0 ;
        if(towers[bombX]!=0 && towers[bombX] == 7 - bombY ){
            towers[bombX]=towers[bombX]-1;
        }
        bombY++;
        if(bombY>7){
            if(is_tower){
                current_plane.score++;
                current_plane.curr_score++;
                if(current_plane.score % 5 == 0){
                    current_plane.lives++;
                }
            }
            current_plane.bomb = false;
            is_tower = false;
            check_score();
        }else{
            canvas[bombY][bombX] = 1 ;
        }
    }
}

/*
 *  Determina las coordenadas que le tocan a una parte del avión
 */
uint8_t x_pos(movement direction,uint8_t current,uint8_t step = 1,uint8_t max = LENGTH - 1){
    if(direction == MOV_LEFT){
        uint8_t new_pos = current + step;
        return (new_pos > max) ? new_pos - max -1 : new_pos; 
    }else{
        return (current<step) ? max - ( step-current ) + 1 : current - step;
    }
}

/*
 *  Cambia la posición del avión en el lienzo
 */
void move_plane(){
    remove_plane();
    current_plane.front = x_pos(current_plane.direction,current_plane.front);
    current_plane.middle = x_pos(current_plane.direction,current_plane.middle);
    current_plane.rear = x_pos(current_plane.direction,current_plane.rear);
    canvas[current_plane.bottom][current_plane.front] = 1;
    canvas[current_plane.bottom][current_plane.middle] = 1;
    canvas[current_plane.bottom][current_plane.rear] = 1;
    canvas[current_plane.bottom-1][(current_plane.direction == MOV_LEFT)? current_plane.rear : current_plane.front] = 1 ;
    check_collision();
}

/*
 * Mueve el avión hacia abajo una unidad
 */
void move_plane_down(){
    remove_plane();
    current_plane.bottom = (current_plane.bottom + 1 < 7 ) ? current_plane.bottom + 1 : 7;
    canvas[current_plane.bottom][current_plane.front] = 1;
    canvas[current_plane.bottom][current_plane.middle] = 1;
    canvas[current_plane.bottom][current_plane.rear] = 1;
    canvas[current_plane.bottom-1][(current_plane.direction == MOV_LEFT)? current_plane.rear : current_plane.front] = 1 ;
    check_collision();
}

/*
 *  Remueve el avión del lienzo, y lo regresa a su posisión por defecto
 */
void reset_plane(){
    remove_plane();
    current_plane.front = 2,
    current_plane.middle = 1,
    current_plane.rear = 0,
    current_plane.bottom = 1,
    canvas[current_plane.bottom][current_plane.front] = 1;
    canvas[current_plane.bottom][current_plane.middle] = 1;
    canvas[current_plane.bottom][current_plane.rear] = 1;
    canvas[current_plane.bottom-1][(current_plane.direction == MOV_LEFT)? current_plane.rear : current_plane.front] = 1 ;
}

/*
 * Remueve del lienzo unicamente el avión, para cuando se tiene que mover
 */
void remove_plane(){
    canvas[current_plane.bottom][current_plane.front] = 0;
    canvas[current_plane.bottom][current_plane.middle] = 0;
    canvas[current_plane.bottom][current_plane.rear] = 0;
    canvas[current_plane.bottom-1][(current_plane.direction == MOV_LEFT)? current_plane.rear : current_plane.front] = 0 ;
}

/*
 * Revisa si el avión sufre una colisión con una torre
 */
void check_collision(){
    if(towers[current_plane.rear]!=0 ||
            towers[current_plane.middle]!=0 ||
            towers[current_plane.front]!=0)
    {
        if(8-towers[current_plane.rear]<=current_plane.bottom ||
            8-towers[current_plane.middle]<=current_plane.bottom ||
            8-towers[current_plane.front]<=current_plane.bottom)
        {
            if(current_plane.lives == 0){
                // implementar GAMEOVER
                newgame();
            }
                current_plane.lives--;
                reset_plane();
                add_towers();
                render_canvas();
        }
    }
}

/*
 * Revisa si hay que pasar al siguiente nivel
 */
void check_score(){
    if(current_plane.curr_score == current_plane.level ){
        next_level();
    }
}

/*
 *  Genera las torres aleatoriamente
 */
void generate_towers(uint8_t nivel){
    uint8_t counter =0;
    while(counter<nivel){
        uint8_t randomY = (rand() % 4 ) + 1 ;
        uint8_t randomX = rand() % LENGTH;
        if(towers[randomX]== 0 ){
            towers[randomX] = randomY;
            counter++;
        }
    }
    clean_canvas();
    add_towers();
}

/*
 *  Pone las torres en el lienzo
 */
void add_towers(){
    for(int i = 0 ; i<LENGTH ; i++){
        for(int y=0; y < towers[i] ;y++ ){
            canvas[7-(y)][i] = 1;
        }
    }
}

/*
 * Reinicia el arreglo de torres
 */
void reset_towers(){
    for(int i = 0 ; i<LENGTH ; i++){
        towers[i] = 0;
    }
}

/*
 * Hace que el avión se mueva una unidad
 */
void gameloop(){
    move_plane();
    /*
    if(towers[current_plane.middle]!=0){//modo automatico xd
        launch_bomb();
    }
    */
    render_canvas();
    bombloop();
    render_canvas();
}

/* TODO: Animación de cambio de nivel
 * Inicia un nuevo nivel
 */
void next_level(){

    clean_canvas();
    reset_towers();

    current_plane.direction = MOV_LEFT;
    //current_plane.direction = (current_plane.direction == MOV_LEFT) ? MOV_RIGHT : MOV_LEFT ;
    current_plane.front = 2;
    current_plane.middle = 1;
    current_plane.rear = 0;
    current_plane.bottom = 1;
    current_plane.curr_score = 0;
    current_plane.bomb = false;
    current_plane.level = (current_plane.level + 1 > 16) ? 16 : current_plane.level+1;

    canvas[current_plane.bottom][current_plane.front] = 1;
    canvas[current_plane.bottom][current_plane.middle] = 1;
    canvas[current_plane.bottom][current_plane.rear] = 1;
    canvas[current_plane.bottom-1][(current_plane.direction == MOV_LEFT) ? current_plane.rear : current_plane.front] = 1 ;

    generate_towers(current_plane.level);
    add_towers();
    gameloop();
}

/*
 * Inicia el juego con valores predeterminados
 */
void newgame(){
    clean_canvas();
    reset_towers();
    current_plane.lives = global_lives;
    current_plane.direction = MOV_LEFT;
    current_plane.front = 2;
    current_plane.middle = 1;
    current_plane.rear = 0;
    current_plane.bottom = 1;
    current_plane.curr_score = 0;
    current_plane.score = 0;
    current_plane.bomb = false;
    current_plane.level = 1;

    canvas[current_plane.bottom][current_plane.front] = 1;
    canvas[current_plane.bottom][current_plane.middle] = 1;
    canvas[current_plane.bottom][current_plane.rear] = 1;
    canvas[current_plane.bottom-1][(current_plane.direction == MOV_LEFT) ? current_plane.rear : current_plane.front] = 1 ;
    generate_towers(1);
    add_towers();
    gameloop();
}

void setup() {
  // put your setup code here, to run once:
  
}

void loop() {
  // put your main code here, to run repeatedly:
}
