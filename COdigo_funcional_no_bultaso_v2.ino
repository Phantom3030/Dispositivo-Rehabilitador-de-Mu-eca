#include <Arduino.h>
#include <Keypad.h>    // libreria para teclado
#include <ESP32Servo.h>     // libreria para servomotor
#include <Wire.h>      // libreria para comunicacion I2C
#include <LiquidCrystal_I2C.h>  // libreria para LCD con adaptador I2C

LiquidCrystal_I2C lcd(0x27, 16, 2);

// declaracion para identificacion los servomotores
Servo myservoFE;      // identificacion del servo para movimineto flexión extensión
Servo myservoAA;      // identificacion del servo para movimineto abduccion aduccion
int posFE = 90;       // variable para el valor de posicion del servo de flexion extensión
int posAA = 90;       // variable para el valor de posicion del servo de abduccion aducción
// el valor de 90 grados se toma como posicion neutral

const uint8_t ROWS = 4;
const uint8_t COLS = 4;

char hexakeys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

uint8_t colPins[COLS] = {13, 12, 14, 27};
uint8_t rowPins[ROWS] = {26, 25, 33, 32};

// identifica el teclado para realizar el mapeo y asociacion de tecla con caracter
Keypad customkeypad = Keypad(makeKeymap(hexakeys), rowPins, colPins, ROWS, COLS);

char tecla; // variable que guardara lo digitado en el teclado

// declaracion de variables que se utilizaran
char valor[3];
int cont=0;
int dura=100; // para la velocidad
int Tipo=0;  //tipo de rehabilitación
int FL=0;    //ángulo flexión
int EX=0;    //ángulo extensión
int AB=0;    //ángulo abducción
int AD=0;    //ángulo aducción
int rep=0;   //repeticiones
int Vel=0;   //tipo de velocidad

int aux=0;   //para conteo de repeticiones

// Declaración de funciones
void selecTipo();
void selecFL();
void selecEX();
void selecAB();
void selecAD();
void selecRep();
void selecVel();
void confirm();
void rutinaUno();
void rutinaDos();
void rutinaTres();
void flashFE();
void flashAA();

void setup(){
  Serial.begin(9600);

  // se establece pin de señal para los servos
  myservoFE.attach(18);  // pin para el servo de flexión extensión
  myservoAA.attach(4);  // pin para el servo de abducción aducción

  // presentación para mostrar al inicio del programa en el LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Rehabilitador");
  lcd.setCursor(0, 1);
  lcd.print("de muñeca");
  delay(1000);
  lcd.clear();
}

