/* Proyecto PST
 *  Termometro a distancia con alarma
*/
// Librerias
//Librerias
#include <Wire.h> //comms
#include <Adafruit_MLX90614.h> //Termico
#include <LiquidCrystal_I2C.h> //Pantalla
#include <DS3231.h> //Reloj
#include <SR04.h> //Ultrasonico
#include "pitches.h" //Buzzer

// Asignacion de pines
#define Echo 11 //Echo del Ultrasonico
#define Trig 12 //Trig del Ultrasonico
#define LP 13 //Salida LED

// Inicializaciones
Adafruit_MLX90614 mlx = Adafruit_MLX90614(); //Sensor Termico
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); // Pantalla LCD
DS3231 clock; //Reloj
RTCDateTime dt; //Fecha
SR04 sr04=SR04(Echo,Trig); //Ultrasonico

//Variables
int Espera1=500; //Espera en el loop

//Fecha
String fDia;
String fMes; //Mes en texto
String fAno;
String fHora;
String fMin;
String fSeg;
String fTime;

//Distancia
int Dist; //Distancia del ultrasonico
int DistMin=10; //Distancia minima para detectar al sujeto (mm)
int Presente=0; //Si hay alguien frente al Termometro
int Espera=3000; //Tiempo de espera para verificar sujeto
unsigned long Tiempo=0; //Tiempo que lleva detectado para Millis
int Ahora=0; //Millis en el momento que se inicia

//Temperatura
float TempObj; //Temperatura del sujeto
float TempMax=37.00; //Temperatura maxima permitida
int TpoAlarma=200; //Tiempo de Alarma por alta temperatura

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:

}
