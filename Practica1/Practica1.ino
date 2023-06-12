#include "LedControl.h"
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#define Num_Matrices 2
#define Matriz_Size 8
#define ROW_NUM 8
#define COL_NUM 16

// BOTONES
#define RIGHT 6
#define LEFT 7
#define START 4
#define BTN_K 3
LedControl matrix_driver = LedControl(51, 53, 52, 1);

// BUFFER PARA AMBAS MATRICES
int buffer[8][16];

void limpiarBuffer(){
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 16; col++) {
      buffer[row][col] = 0;
    }
  }
}

// ------------------------------------------------------
// -------------------- ESTADÍSTICAS --------------------
int punteos[] = {2,1,0,0,0}; // Stack de Punteos
int stats[] = {0,0,0,0,0}; // Stack de Stats (0-8)
int bufferStats[8][16];


void limpiarBufferStats(){
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 16; col++) {
      bufferStats[row][col] = 0;
    }
  }
}

void calcularStat(int indiceStat, int sumatoria){
  if (sumatoria == 0){
    stats[indiceStat] = 1;
    return;
  }
  float porcentaje = (float(punteos[indiceStat]) / sumatoria * 100);
  int stat = map(porcentaje, 0, 100, 1, 8); // Mapear, valor, 0-100 porcentaje, 1-8 valor barra en matriz
  stats[indiceStat] = stat;
}

void generarStats(){
  int sumatoria = punteos[0] + punteos[1] + punteos[2] + punteos[3] + punteos[4];
  for (int i = 0; i <= 4; i++){ // Por cada punteo
    calcularStat(i, sumatoria); // Cálculo de estadística 0-8
  }
}

void agregarPunteo(int nuevoPunteo){ // Añade al Stack de Punteos
  for (int i = 4; i <= 1; i--){ // Por cada punteo
    punteos[i] = punteos[i-1];  // Se corre una posición
  }
  punteos[0] = nuevoPunteo;     // Se agrega nuevo punteo
  generarStats();               // Genera las estadísticas del 0-8
}

void actualizarBufferStats(){
  int col = 1;
  for(int stat = 0; stat < 5; stat++){ // Por cada Stat
    for(int i=0; i<2; i++){ // 2 Veces (2 columnas)
      if(stats[stat] != 0){
        for (int row = 7; row > (7 - stats[stat]); row--) {       
          bufferStats[row][col] = 1;      
        }
      }
      col++;
    }
    col++;
  }
}
// -------------------- ESTADÍSTICAS --------------------
// ------------------------------------------------------



//*************MENSAJE ----- MENU****************
int contMenu=0;

unsigned long delaytime = 100;

// MATRIZ NODRIVER
int filas_no_driver[] = {50, 49, 48, 47, 46, 45, 44, 43};
int columnas_no_driver[] = {42, 41, 40, 39, 38, 37, 36, 35};

// TABLERO DE JUEGO
int tablero_de_juego[ROW_NUM][COL_NUM] = {}; // tablero para el juego


int numeroContador[][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};


int valor_potenciometro = 0;
// INICIALIZACION DEL JUEGO
bool iniciaJuego = false; // Iniciar juego nuevo
bool enJuego = false; // Jugando 

// VARIABLE PARA ESTAR EN MENSAJE
bool enMensaje =true;
// VARIABLE PARA ESTAR EN EL MENU
bool enMenu = false;
// VARIABLE PARA ESTAR EN LA CONFIGURACION
bool enConfiguracion = false;
// VARIABLE PARA ESTAR EN LA ESTADISCTICAS
bool enStast=false;

//VARIABLE PARA CAMBIO DE DIRECCION
bool derecha=false;

//VARIABLE PARA ENPAUSA
bool enPausa=false;


//valor numerico de  tiempo presionado en algun boton
int pressTime = 0;
//Mensaje