void loop() {
  // coloca los servos en posición neutra 
  myservoFE.write(135);
  delay(20);
  myservoAA.write(90);
  delay(20);

  //selección de tipo
  lcd.setCursor(0, 0);
  lcd.print("Tipo de rutina: ");
  lcd.setCursor(0, 1);  // Mover el cursor primera columna (0), segunda fila (1)
  delay(500);
  lcd.print("1 2 3");
  delay(500);

  selecTipo();

  delay(400);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Rutina elegida:");
  delay(100);
  lcd.setCursor(7, 1);  // Mover el cursor primera columna (0), segunda fila (1)
  lcd.print(Tipo);
  delay(800);
  lcd.clear();

  delay(200);

  // dar valores a ángulos
  //ángulo flexión
  lcd.setCursor(0, 0);
  lcd.print("Valor Angulo FL:");

  selecFL();

  delay(400);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Valor FL:");
  delay(100);
  lcd.setCursor(6, 1);  // Mover el cursor primera columna (0), segunda fila (1)
  lcd.print(FL);
  delay(800);
  lcd.clear();

  delay(200);

  //ángulo Extención
  lcd.setCursor(0, 0);
  lcd.print("Valor Angulo EX:");

  selecEX();

  delay(400);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Valor EX:");
  delay(100);
  lcd.setCursor(6, 1);  // Mover el cursor primera columna (0), segunda fila (1)
  lcd.print(EX);
  delay(800);
  lcd.clear();

  delay(200);

  //ángulo abducción
  lcd.setCursor(0, 0);
  lcd.print("Valor Angulo AB:");

  selecAB();

  delay(400);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Valor AB:");
  delay(100);
  lcd.setCursor(6, 1);  // Mover el cursor primera columna (0), segunda fila (1)
  lcd.print(AB);
  delay(800);
  lcd.clear();

  delay(200);

  //ángulo aducción
  lcd.setCursor(0, 0);
  lcd.print("Valor Angulo AD:");

  selecAD();

  delay(400);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Valor AD:");
  delay(100);
  lcd.setCursor(6, 1);  // Mover el cursor primera columna (0), segunda fila (1)
  lcd.print(AD);
  delay(800);
  lcd.clear();

  delay(200);
  //repeticiones
  lcd.setCursor ( 0, 0 );
  lcd.print("N Repeticiones:");

  selecRep();

  delay(400);
  lcd.clear();
  lcd.setCursor ( 0, 0 );
  lcd.print("Repeticiones:"); 
  delay(100);
  lcd.setCursor ( 0, 1 ); // Mover el cursor primera columna (0), segunda fila (1)
  lcd.print(rep);
  delay(800);
  lcd.clear();

  delay(200);

  //velocidad
  lcd.setCursor ( 0, 0 );
  lcd.print("Velocidad");

  selecVel();

  delay(400);

  lcd.clear();
  lcd.setCursor ( 0, 0 );
  lcd.print("velocidad:"); 
  delay(100);
  if (Vel==1) {
    dura=100;
    lcd.setCursor ( 4, 1 );
    lcd.print("lenta");
  }
  if (Vel==2) {
    dura=50;
    lcd.setCursor ( 4, 1 );
    lcd.print("Media");
  }
  if (Vel==3) {
    dura=15;
    lcd.setCursor ( 4, 1 );
    lcd.print("Rapida");
  }

  delay(800);
  lcd.clear();

  delay(200);

  //confirmacion para iniciar rutina
  lcd.setCursor ( 0, 0 );
  lcd.print("Iniciar Rutina");
  lcd.setCursor ( 0, 1 ); // Mover el cursor primera columna (0), segunda fila (1)
  delay(100);
  lcd.print("Pulse A");

  confirm();

  lcd.clear();

  lcd.setCursor ( 0, 0 );
  lcd.print("Inicio en:");
  lcd.setCursor ( 12, 1 );
  lcd.print("SEG");

  delay(500);

  lcd.setCursor ( 11, 1 );
  lcd.print("3");
  delay(800);
  lcd.setCursor ( 11, 1 );
  lcd.print("2");
  delay(800);
  lcd.setCursor ( 11, 1 );
  lcd.print("1");
  delay(800);
  lcd.clear();
  lcd.setCursor ( 0, 0 );
  lcd.print("Ejecutando rutina");
  lcd.setCursor ( 0, 1 );
  lcd.print("Esperar...");

  delay(200);
  lcd.clear();
  lcd.setCursor ( 0, 0 );
  lcd.print("Iniciando");
  delay(800);
  lcd.setCursor ( 9, 0 );
  lcd.print(".");
  delay(800);
  lcd.setCursor ( 10, 0 );
  lcd.print(".");
  delay(800);
  lcd.setCursor ( 11, 0 );
  lcd.print("-");
  delay(800);
  lcd.setCursor ( 12, 0 );
  lcd.print("-");
  delay(800);
  lcd.setCursor ( 13, 0 );
  lcd.print("!");
  delay(800);
  lcd.setCursor ( 14, 0 );
  lcd.print("!!");

  if (Tipo==1) rutinaUno();
  if (Tipo==2) rutinaDos();
  if (Tipo==3) rutinaTres();

  delay(200);
  lcd.clear();
  lcd.setCursor ( 0, 0 );
  lcd.print("FIN DE LA RUTINA");
  delay(1000);
  lcd.setCursor ( 0, 1 );
  lcd.print("RETIRAR LA MANO");
  delay(2000);
  lcd.clear();
  delay(1000);
}

