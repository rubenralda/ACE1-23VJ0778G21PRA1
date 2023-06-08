int punteos[] = {3,4,8,11,15}; // Stack de Punteos
int stats[] = {0,0,0,0,0}; // Stack de Stats (0-8)
byte bufferStats[8][16];


void limpiarBufferStats(){
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 16; col++) {
      bufferStats[row][col] = 0;
    }
  }
}

void calcularStat(int indiceStat, int sumatoria){
  if (sumatoria == 0){
    stats[indiceStat] = 0;
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

// BORRAR

void printBinaryMatrix() {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 16; col++) {
      Serial.print(bufferStats[row][col]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

// ---


void setup() {
  Serial.begin(9600);
  generarStats();
  limpiarBufferStats();

  // BORRAR
  for (int i = 0; i < 5; i++) {
  Serial.println(stats[i]);
  }
  delay(500);
  actualizarBufferStats();
  printBinaryMatrix();
  //---
}

void loop() {
  // put your main code here, to run repeatedly:
}