const int cantidad_columnas = 128;
const int cantidad_columnas2 = 16;
int posicion_cadena = 0;
int posicion_cadena2 = 0;
int posicion_cadena1 = 0;
int posicion_cadena22 = 0;
byte cadena[8][cantidad_columnas] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0},
  {0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0},
  {0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
  {0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1},
  {0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0},
  {0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};


byte cadena2[8][cantidad_columnas2] = {
  {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
  {0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0},
  {0, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0},
  {0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0},
  {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}
};

//control de direccion

bool movimiento_inv = false;
bool temporal_movimiento_inv = false;
//Controlan el tiempo de recorrido de la cadena
long prev_MillisM = 0;

//*************MENAJE ------ MENU****************

typedef enum{
    MOV_LEFT,
    MOV_RIGHT,
}movement; // dirección del desplazamiento, hacia la izquierda o derecha

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
    unsigned long   plane_time; // reloj del avión
}plane;
plane current_plane;

uint8_t towers[COL_NUM] = { 0 }; // Torres del juego, el indice es x, el valor es la altura de la torre
uint8_t bombX,bombY;            // posición de una bomba siendo lanzada
bool    is_tower = false;       // marca si la posisión en la que se lanzó un proyectil tiene un torre

/*** Métodos de juego  ***/
uint8_t x_pos(movement,uint8_t,uint8_t,uint8_t);
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
void gamepad_action(char);

void mostrarNumero(int num, int seconds, bool mostrar_tick = false);

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
        tablero_de_juego[bombY][bombX] = 0 ;
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
            tablero_de_juego[bombY][bombX] = 1 ;
        }
    }
}

/*
 *  Determina las coordenadas que le tocan a una parte del avión
 */
uint8_t x_pos(movement direction,uint8_t current,uint8_t step = 1,uint8_t max = COL_NUM - 1){
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
    tablero_de_juego[current_plane.bottom][current_plane.front] = 1;
    tablero_de_juego[current_plane.bottom][current_plane.middle] = 1;
    tablero_de_juego[current_plane.bottom][current_plane.rear] = 1;
    tablero_de_juego[current_plane.bottom-1][(current_plane.direction == MOV_LEFT)? current_plane.rear : current_plane.front] = 1 ;
    check_collision();
}

/*
 * Mueve el avión hacia abajo una unidad
 */
void move_plane_down(){
    remove_plane();
    current_plane.bottom = (current_plane.bottom + 1 < 7 ) ? current_plane.bottom + 1 : 7;
    tablero_de_juego[current_plane.bottom][current_plane.front] = 1;
    tablero_de_juego[current_plane.bottom][current_plane.middle] = 1;
    tablero_de_juego[current_plane.bottom][current_plane.rear] = 1;
    tablero_de_juego[current_plane.bottom-1][(current_plane.direction == MOV_LEFT)? current_plane.rear : current_plane.front] = 1 ;
    check_collision();
}

/*
 * Realiza acciones de acuerdo al input dado por el gamepad 
 */
void gamepad_action(char button){
    if(button == 'r'){
        current_plane.direction = MOV_LEFT;
    }else if(button == 'l'){
        current_plane.direction = MOV_RIGHT;
    }else if(button == 'k'){
        if(enPausa){
            long int pressStart = millis();
            bool toMenu = false;
            bool toGame = false;
            while(digitalRead(BTN_K)==HIGH){ 
                //pressTime++;
                //delay(100);
                mostrarNumero(current_plane.lives,1,true);
                long int comp = millis() - pressStart;
                if(comp> 2900){
                    Serial.print(comp);
                    Serial.println(" REGRESAR AL MENU");
                    toMenu = true;
                }else if (comp < 3000 && comp > 1900){
                    Serial.print(comp);
                    Serial.println("REGRESAR AL JUEGO");
                    toGame = true;
                }else{
                    Serial.print(comp);
                    Serial.println("QUEDARSE EN PAUSA"); 
                }
            }
            if(toMenu){
                // Si k se presiona durante 3 segundos mientras el juego está pausado, se regresa al menú principal
                enPausa = false;
                enJuego = false;
                enMensaje=false;
                enMenu=true;
                iniciaJuego=false;
                enConfiguracion=false;
                pressTime = 0;
            }else if(toGame){
                enPausa = false;
                enJuego = true;
                enMensaje=false;
                enMenu=false;
                iniciaJuego=false;
                enConfiguracion=false;
                pressTime = 0;

            }
        }else{
            enPausa = true;
            enJuego = false;
            enMensaje=false;
            enMenu=false;
            iniciaJuego=false;
            enConfiguracion=false;
        }
        // menú pausa
    }else if(button == 's'){
        launch_bomb();
    }
}
/*
 *  Remueve el avión del lienzo, y lo regresa a su posisión por defecto
 */
