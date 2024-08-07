#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"

#define I2C_MASTER_SCL_IO           GPIO_NUM_9  // GPIO number for I2C master clock (SCL)
#define I2C_MASTER_SDA_IO           GPIO_NUM_8  // GPIO number for I2C master data (SDA)
#define I2C_MASTER_NUM              I2C_NUM_0  // I2C port number for master dev
#define I2C_MASTER_FREQ_HZ          400000     // I2C master clock frequency
#define I2C_MASTER_TX_BUF_DISABLE   0          // I2C master doesn"t need buffer
#define I2C_MASTER_RX_BUF_DISABLE   0          // I2C master doesn"t need buffer
#define I2C_NUM                     I2C_NUM_0

#define SLAVE_ADDRESS_LCD 0x27 // change this according to ur setup
esp_err_t err;
static const char *TAG = "LCD";

#define SERVO_FE_PIN 18
#define SERVO_AA_PIN 4

#define LCD_CMD 0
#define LCD_DATA 1

//static const char *TAG = "Rehab";

// Keypad pins and keymap
#define ROW_NUM 4
#define COL_NUM 4

// Keypad GPIO pins
int row_pins[ROW_NUM] = {GPIO_NUM_4, GPIO_NUM_5, GPIO_NUM_6, GPIO_NUM_7};
int col_pins[COL_NUM] = {GPIO_NUM_1, GPIO_NUM_0, GPIO_NUM_3, GPIO_NUM_2};

// Keymap
static const char keymap[ROW_NUM][COL_NUM] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

char tecla; // Variable que guardara lo digitado en el teclado

// Variables
int dura = 500;
int Tipo = 0;
int FL = 0;
int EX = 0;
int AB = 0;
int AD = 0;
int rep = 0;
int Vel = 0;
char str[3];
int aux=0;

// Funciones
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

//Funciones para la LCD
void lcd_send_cmd(char cmd);
void lcd_send_data(char data);
void lcd_clear(void);
void lcd_gotoxy(int row, int col);
void lcd_putc(char *str);
void lcd_init(void);

void keypad_init(void);

/**
 * @brief i2c master initialization
 */
static esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}


