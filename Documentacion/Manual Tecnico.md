Version proteus 1.12
arduino IDE 2.1.0
led control 1.0.6

## Estadísticas

### Manejo de Memoria

Para almacenar las estadísticas se creó un stack de scores, el cuál tiene una capacidad de 5 puntajes. Además se vinculó a dicho stack, otro stack de estadísticas, que representa el valor de la gráfica de cada punteo y finalmente un buffer de estadísticas que despliega en ambas matrices las respectivas estadísticas.

~~~
int punteos[] = {0,0,0,0,0}; // Stack de Punteos
int stats[] = {0,0,0,0,0}; // Stack de Stats (0-8)
int bufferStats[8][16];
~~~

### Agregar Punteo

Al agregar un punteo genera una reacción en cadena que actualiza automáticamente todos los datos y también el buffer de estadísticas para poder desplegarlas inmediata o posteriormente, según desee el usuario. 

~~~
void agregarPunteo(int nuevoPunteo){ // Añade al Stack de Punteos
  for (int i = 4; i <= 1; i--){ // Por cada punteo
    punteos[i] = punteos[i-1];  // Se corre una posición
  }
  punteos[0] = nuevoPunteo;     // Se agrega nuevo punteo
  generarStats();               // Genera las estadísticas del 0-8
}
~~~

### Cálculo de Gráfica

Anteriormente se mencionó el cálculo que se realiza, pero a continuación se detalla el método que realiza dicho cálculo. 

~~~
void generarStats(){
  int sumatoria = punteos[0] + punteos[1] + punteos[2] + punteos[3] + punteos[4];
  for (int i = 0; i <= 4; i++){ // Por cada punteo
    calcularStat(i, sumatoria); // Cálculo de estadística 0-8
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
~~~

### Gráfica de Barras

Tras obtener los valores de 1-8 que representan la altura de cada barra. Se puede ahora trazar dichas gráficas dentro del buffer de estadísticas y posteriormente imprimirlo en las matrices. 

~~~
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
~~~

## Juego

### Variables de juego

``` cpp
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
```

### Loop de Juego

Cada vez que se ejecuta el loop principal del programa, y se encuentra dentro del juego, este método hace que:
 * El avión se mueva una unidad, hacia la dirección en la que se dirige 
 * Calcula el tiempo transcurrido desde la última vez que el avión bajo, si el tiempo es mayor a 1000ms hace que el avión baje, y actualiza el reloj.
 * Ejecuta el loop de bomba, que se explica más adelante.

``` cpp
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
    draw();
    bombloop();
    draw();
}
```

### Loop de bomba

Es método de encarga de verificar el estado de una bomba en el juego. Cada vez que se ejecuta, si hay una bomba en el juego, se asegura de que:

* La bomba caiga una unidad hacia abajo
* Si cayó en la posición de una torre, esta reduzca su altura, hasta esta llegar a 0
* Cuanda dicha torre llegue a 0, aumente el puntaje, y agregar una vida extra cuando el puntaje es multiplo de 5
* Llamar a check_score, para verificar si hay que pasar al siguiente nivel

``` cpp
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
```
 
### Movimiento del avión

#### remove_plane

Se encarga de eliminar el avión del tablero de juego, esto se hace para que cuando el avión se mueva no queden espacios encendidos en la matriz de led donde no los tendrían que haber.

``` cpp
void remove_plane(){
    tablero_de_juego[current_plane.bottom][current_plane.front] = 0;
    tablero_de_juego[current_plane.bottom][current_plane.middle] = 0;
    tablero_de_juego[current_plane.bottom][current_plane.rear] = 0;
    tablero_de_juego[current_plane.bottom-1][(current_plane.direction == MOV_LEFT)? current_plane.rear : current_plane.front] = 0 ;
}
```

#### move_plane 

Se encarga de movel el avión una unidad hacia ya sea la derecha o la izquierda, llama a check_collision para verificar si choca con una torre. Se calcula la posisión con la función x_pos().

``` cpp
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
```
#### move_plane_down

Mueve el avión en una unidad hacia abajo, llama a check_collision para verificar si choca con una torre al bajar

``` cpp
void move_plane_down(){
    remove_plane();
    current_plane.bottom = (current_plane.bottom + 1 < 7 ) ? current_plane.bottom + 1 : 7;
    tablero_de_juego[current_plane.bottom][current_plane.front] = 1;
    tablero_de_juego[current_plane.bottom][current_plane.middle] = 1;
    tablero_de_juego[current_plane.bottom][current_plane.rear] = 1;
    tablero_de_juego[current_plane.bottom-1][(current_plane.direction == MOV_LEFT)? current_plane.rear : current_plane.front] = 1 ;
    check_collision();
}
```

#### reset_plane

Esta función es utilizada por check_collision para regresar el avión 2 unidades hacia arriba en caso de colisón.

``` cpp
void reset_plane(){
    remove_plane();
    current_plane.bottom -=2 ,
    tablero_de_juego[current_plane.bottom][current_plane.front] = 1;
    tablero_de_juego[current_plane.bottom][current_plane.middle] = 1;
    tablero_de_juego[current_plane.bottom][current_plane.rear] = 1;
    tablero_de_juego[current_plane.bottom-1][(current_plane.direction == MOV_LEFT)? current_plane.rear : current_plane.front] = 1 ;
}
```