void reset_plane(){
    remove_plane();
    //current_plane.front = 2,
    //current_plane.middle = 1,
    //current_plane.rear = 0,
    current_plane.bottom -=2 ,
    tablero_de_juego[current_plane.bottom][current_plane.front] = 1;
    tablero_de_juego[current_plane.bottom][current_plane.middle] = 1;
    tablero_de_juego[current_plane.bottom][current_plane.rear] = 1;
    tablero_de_juego[current_plane.bottom-1][(current_plane.direction == MOV_LEFT)? current_plane.rear : current_plane.front] = 1 ;
}

/*
 * Remueve del lienzo unicamente el avión, para cuando se tiene que mover
 */
void remove_plane(){
    tablero_de_juego[current_plane.bottom][current_plane.front] = 0;
    tablero_de_juego[current_plane.bottom][current_plane.middle] = 0;
    tablero_de_juego[current_plane.bottom][current_plane.rear] = 0;
    tablero_de_juego[current_plane.bottom-1][(current_plane.direction == MOV_LEFT)? current_plane.rear : current_plane.front] = 0 ;
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
                //GAMEOVER;
                enPausa = false;
                enJuego = false;
                enMensaje=true;
                enMenu=false;
                iniciaJuego=false;
                enConfiguracion=false;
            }
                current_plane.lives--;
                reset_plane();
                add_towers();
                //render_tablero_de_juego();
                draw();
        }
    }
}

/*
 * Revisa si hay que pasar al siguiente nivel
 */
void check_score(){
    if(current_plane.curr_score == current_plane.level + 2){
        next_level();
    }
}

/*
 *  Genera las torres aleatoriamente
 */
void generate_towers(uint8_t nivel){
    uint8_t counter =0;
    while(counter<nivel + 2){
        uint8_t randomY = (rand() % 4 ) + 1 ;
        uint8_t randomX = rand() % COL_NUM;
        if(towers[randomX]== 0 ){
            towers[randomX] = randomY;
            counter++;
        }
    }
    limpiarTableroDeJuego();
    add_towers();
}

/*
 *  Pone las torres en el lienzo
 */
void add_towers(){
    for(int i = 0 ; i<COL_NUM ; i++){
        for(int y=0; y < towers[i] ;y++ ){
            tablero_de_juego[7-(y)][i] = 1;
        }
    }
}

/*
 * Reinicia el arreglo de torres
 */
void reset_towers(){
    for(int i = 0 ; i<COL_NUM ; i++){
        towers[i] = 0;
    }
}

/*
 * Hace que el avión se mueva una unidad
 */
void gameloop(){
    move_plane();
    if(millis() - current_plane.plane_time > 1000 ){
        current_plane.plane_time = millis();
        move_plane_down();
    }
    /*
    if(towers[current_plane.middle]!=0){//modo automatico xd
        launch_bomb();
    }
    */
    //render_tablero_de_juego();
    draw();
    bombloop();
    //render_tablero_de_juego();
    draw();
}

/* TODO: Animación de cambio de nivel
 * Inicia un nuevo nivel
 */
void next_level(){
    limpiarTableroDeJuego();
    reset_towers();

    current_plane.direction = MOV_LEFT;
    //current_plane.direction = (current_plane.direction == MOV_LEFT) ? MOV_RIGHT : MOV_LEFT ;
    current_plane.front = 2;
    current_plane.middle = 1;
    current_plane.rear = 0;
    current_plane.bottom = 1;
    current_plane.curr_score = 0;
    current_plane.bomb = false;
    current_plane.level = (current_plane.level + 1 > 14) ? 14 : current_plane.level+1;
    mostrarNumero(current_plane.level,2);
    current_plane.plane_time = millis();
    tablero_de_juego[current_plane.bottom][current_plane.front] = 1;
    tablero_de_juego[current_plane.bottom][current_plane.middle] = 1;
    tablero_de_juego[current_plane.bottom][current_plane.rear] = 1;
    tablero_de_juego[current_plane.bottom-1][(current_plane.direction == MOV_LEFT) ? current_plane.rear : current_plane.front] = 1 ;

    generate_towers(current_plane.level);
    add_towers();
    gameloop();
}

/*
 * Inicia el juego con valores predeterminados
 */