void app_main() {
  ESP_ERROR_CHECK(i2c_master_init());
  ESP_LOGI(TAG, "I2C initialized successfully");
  
  // Inicializar los servos
  ledc_timer_config_t ledc_timer = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .timer_num = LEDC_TIMER_0,
    .duty_resolution = LEDC_TIMER_13_BIT,
    .freq_hz = 50,  // 50 Hz for servos
  };
  ledc_timer_config(&ledc_timer);

  ledc_channel_config_t ledc_channel_FE = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_0,
    .timer_sel = LEDC_TIMER_0,
    .intr_type = LEDC_INTR_DISABLE,
    .gpio_num = SERVO_FE_PIN,
    .duty = 0,  
    .hpoint = 0,
  };
  ledc_channel_config(&ledc_channel_FE);

  ledc_channel_config_t ledc_channel_AA = {
    .speed_mode = LEDC_LOW_SPEED_MODE,
    .channel = LEDC_CHANNEL_1,
    .timer_sel = LEDC_TIMER_0,
    .intr_type = LEDC_INTR_DISABLE,
    .gpio_num = SERVO_AA_PIN,
    .duty = 0, 
    .hpoint = 0,
  };
  ledc_channel_config(&ledc_channel_AA);

  // Inicializar el LCD
  lcd_init();
  lcd_clear();
  lcd_gotoxy(0, 0);
  lcd_putc("     Rehabilitador");
  lcd_gotoxy(0, 1);
  lcd_putc("de muneca");
  keypad_init ();
  vTaskDelay(10000 / portTICK_PERIOD_MS);

  while (1) {
    //selección de tipo
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_putc("     Tipo de rutina: ");
    lcd_gotoxy(0, 1);  // Mover el cursor primera columna (0), segunda fila (1)
    lcd_putc("1 2 3");
    vTaskDelay(500 / portTICK_PERIOD_MS);

    selecTipo();

    vTaskDelay(500 / portTICK_PERIOD_MS);
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_putc("     Rutina elegida:");
    vTaskDelay(500 / portTICK_PERIOD_MS);
    lcd_gotoxy(7, 1);
    sprintf(str, "%d", Tipo);
    lcd_putc(str);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    lcd_clear();

    vTaskDelay(500 / portTICK_PERIOD_MS);

    // dar valores a ángulos
    //ángulo flexión
    lcd_gotoxy(0, 0);
    lcd_putc("Valor Angulo FL:");

    selecFL();

    usleep(400);
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_putc("     Valor FL:");
    vTaskDelay(200 / portTICK_PERIOD_MS);
    lcd_gotoxy(6, 1);  // Mover el cursor primera columna (0), segunda fila (1)
    sprintf(str, "%d", FL);
    lcd_putc(str);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    lcd_clear();

    usleep(200);

    //ángulo Extención
    lcd_gotoxy(0, 0);
    lcd_putc("    Valor Angulo EX:");

    selecEX();

    usleep(400);
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_putc("     Valor EX:");
    vTaskDelay(200 / portTICK_PERIOD_MS);
    lcd_gotoxy(6, 1);  // Mover el cursor primera columna (0), segunda fila (1)
    sprintf(str, "%d", EX);
    lcd_putc(str);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    lcd_clear();

    usleep(200);

    //ángulo abducción
    lcd_gotoxy(0, 0);
    lcd_putc("    Valor Angulo AB:");

    selecAB();

    usleep((400));
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_putc("     Valor AB:");
    vTaskDelay(200 / portTICK_PERIOD_MS);
    lcd_gotoxy(6, 1);  // Mover el cursor primera columna (0), segunda fila (1)
    sprintf(str, "%d", AB);
    lcd_putc(str);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    lcd_clear();

    usleep((200));

    //ángulo aducción
    lcd_gotoxy(0, 0);
    lcd_putc("    Valor Angulo AD:");

    selecAD();

    usleep((400));
    lcd_clear();
    lcd_gotoxy(0, 0);
    lcd_putc("     Valor AD:");
    vTaskDelay(200 / portTICK_PERIOD_MS);
    lcd_gotoxy(6, 1);  // Mover el cursor primera columna (0), segunda fila (1)
    sprintf(str, "%d", AD);
    lcd_putc(str);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    lcd_clear();

    usleep((200));
    //repeticiones
    lcd_gotoxy (0, 0);
    lcd_putc("    N Repeticiones:");

    selecRep();

    usleep((400));
    lcd_clear();
    lcd_gotoxy (0, 0);
    lcd_putc("     Repeticiones:"); 
    vTaskDelay(200 / portTICK_PERIOD_MS);
    lcd_gotoxy (0, 1); // Mover el cursor primera columna (0), segunda fila (1)
    sprintf(str, "%d", rep);
    lcd_putc(str);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    lcd_clear();

    usleep((200));

    //velocidad
    lcd_gotoxy (0, 0);
    lcd_putc("    Velocidad");

    selecVel();

    usleep((400));

    lcd_clear();
    lcd_gotoxy (0, 0);
    lcd_putc("     Velocidad:"); 
    vTaskDelay(200 / portTICK_PERIOD_MS);
    if (Vel==1) {
      dura=100;
      lcd_gotoxy (4, 1);
      lcd_putc("Lenta");
    }
    if (Vel==2) {
      dura=50;
      lcd_gotoxy (4, 1);
      lcd_putc("Media");
    }
    if (Vel==3) {
      dura=15;
      lcd_gotoxy (4, 1);
      lcd_putc("Rapida");
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    lcd_clear();

    vTaskDelay(200 / portTICK_PERIOD_MS);

    //confirmacion para iniciar rutina
    lcd_gotoxy (0, 0);
    lcd_putc("Iniciar Rutina");
    lcd_gotoxy (0, 1); // Mover el cursor primera columna (0), segunda fila (1)
    vTaskDelay(200 / portTICK_PERIOD_MS);
    lcd_putc("Pulse #");

    confirm();

    lcd_clear();
    lcd_gotoxy (0, 0);
    lcd_putc("     Inicio en:");
    lcd_gotoxy (12, 1);
    lcd_putc("SEG");

    vTaskDelay(400 / portTICK_PERIOD_MS);

    lcd_gotoxy (11, 1);
    lcd_putc("3");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    lcd_gotoxy (11, 1);
    lcd_putc("2");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    lcd_gotoxy (11, 1);
    lcd_putc("1");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    lcd_clear();
    lcd_gotoxy (0, 0);
    lcd_putc("     Ejecutando rutina");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    lcd_gotoxy (0, 1);
    lcd_putc("Esperar...");

    vTaskDelay(500 / portTICK_PERIOD_MS);
    lcd_clear();
    lcd_gotoxy (0, 1);
    lcd_putc("    Iniciando");
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    lcd_gotoxy (9, 1);
    lcd_putc(".");
    vTaskDelay(500 / portTICK_PERIOD_MS);
    lcd_gotoxy (10, 1);
    lcd_putc(".");
    vTaskDelay(500 / portTICK_PERIOD_MS);
    lcd_gotoxy (11, 1);
    lcd_putc("-");
    vTaskDelay(500 / portTICK_PERIOD_MS);
    lcd_gotoxy (12, 1);
    lcd_putc("-");
    vTaskDelay(500 / portTICK_PERIOD_MS);
    lcd_gotoxy (13, 1);
    lcd_putc("!");
    vTaskDelay(500 / portTICK_PERIOD_MS);
    lcd_gotoxy (14, 1);
    lcd_putc("!!");

    if (Tipo==1) rutinaUno();
    if (Tipo==2) rutinaDos();
    if (Tipo==3) rutinaTres();

    vTaskDelay(2000 / portTICK_PERIOD_MS);
    lcd_clear();
    lcd_gotoxy (0, 0);
    lcd_putc("     FIN DE LA RUTINA");
    vTaskDelay(2000 / portTICK_PERIOD_MS);;
    lcd_gotoxy (0, 1);
    lcd_putc("RETIRAR LA MANO");
    vTaskDelay(4000 / portTICK_PERIOD_MS);
    lcd_clear();
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

// Funciones LCD
void lcd_send_cmd (char cmd)
{
  char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	err = i2c_master_write_to_device(I2C_NUM, SLAVE_ADDRESS_LCD, data_t, 4, 1000);
	if (err!=0) ESP_LOGI(TAG, "Error in sending command");
}

void lcd_send_data (char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=0
	data_t[1] = data_u|0x09;  //en=0, rs=0
	data_t[2] = data_l|0x0D;  //en=1, rs=0
	data_t[3] = data_l|0x09;  //en=0, rs=0
	err = i2c_master_write_to_device(I2C_NUM, SLAVE_ADDRESS_LCD, data_t, 4, 1000);
	if (err!=0) ESP_LOGI(TAG, "Error in sending data");
}

void lcd_clear (void)
{
	lcd_send_cmd (0x01);
	usleep(500);
}

void lcd_gotoxy(int row, int col)
{
    switch (col)
    {
        case 0:
            row |= 0x80;
            break;
        case 1:
            row |= 0xC0;
            break;
    }

    lcd_send_cmd (row);
}


void lcd_init (void)
{
	// 4 bit initialisation
	usleep(50000);  // wait for >40ms
	lcd_send_cmd (0x30);
	usleep(5000);  // wait for >4.1ms
	lcd_send_cmd (0x30);
	usleep(200);  // wait for >100us
	lcd_send_cmd (0x30);
	usleep(10000);
	lcd_send_cmd (0x20);  // 4bit mode
	usleep(10000);

  // dislay initialisation
	lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	usleep(1000);
	lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	usleep(1000);
	lcd_send_cmd (0x01);  // clear display
	usleep(1000);
	usleep(1000);
	lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	usleep(1000);
	lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
	usleep(1000);
}

void lcd_putc (char *str)
{
	while (*str) lcd_send_data (*str++);
}

// Keypad
void keypad_init() {
  // Las filas seran output
  for (int i = 0; i < ROW_NUM; i++) {
    gpio_set_direction(row_pins[i], GPIO_MODE_OUTPUT);
    gpio_set_level(row_pins[i], 1);  // Row pins = high
  }

  // Las columnas seran inputs con pull-up
  for (int i = 0; i < COL_NUM; i++) {
    gpio_set_direction(col_pins[i], GPIO_MODE_INPUT);
    gpio_set_pull_mode(col_pins[i], GPIO_PULLUP_ONLY);
  }
}

char read_keypad() {
  for (int row = 0; row < ROW_NUM; row++) {
    gpio_set_level(row_pins[row], 0);  

    for (int col = 0; col < COL_NUM; col++) {
      if (gpio_get_level(col_pins[col]) == 0) {  // Ver si la columna esta siendo presionada
        while (gpio_get_level(col_pins[col]) == 0);  // Esperar a que se deja de presionar la tecla
        gpio_set_level(row_pins[row], 1);  // Reiniciar la fila
        return keymap[row][col];  // Retornar el valor presionado
      }
    }
    gpio_set_level(row_pins[row], 1);  // Reiniciar la fila
  }
  return '\0';  // Retorna null si no se presiono ninguna tecla
}

// Funciones Principales

void selecTipo() {
  tecla = read_keypad();
  char valorSE[2];
  usleep(30);
  while (tecla != '1' && tecla != '2' && tecla != '3') {
    tecla = read_keypad();
    usleep(10);
    //sprintf(str, "%d", tecla);
  }
  valorSE[0] = tecla;
  Tipo = atol(valorSE);
}

void selecFL() {
  char valorFL[3] = {0}; // Iniciaizar el Array y llenarlo con 0
  int contFL = 0; // Contador para el Array

  while (1) {
    char tecla = read_keypad(); 
    if (tecla == '#') break; 
    
    if(tecla != '\0') {
      valorFL[contFL] = tecla; 
      valorFL[contFL + 1] = '\0'; // Asegurarse que el Array termino 
      FL = atol(valorFL); // Se convierte el valor
      sprintf(str, "%d", FL);
      lcd_gotoxy(0, 1); 
      lcd_putc(str); 
      vTaskDelay(100 / portTICK_PERIOD_MS);  
      contFL++;
      tecla = '\0';
    } 

    if (contFL == 2) { 
      contFL = 0; 
      valorFL[0] = '\0'; 
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
  }
}

void selecEX() {
  char valorEX[3] = {0}; // Iniciaizar el Array y llenarlo con 0
  int contEX = 0; // Contador para el Array

  while (1) {
    char tecla = read_keypad(); 
    if (tecla == '#') break; 

    if(tecla != '\0'){
      valorEX[contEX] = tecla; 
      valorEX[contEX + 1] = '\0'; // Asegurarse que el Array termino 
      EX = atol(valorEX); // Se convierte el valor
      sprintf(str, "%d", EX);
      lcd_gotoxy(0, 1); 
      lcd_putc(str); 
      vTaskDelay(100 / portTICK_PERIOD_MS);
      contEX++;
      tecla = '\0';
    } 

    if (contEX == 2) { 
      contEX = 0; 
      valorEX[0] = '\0'; 
      vTaskDelay(500 / portTICK_PERIOD_MS); 
    }
  }
}

void selecAB() {
  char valorAB[3] = {0}; // Iniciaizar el Array y llenarlo con 0
  int contAB = 0; 

  while (1) {
    char tecla = read_keypad(); 
    if (tecla == '#') break;

    if(tecla != '\0'){ 
    valorAB[contAB] = tecla; 
    valorAB[contAB + 1] = '\0'; // Asegurarse que el Array termino 
    AB = atol(valorAB); // Se convierte el valor
    sprintf(str, "%d", AB);
    lcd_gotoxy(0, 1); 
    lcd_putc(str); 
    vTaskDelay(100 / portTICK_PERIOD_MS); 
    contAB++; 
    tecla = '\0';
    }

    if (contAB == 2) { 
      contAB = 0; 
      valorAB[0] = '\0'; 
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }
  }
}

void selecAD() {
  char valorAD[3] = {0}; // Iniciaizar el Array y llenarlo con 0
  int contAD = 0; // Counter for the array indAD

  while (1) {
    char tecla = read_keypad(); 
    if (tecla == '#') break;

    if(tecla != '\0'){
      valorAD[contAD] = tecla; 
      valorAD[contAD + 1] = '\0'; // Asegurarse que el Array termino 
      AD = atol(valorAD); // Se convierte el valor
      sprintf(str, "%d", AD);
      lcd_gotoxy(0, 1); 
      lcd_putc(str); 
      vTaskDelay(100 / portTICK_PERIOD_MS);
      contAD++;
      tecla = '\0';
    } 

    if (contAD == 2) { 
      contAD = 0; 
      valorAD[0] = '\0'; 
      vTaskDelay(500 / portTICK_PERIOD_MS); 
    }
  }
}

void selecRep() {
  char valorRep[3] = {0}; // Iniciaizar el Array y llenarlo con 0
  int contRep = 0; 

  while (1) {
    char tecla = read_keypad(); 
    if (tecla == '#') break; 
      
    if(tecla != '\0'){
      valorRep[contRep] = tecla; 
      valorRep[contRep + 1] = '\0'; // Asegurarse que el Array termino 
      rep = atol(valorRep); // Se convierte el valor
      sprintf(str, "%d", rep);
      lcd_gotoxy(0, 1); 
      lcd_putc(str); 
      vTaskDelay(100 / portTICK_PERIOD_MS);
      contRep++;
      tecla = '\0';
    } 

    if (contRep == 2) { 
      contRep = 0; 
      valorRep[0] = '\0'; 
      vTaskDelay(500 / portTICK_PERIOD_MS); 
    }
  }
}

void selecVel() {
  char valorVel[2] = {0}; // Inicializa el array y lo llena con ceros
  tecla = read_keypad(); // Lee la tecla presionada inicialmente

  // Bucle hasta que se presione '1', '2' o '3'
  while (tecla != '1' && tecla != '2' && tecla != '3') {
    tecla = read_keypad(); // Lee la próxima tecla presionada
    usleep(10); // Pausa de 10 ms
  }

  valorVel[0] = tecla;  // Almacena la tecla válida en valorVel
  valorVel[1] = '\0';   // Asegura que el array esté terminado con un carácter nulo
  Vel = atol(valorVel); // Convierte el valor almacenado en valorVel a un número entero largo
}


void rutinaUno() {
  aux = 0;
  lcd_clear();
  lcd_gotoxy(0, 0);
  lcd_putc("     Repeticiones:");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  lcd_gotoxy(0, 1);
  sprintf(str, "%d", aux);
  lcd_putc(str);
  lcd_gotoxy(7, 1);
  lcd_putc("de");
  lcd_gotoxy(13, 1);
  sprintf(str, "%d", rep);
  lcd_putc(str);
  int contRU = 0;
  while (contRU < rep) {
    aux = contRU + 1;
    lcd_gotoxy(0, 1);
    sprintf(str, "%d", aux);
    lcd_putc(str);
    flashFE();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    contRU++;
  }
}

void rutinaDos() {
  aux = 0;
  lcd_clear();
  lcd_gotoxy(0, 0);
  lcd_putc("     Repeticiones:");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  lcd_gotoxy(0, 1);
  sprintf(str, "%d", aux);
  lcd_putc(str);
  lcd_gotoxy(7, 1);
  lcd_putc("de");
  lcd_gotoxy(13, 1);
  sprintf(str, "%d", rep);
  lcd_putc(str);
  int contRD = 0;

  while (contRD < rep) {
    aux = contRD + 1;
    lcd_gotoxy(0, 1);
    sprintf(str, "%d", aux);
    lcd_putc(str);
    flashAA();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    contRD++;
  }
}

void rutinaTres() {
  aux = 0;
  lcd_clear();
  lcd_gotoxy(0, 0);
  lcd_putc("     Repeticiones:");
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  lcd_gotoxy(0, 1);
  sprintf(str, "%d", aux);
  lcd_putc(str);
  lcd_gotoxy(7, 1);
  lcd_putc("de");
  lcd_gotoxy(13, 1);
  sprintf(str, "%d", rep);
  lcd_putc(str);
  int contRT = 0;

  while (contRT < rep) {
    aux = contRT + 1;
    lcd_gotoxy(0, 1);
    sprintf(str, "%d", aux);
    lcd_putc(str);
    flashFE();
    vTaskDelay(500 / portTICK_PERIOD_MS);
    flashAA();
    vTaskDelay(500 / portTICK_PERIOD_MS);
    contRT++;
  }
}

void confirm() {
  tecla = read_keypad();
  while (tecla != '#') {
    tecla = read_keypad();
    usleep(500);
  }
}

void flashAA() {
   
}

void flashFE() {

}