void selecTipo() {
  tecla = customkeypad.getKey();
  char valorSE[2];
  delay(30);
  while (tecla != '1' && tecla != '2' && tecla != '3') {
    tecla = customkeypad.getKey();
    delay(10);
    Serial.print(tecla);
  }

  valorSE[0] = tecla;
  Tipo = atol(valorSE);
}

void selecFL() {
  char valorFL[3] = {0}; // Inicializa el array y lo llena con ceros
  int contFL = 0; // Contador para el índice del array valorFL

  while (true) { 
    char tecla = customkeypad.getKey(); // Lee la tecla presionada
    if (tecla == 'A') break; // Sale del bucle si se presiona 'A'
    
    if (tecla != NO_KEY && tecla != 'D' && tecla != 'B' && tecla != 'C' && tecla != '*' && tecla != '#') {
      valorFL[contFL] = tecla; // Almacena la tecla presionada en el array valorFL
      valorFL[contFL + 1] = '\0'; // Asegura que el array esté correctamente terminado con un nulo

      FL = atol(valorFL); // Convierte el valor almacenado en valorFL a un número entero largo

      lcd.setCursor(0, 1); // Establece el cursor de la LCD en la posición (0, 1)
      lcd.print(FL); // Muestra el valor de FL en la LCD
      delay(20); // Pausa de 20 milisegundos

      contFL = contFL + 1; // Incrementa el contador
      if (contFL == 2) { // Si el contador llega a 2 (2 dígitos ingresados)
        contFL = 0; // Resetea el contador
        valorFL[0] = '\0'; // Resetea el array
        delay(20); // Pausa de 20 milisegundos
      }
    }
  }
}

void selecEX() {
  char valorEX[3] = {0}; // Inicializa el array y lo llena con ceros
  int contEX = 0; // Contador para el índice del array valorFL

  while (true) { 
    char tecla = customkeypad.getKey(); // Lee la tecla presionada
    if (tecla == 'A') break; // Sale del bucle si se presiona 'A'
    
    if (tecla != NO_KEY && tecla != 'D' && tecla != 'B' && tecla != 'C' && tecla != '*' && tecla != '#') {
      valorEX[contEX] = tecla; // Almacena la tecla presionada en el array valorFL
      valorEX[contEX + 1] = '\0'; // Asegura que el array esté correctamente terminado con un nulo

      EX = atol(valorEX); // Convierte el valor almacenado en valorFL a un número entero largo

      lcd.setCursor(0, 1); // Establece el cursor de la LCD en la posición (0, 1)
      lcd.print(EX); // Muestra el valor de FL en la LCD
      delay(20); // Pausa de 20 milisegundos

      contEX = contEX + 1; // Incrementa el contador
      if (contEX == 2) { // Si el contador llega a 2 (2 dígitos ingresados)
        contEX = 0; // Resetea el contador
        valorEX[0] = '\0'; // Resetea el array
        delay(20); // Pausa de 20 milisegundos
      }
    }
  }
}