void newgame(){
    limpiarTableroDeJuego();
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
    current_plane.plane_time = millis();

    tablero_de_juego[current_plane.bottom][current_plane.front] = 1;
    tablero_de_juego[current_plane.bottom][current_plane.middle] = 1;
    tablero_de_juego[current_plane.bottom][current_plane.rear] = 1;
    tablero_de_juego[current_plane.bottom-1][(current_plane.direction == MOV_LEFT) ? current_plane.rear : current_plane.front] = 1 ;
    generate_towers(1);
    add_towers();
    mostrarNumero(current_plane.level,2);
    gameloop();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  randomSeed(analogRead(0)); 

  // Definicion de los pines de salida para las filas y columnas
  for (int i = 0; i < 8; i++) {
    pinMode(filas_no_driver[i], OUTPUT);
    pinMode(columnas_no_driver[i], OUTPUT);
  }

  // inicializar matriz con driver
  matrix_driver.shutdown(0, false);
  matrix_driver.setIntensity(0, 8);
  matrix_driver.clearDisplay(0);
  // inicializar matriz sin driver
  clearMatrizNoDriver();

  // botones del gamepad
  pinMode(RIGHT, INPUT);
  pinMode(LEFT, INPUT);
  pinMode(START, INPUT);
  pinMode(BTN_K, INPUT);

 //Inicialización de Estadísticas
  generarStats();
  limpiarBufferStats();
  actualizarBufferStats();

}

void loop() {
  //Implementar acciones segun condiciones
  if(enMensaje){
    Serial.println("en mensaje");
    gamePad();
    mostrarMensaje();

  }else if (enMenu) {
    Serial.println("en menu");
    menu();
  } else if(enConfiguracion) {
    Serial.println("en config");
    configuracion();
  } else if(enPausa){
    Serial.println("en pausa");
    mostrarNumero(current_plane.lives,1,true);
    gamepad_action(gamePad());
    return;
  } else if(iniciaJuego){
    Serial.println("nuevo juego");
    newgame();
    iniciaJuego = false;
    enJuego = true;
  } else if (enJuego){
    Serial.println("en juego");
    limpiarMatrices();
    gameloop();
    gamepad_action(gamePad());
    delay(delaytime);
  }
}



// TODO: implementar el menu principal el cual
// despliega el mensaje
void menu() {  

  //mostrar mensaje
  //mostrarMensaje();
  if(contMenu==0){
    limpiarMatrices();
   for (int columna = 0; columna < 8; columna++) {
      for (int fila = 0; fila < 8; fila++) {
      numeroContador[columna][fila] = numeroContador[columna][fila];
      }
    }
    pintar();
    pintar();
    pintar();
    delay(10);
    contMenu=1;
  }else{
    mostrarMensaje2();
    //delay(1);
  }


  if(gamePad()=='l'){
    Serial.println("Juego");
    enMenu=false;
    iniciaJuego=true;
    
  }else if(gamePad()=='s'){
    Serial.println("stats");
    mostrarStats();
    delay(150);

  }else if(gamePad()=='r'){
    Serial.println("enConfig");
    enMenu=false;
    enConfiguracion=true;
  }

}

//Muestra la cantidad de vidas en el 
char gamePad() {
  if (digitalRead(RIGHT) == HIGH) {
    return 'r';

  } else if (digitalRead(LEFT) == HIGH) {
    return 'l';

  } else if (digitalRead(START) == HIGH) {
    if(enMensaje==true && enMenu==false && iniciaJuego==false && enConfiguracion == false){
      
      Serial.println("Cambio de direccion");
      delay(50);
      if(temporal_movimiento_inv==false){
        Serial.println("izquierda");
        temporal_movimiento_inv = true;
        movimiento_inv = temporal_movimiento_inv;
        //derecha=true;
      }else if(temporal_movimiento_inv== true){
        Serial.println("derecha");        
        temporal_movimiento_inv = false;
        movimiento_inv = temporal_movimiento_inv;
        //derecha =false;
      }
      delay(50);
    }  
    return 's';
  }else if(digitalRead(BTN_K)==HIGH){
      if(enPausa){
        return 'k';
      }
      pressTime++;
      delay(50); // works like a timer with one second steps
      Serial.println(pressTime);
      if (pressTime >= 30)
      {
        if(enMensaje == true && enMenu==false && iniciaJuego==false && enConfiguracion == false){
          
          Serial.println("3seg");
          enMensaje=false;
          enMenu=true;
          iniciaJuego=false;
          enConfiguracion = false;
          pressTime=0;
        
        }if(enMensaje == false && enMenu==false && iniciaJuego==true && enConfiguracion == false){
          
          Serial.println("3seg");
          enMensaje=false;
          enMenu=true;
          iniciaJuego=false;
          enConfiguracion = false;
          pressTime=0;
        
        }
        if(enMensaje == false && enMenu==false && iniciaJuego==false && enConfiguracion == true){
          
          Serial.println("3seg config");
          enMensaje=false;
          enMenu=true;
          iniciaJuego=false;
          enConfiguracion = false;
          pressTime=0;
        
        }
        /*
        if(enPausa){
            Serial.println("3seg REGRESAR AL MENÚ");
            // Si k se presiona durante 3 segundos mientras el juego está pausado, se regresa al menú principal
            enPausa = false;
            enJuego = false;
            enMensaje=false;
            enMenu=true;
            iniciaJuego=false;
            enConfiguracion=false;
            pressTime = 0;

        }*/
      }/*  else if( enPausa && pressTime > 19 && pressTime < 30) {
            // Si k se presiona por 2 segundos, mientras se está en pausa, el juego se reanuda
             
           Serial.println("2 seg REANUNDANDO JUEGO");
           enPausa = false;
           enJuego = true;
           return ' '; 
          
        }   */   
       
    return 'k';
  }
  pressTime = 0;
  return ' ';
}


