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
 