void selecAB() {
  char valorAB[3] = {0}; // Inicializa el array y lo llena con ceros
  int contAB = 0; // Contador para el índice del array valorFL

  while (true) { 
    char tecla = customkeypad.getKey(); // Lee la tecla presionada
    if (tecla == 'A') break; // Sale del bucle si se presiona 'A'
    
    if (tecla != NO_KEY && tecla != 'D' && tecla != 'B' && tecla != 'C' && tecla != '*' && tecla != '#') {
      valorAB[contAB] = tecla; // Almacena la tecla presionada en el array valorFL
      valorAB[contAB + 1] = '\0'; // Asegura que el array esté correctamente terminado con un nulo

      AB = atol(valorAB); // Convierte el valor almacenado en valorFL a un número entero largo

      lcd.setCursor(0, 1); // Establece el cursor de la LCD en la posición (0, 1)
      lcd.print(AB); // Muestra el valor de FL en la LCD
      delay(20); // Pausa de 20 milisegundos

      contAB = contAB + 1; // Incrementa el contador
      if (contAB == 2) { // Si el contador llega a 2 (2 dígitos ingresados)
        contAB = 0; // Resetea el contador
        valorAB[0] = '\0'; // Resetea el array
        delay(20); // Pausa de 20 milisegundos
      }
    }
  }
}

void selecAD() {
  char valorAD[3] = {0}; // Inicializa el array y lo llena con ceros
  int contAD = 0; // Contador para el índice del array valorFL

  while (true) { 
    char tecla = customkeypad.getKey(); // Lee la tecla presionada
    if (tecla == 'A') break; // Sale del bucle si se presiona 'A'
    
    if (tecla != NO_KEY && tecla != 'D' && tecla != 'B' && tecla != 'C' && tecla != '*' && tecla != '#') {
      valorAD[contAD] = tecla; // Almacena la tecla presionada en el array valorFL
      valorAD[contAD + 1] = '\0'; // Asegura que el array esté correctamente terminado con un nulo

      AD = atol(valorAD); // Convierte el valor almacenado en valorFL a un número entero largo

      lcd.setCursor(0, 1); // EstADlece el cursor de la LCD en la posición (0, 1)
      lcd.print(AD); // Muestra el valor de FL en la LCD
      delay(20); // Pausa de 20 milisegundos

      contAD = contAD + 1; // Incrementa el contador
      if (contAD == 2) { // Si el contador llega a 2 (2 dígitos ingresados)
        contAD = 0; // Resetea el contador
        valorAD[0] = '\0'; // Resetea el array
        delay(20); // Pausa de 20 milisegundos
      }
    }
  }
}

void selecRep() {
  char valorRep[3] = {0}; // Inicializa el array y lo llena con ceros
  int contRep = 0; // Contador para el índice del array valorFL

  while (true) { 
    char tecla = customkeypad.getKey(); // Lee la tecla presionada
    if (tecla == 'A') break; // Sale del bucle si se presiona 'A'
    
    if (tecla != NO_KEY && tecla != 'D' && tecla != 'B' && tecla != 'C' && tecla != '*' && tecla != '#') {
      valorRep[contRep] = tecla; // Almacena la tecla presionada en el array valorFL
      valorRep[contRep + 1] = '\0'; // Asegura que el array esté correctamente terminado con un nulo

      rep = atol(valorRep); // Convierte el valor almacenado en valorFL a un número entero largo

      lcd.setCursor(0, 1); // EstReplece el cursor de la LCD en la posición (0, 1)
      lcd.print(rep); // Muestra el valor de FL en la LCD
      delay(20); // Pausa de 20 milisegundos

      contRep = contRep + 1; // Incrementa el contador
      if (contRep == 2) { // Si el contador llega a 2 (2 dígitos ingresados)
        contRep = 0; // Resetea el contador
        valorRep[0] = '\0'; // Resetea el array
        delay(20); // Pausa de 20 milisegundos
      }
    }
  }
}