void limpiarTableroDeJuego() {
  // limpiaremos el tablero de juego
  for (int i = 0; i < ROW_NUM; i ++) {
    for (int j = 0; j < COL_NUM; j++) {
      tablero_de_juego[i][j] = 0;
    }
  }
}

void actualizarMatrices(int matriz_izquierda[ROW_NUM][COL_NUM/2], int matriz_derecha[ROW_NUM][COL_NUM/2]) {
  //Lo primero que hay que hacer es un barrido por columnas
  for (int i = 0; i < COL_NUM/2; i++) {
    digitalWrite(columnas_no_driver[i], HIGH);
    // despues de prender la columna actual debemos
    // pintar las filas que se encuentran prendidas
    for (int j = 0; j < ROW_NUM; j++) {
      // despues comprobar si hay que pintar una posicion
      // en la matriz sin driver
      if (matriz_derecha[j][i] == 1) {
        digitalWrite(filas_no_driver[j], LOW);
      } else {
        digitalWrite(filas_no_driver[j], HIGH);
      }
      //actualizando la matriz con driver
      matrix_driver.setLed(0, i, j, matriz_izquierda[i][j]);
    }
    delay(1);
    //limpiando para la matriz sin driver
    digitalWrite(columnas_no_driver[i], LOW);
    for (int j = 0; j < ROW_NUM; j++) {
      digitalWrite(filas_no_driver[j], HIGH);
    }
  }
}

void draw() {
   // matriz con driver
   int matriz_izquierda[ROW_NUM][COL_NUM/2] = {};
   // matriz sin driver
   int matriz_derecha[ROW_NUM][COL_NUM/2] = {};
  
  // este loop hace que se pueda escribir dentro de la matriz de led
  for (int i = 0; i < ROW_NUM; i++) {
    for (int j = 0; j < COL_NUM; j++) {
      //si la columna es menor que 8, esta pintara en la matriz con driver (izquierda)
      //de lo contrario pintara en la matriz sin driver (derecha)
      if (j < COL_NUM/2) {
        matriz_izquierda[i][j] = tablero_de_juego[i][j];
      } else {
        matriz_derecha[i][j-8] = tablero_de_juego[i][j];
      }
    }
  }
  // actualizacion de las matrices
  actualizarMatrices(matriz_izquierda, matriz_derecha);
}

void limpiarMatrices() {
  // limpiaremos los displays de las matrices.
  matrix_driver.clearDisplay(0);
  for (int i = 0; i < (COL_NUM / 2); i++) {
    digitalWrite(columnas_no_driver[i], LOW);
    for (int j = 0; j < ROW_NUM; j++) {
      digitalWrite(filas_no_driver[j], HIGH);
    }
  }
}


void pintar() {
  for (int i = 0; i < 8; i++) {
    digitalWrite(filas_no_driver[i], LOW);
    for (int j = 0; j < 8; j++) {
      if (numeroContador[i][j] == 1) {
        digitalWrite(columnas_no_driver[j], HIGH);
      }
    }
    delay(1);
    digitalWrite(filas_no_driver[i], HIGH);
    for (int j = 0; j < 8; j++) {
      digitalWrite(columnas_no_driver[j], LOW);
    }
  }
}




void mostrarMensaje() {
  unsigned long currentMillis = millis();
 
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 16; j++) {
      if(cadena[i][j + posicion_cadena2]==0 ){
        tablero_de_juego[i][j]=0;
      }else{ 
        tablero_de_juego[i][j]=1;
      } 
    }
    delay(1);
  }

  if (currentMillis - prev_MillisM > delaytime) {
    prev_MillisM = currentMillis;
    //cadena de derecha a izquierda = true
    if (movimiento_inv == true) {
      
      posicion_cadena2 = (posicion_cadena2 == (cantidad_columnas - 1 )) ? 0 : posicion_cadena2 + 1;
      posicion_cadena = (posicion_cadena == 0) ? cantidad_columnas - 2 : posicion_cadena - 1;

    } else {
      posicion_cadena2 = (posicion_cadena2 == 0) ? cantidad_columnas - 2 : posicion_cadena2 - 1;
      posicion_cadena = (posicion_cadena == (cantidad_columnas - 1)) ? 0 : posicion_cadena + 1;
    }
   
  }

  
 draw();
 limpiarTableroDeJuego();
}


void mostrarMensaje2() {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 16; j++) {
      if(cadena2[i][j ]==0 ){
        tablero_de_juego[i][j]=0;
      }else{
        tablero_de_juego[i][j]=1;
      } 
    }
    delay(1);
  }
  draw();
  limpiarTableroDeJuego();
}

void mostrarStats() { 
  long int t0 = 0;
  long int t1 = 0;
  bool presionado_actualmente = false;
  while(1){
    unsigned long currentMillis = millis(); 
    for (int i = 0; i < 8; i++) {   
      for (int j = 0; j < 16; j++) {  
        if(bufferStats[i][j]==0 ){     
          tablero_de_juego[i][j]=0;       
        }else{       
          tablero_de_juego[i][j]=1;
        }      
      }
      delay(1);
    } 
  draw();
  limpiarTableroDeJuego();

  // Contador Botón 3 segundos para salir 
  t1 = millis();
    if(digitalRead(BTN_K)==HIGH) { // BOTON K
        if (!presionado_actualmente) {
            presionado_actualmente = true;
            t0 = millis();
        } else {
            long int diferencia = t1 - t0;            
            if (diferencia >= 2800 && diferencia <= 3300) {   // APROX 3 SEG              
                if (!digitalRead(2)) { 
                    limpiarTableroDeJuego();
                    delay(100);
                    break;
                }
            }
        }
    } else {
        presionado_actualmente = false;
    }
  }
}


void clearMatrizNoDriver() {
  for (int i = 0; i < COL_NUM/2; i++) {
    digitalWrite(columnas_no_driver[i], LOW);
    for (int j = 0; j < ROW_NUM; j++) {
     digitalWrite(filas_no_driver[j], HIGH);
    }
  }
}

//configuracion del juego y el mensaje
void configuracion() {
  gamePad();
  int potenciometro_velocidad = map(analogRead(A0), 0, 1024, 100, 0);
  //int mostrar_velocidad = 10 - (potenciometro_velocidad*10)/100; 
  int mostrar_velocidad = 10 - potenciometro_velocidad/10;//porcentaje de la velocidad por regla de 3
  delaytime = potenciometro_velocidad;
  int potenciometro_vidas = map(analogRead(A8), 0, 1000, 3, 10);
  global_lives = potenciometro_vidas;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 16; j++) {
        tablero_de_juego[i][j] = 0;
        if(i >= 1 && i < 3 && j < mostrar_velocidad){
          tablero_de_juego[i][j] = 1;
        }
        if(i >= 5 && i < 7 && j < potenciometro_vidas){
          tablero_de_juego[i][j] = 1;
        }      
    }
  }
  Serial.println(mostrar_velocidad);
  Serial.println(delaytime);
  draw();
}

// ------------- CONTADOR -----------------