void selecVel() {
  Serial.print("asd ");
  char valorVel[2] = {0}; // Inicializa el array y lo llena con ceros
  tecla = customkeypad.getKey(); // Lee la tecla presionada inicialmente
  delay(30); // Pausa de 30 ms
  Serial.print(tecla); // Imprime la tecla leída
  delay(30); // Pausa de 30 ms
  Serial.print(" eeee"); // Imprime "eeee"
  delay(30); // Pausa de 30 ms

  // Bucle hasta que se presione '1', '2' o '3'
  while (tecla != '1' && tecla != '2' && tecla != '3') {
    tecla = customkeypad.getKey(); // Lee la próxima tecla presionada
    delay(10); // Pausa de 10 ms
    Serial.print(tecla); // Imprime la tecla leída
  }

  valorVel[0] = tecla; // Almacena la tecla válida en valorVel
  valorVel[1] = '\0'; // Asegura que el array esté terminado con un carácter nulo
  Vel = atol(valorVel); // Convierte el valor almacenado en valorVel a un número entero largo
}

void rutinaUno() {
  aux = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Repeticiones:");
  delay(200);
  lcd.setCursor(0, 1);
  lcd.print(aux);
  lcd.setCursor(7, 1);
  lcd.print("de");
  lcd.setCursor(13, 1);
  lcd.print(rep);
  int contRU = 0;
  while (contRU < rep) {
    aux = contRU + 1;
    lcd.setCursor(0, 1);
    lcd.print(aux);
    flashFE();
    delay(100);
    contRU++;
  }
}

void rutinaDos() {
  aux = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Repeticiones:");
  delay(200);
  lcd.setCursor(0, 1);
  lcd.print(aux);
  lcd.setCursor(7, 1);
  lcd.print("de");
  lcd.setCursor(13, 1);
  lcd.print(rep);
  int contRD = 0;

  while (contRD < rep) {
    aux = contRD + 1;
    lcd.setCursor(0, 1);
    lcd.print(aux);
    flashAA();
    delay(100);
    contRD++;
  }
}

void rutinaTres() {
  aux = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Repeticiones:");
  delay(200);
  lcd.setCursor(0, 1);
  lcd.print(aux);
  lcd.setCursor(7, 1);
  lcd.print("de");
  lcd.setCursor(13, 1);
  lcd.print(rep);
  int contRT = 0;

  while (contRT < rep) {
    aux = contRT + 1;
    lcd.setCursor(0, 1);
    lcd.print(aux);
    flashFE();
    delay(200);
    flashAA();
    delay(200);
    contRT++;
  }
}

void confirm() {
  tecla = customkeypad.getKey();
  while (tecla != 'A') {
    tecla = customkeypad.getKey();
    delay(5);
  }
}

void flashAA() {
  for (posAA = 90; posAA >= 90-AB; posAA -= 1) { // in steps of 1 degree
  myservoAA.write(posAA);          // tell servo to go to position in variable 'pos'
  delay(dura);                    // waits 15ms for the servo to reach the position
  }

  for (posAA = 90-AB ; posAA <= 90+AD; posAA += 1) { // goes from 180 degrees to 0 degrees
  myservoAA.write(posAA);          // tell servo to go to position in variable 'pos'
  delay(dura);                    // waits 15ms for the servo to reach the position
  }

  for (posAA = 90+AD; posAA >= 90; posAA -= 1) { // in steps of 1 degree
  myservoAA.write(posAA);          // tell servo to go to position in variable 'pos'
  delay(dura);                    // waits 15ms for the servo to reach the position
  }
}

void flashFE() {
  for (posFE = 135; posFE >= 135-EX; posFE -= 1) { // in steps of 1 degree
  myservoFE.write(posFE);          // tell servo to go to position in variable 'pos'
  delay(dura);                    // waits 15ms for the servo to reach the position
  }

  for (posFE = 135- EX; posFE <= 135 + FL ; posFE += 1) { // goes from 180 degrees to 0 degrees
  myservoFE.write(posFE);          // tell servo to go to position in variable 'pos'
  delay(dura);                    // waits 15ms for the servo to reach the position
  }

  for (posFE = 135+FL; posFE >= 135; posFE -= 1) { //in steps of 1 degree
  myservoFE.write(posFE);          // tell servo to go to position in variable 'pos'
  delay(dura);                    // waits 15ms for the servo to reach the position
  }
}