int digitos[10][8][8] = {
  { {0,0,1,1,1,0,0,0}, // Cero
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,0,1,1,1,0,0,0}
  }
  ,
  { {0,0,0,1,0,0,0,0}, // Uno
    {0,0,1,1,0,0,0,0},
    {0,0,0,1,0,0,0,0},
    {0,0,0,1,0,0,0,0},
    {0,0,0,1,0,0,0,0},
    {0,0,0,1,0,0,0,0},
    {0,0,0,1,0,0,0,0},
    {0,0,1,1,1,0,0,0}
  },
  { {0,0,0,0,0,0,0,0}, // dos
    {0,0,1,1,1,0,0,0},
    {0,1,0,0,0,1,0,0},
    {0,0,0,0,1,0,0,0},
    {0,0,0,1,0,0,0,0},
    {0,0,1,0,0,0,0,0},
    {0,1,1,1,1,1,0,0},
    {0,0,0,0,0,0,0,0}
  },
  { {0,0,0,0,0,0,0,0}, // 3
    {0,0,1,1,1,0,0,0},
    {0,1,0,0,0,1,0,0},
    {0,0,0,0,0,1,0,0},
    {0,0,0,1,1,0,0,0},
    {0,0,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,0,1,1,1,0,0,0}
  },
  { {0,0,0,0,0,0,0,0}, // 4
    {0,1,0,0,0,1,0,0},
    {0,1,0,0,0,1,0,0},
    {0,1,1,1,1,1,1,0},
    {0,0,0,0,0,1,0,0},
    {0,0,0,0,0,1,0,0},
    {0,0,0,0,0,1,0,0},
    {0,0,0,0,0,0,0,0}
  },
  { {0,0,0,0,0,0,0,0}, // 5
    {0,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,0,0},
    {0,1,1,1,1,0,0,0},
    {0,0,0,0,0,1,0,0},
    {0,0,0,0,0,0,1,0},
    {0,0,0,0,0,1,0,0},
    {0,1,1,1,1,0,0,0}
  },
  { {0,0,0,0,0,0,0,0}, // 6
    {0,0,1,1,1,0,0,0},
    {0,1,0,0,0,0,0,0},
    {0,1,0,1,1,1,0,0},
    {0,1,1,0,0,0,1,0},
    {0,1,0,0,0,0,1,0},
    {0,0,1,1,1,1,0,0},
    {0,0,0,0,0,0,0,0}
  }, { {0,0,0,0,0,0,0,0}, // 7
    {0,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,1,0},
    {0,0,0,0,0,1,0,0},
    {0,0,0,0,1,0,0,0},
    {0,0,0,1,0,0,0,0},
    {0,0,1,0,0,0,0,0},
    {0,0,0,0,0,0,0,0}
  }, { {0,0,0,0,0,0,0,0}, // 8
    {0,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,1,0},
    {0,1,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,1,0},
    {0,1,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,0}
  },
  { {0,0,0,0,0,0,0,0}, // 9
    {0,1,1,1,1,1,1,0},
    {0,1,0,0,0,0,1,0},
    {0,1,1,1,1,1,1,0},
    {0,0,0,0,0,0,1,0},
    {0,0,0,0,0,0,1,0},
    {0,0,0,0,0,0,1,0},
    {0,0,0,0,0,0,0,0}
  }
  
};

void mostrarNumero(int num, int seconds, bool mostrar_tick){
  int unidades = num % 10;
  int decenas = (num - unidades) / 10;

  // Settear Buffer 
  for(int i = 0; i < 16; i++){
    for(int j = 0; j < 8; j++){
      if(i < 8){
        buffer[j][i] = digitos[decenas][j][i];
      }
      else {
        buffer[j][i] = digitos[unidades][j][i];
      }      
    }
  }
  // Imprimir Buffer
  // matriz con driver
   int matriz_izquierda[ROW_NUM][COL_NUM/2] = {};
   // matriz sin driver
   int matriz_derecha[ROW_NUM][COL_NUM/2] = {};
  
  // este loop hace que se pueda escribir dentro de la matriz de led
  for (int i = 0; i < ROW_NUM; i++) {
    for (int j = 0; j < COL_NUM; j++) {
      //si la columna es menor que 8, esta pintara en la matriz con driver (izquierda)
      //de lo contrario pintara en la matriz sin driver (derecha)
      if (j < COL_NUM/2) {
        matriz_izquierda[i][j] = buffer[i][j];
      } else {
        matriz_derecha[i][j-8] = buffer[i][j];
      }
    }
  }
  // actualizacion de las matrices
  if (mostrar_tick){ // cuando sólo se requiere renderizar por loop, en lugar de n segundos
      actualizarMatrices(matriz_izquierda, matriz_derecha);
 }else{
    for (int i = 0; i < seconds*40; i++) {
      actualizarMatrices(matriz_izquierda, matriz_derecha);
    }
  }
}
