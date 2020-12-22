/* Proyecto PST
 *  Termometro a distancia con alarma
 *  USB Host Shield
*/
//Librerias
#include <Wire.h> //comms
#include <Adafruit_MLX90614.h> //Termico
#include <LiquidCrystal_I2C.h> //Pantalla
#include <DS3231.h> //Reloj
#include <SR04.h> //Ultrasonico
#include "pitches.h" //Buzzer
////////Librerias para lector de Barras
#include <hid.h> // Agregar el código Oleg Mazurov al lector de códigos de barras
#include <hiduniversal.h> // Agregar el código Oleg Mazurov al lector de códigos de barras
#include <usbhub.h>
#include <avr / pgmspace.h>
#include <Usb.h>
#include <usbhub.h>
#include <avr / pgmspace.h>
#include <hidboot.h>


// Asignacion de pines
#define Echo 6 //Echo del Ultrasonico
#define Trig 7 //Trig del Ultrasonico
#define LP 5 //Salida LED

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


//Funcinoamiento de Lector de barra
USB USB;
HIDUniversal Hid (y Usb); // Agrega esta línea para que se reconozca el escáner de código de barras, yo uso "Hid" debajo
HIDBoot <HID_PROTOCOL_KEYBOARD> Teclado (y Usb);

clase KbdRptParser: público KeyboardReportParser
{
PrintKey vacío (mod uint8_t, clave uint8_t); // Agrega esta línea para imprimir el carácter en ASCII
protegido:
OnKeyDown vacío virtual (mod uint8_t, clave uint8_t);
virtual vacío OnKeyPressed (clave uint8_t);
};

void KbdRptParser :: OnKeyDown (uint8_t mod, uint8_t clave)
{
  uint8_t c = OemToAscii (mod, clave);
  si (c)
  OnKeyPressed (c);
}

/ * qué hacer cuando llega el símbolo * /
void KbdRptParser :: OnKeyPressed (clave uint8_t)
{
  if (clave! = 19) Serial.print (clave (char));
  más Serial.print ((char) 0x0D);
};
KbdRptParser Prs;

configuración vacía ()
{
  Serial.begin (9600); 
  if (Usb.Init () == - 1) Serial.println ("OSC no se inició");
  else Serial.println ("Código de barras listo");
  Hid.SetReportParser (0, (HIDReportParser *) & Prs); // Cambiar "Teclado" por "Oculto"
}

bucle vacío ()
{
  Usb.Task ();
}

void setup() 
{
  pinMode(LP,OUTPUT); //LED
  mlx.begin(); //Termico

  clock.begin(); //Reloj
  //clock.setDateTime(2020,7,9,20,19,0); //Quitar comentaro y ajustar a la hora deseada en primer run.

  //Sonido inicial
  tone(4,NOTE_C5,3000);
  digitalWrite(LP,HIGH);
  delay(3000);
  digitalWrite(LP,LOW);

//Mensaje inicial
  lcd.begin(20,4);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" TERMOMETRO / RELOJ");
  lcd.setCursor(0,1);
  lcd.print("AMB: ");
  lcd.setCursor(0,2);
  lcd.print("PER: ");
}

void loop() 
{
//Distancia
Dist=sr04.Distance();
if(Dist>DistMin) //No hay nadie
{
  Presente=0;
  Tiempo=millis();
}
if(Dist<=DistMin && Presente==0) //Llego alguien, tomemos el tiempo
{
  Presente=1;
  Tiempo=millis();
}

if(Presente==1)
{
  if(millis()-Tiempo>Espera) //Se completo el tiempo
  {
    Presente=2;
  }
}

//Actualizar pantalla
lcd.setCursor(12,3);
//Temperaturas
lcd.setCursor(4,1);
lcd.print(mlx.readAmbientTempC()); //imprime el valor de la temperatura en el lcd
lcd.setCursor(9,1);
lcd.print("c");

TempObj=mlx.readObjectTempC(); //Actualiza el valor de temperatura
switch(Presente)
{
  case 0: //No hay nadie
  lcd.setCursor(4,2);
  lcd.print("--.--c");
  lcd.setCursor(0,3);
  lcd.print("BUSCANDO...  ");
  break;

  case 1: //Llego alguien
  lcd.setCursor(4,2);
  lcd.print("--.--c");
  lcd.setCursor(0,3);
  lcd.print("LEYENDO...");
  tone(4,NOTE_C5,TpoAlarma);
  break;

  case 2: //Se Completo el tiempo
  lcd.setCursor(4,2);
  lcd.print(TempObj);
  lcd.setCursor(9,2);
  lcd.print("c");
  if(TempObj>TempMax)
  {
    tone(10,NOTE_G5,TpoAlarma); //suena la alarma
   lcd.setCursor(0,3);
   lcd.print("TEMP ALTA!!!");
   digitalWrite(LP,HIGH);
   delay(TpoAlarma);
   digitalWrite(LP,LOW);
  }
  else
  {
  lcd.setCursor(0,3);
  lcd.print("NORMAL        ");
  }
  break;
}

//Hora y Fecha
dt=clock.getDateTime();
fDia=String(dt.day);
if(dt.day<10)fDia="0"+fDia; //Para mantener en dos digitos

fAno=String(dt.year-2000);

lcd.setCursor(11,1);
lcd.print(fDia+"/"+ Mes()+"/"+fAno); //Uso de la funcion Mes()

fHora=String(dt.hour);
if(dt.hour<10)fHora="0"+fHora; //Para mantener en dos digitos

fMin=String(dt.minute);
if(dt.minute<10)fMin="0"+fMin; //Para mantener en dos digitos

fSeg=String(dt.second);
if(dt.second<10)fSeg="0"+fSeg; //Para mantener en dos digitos

fTime=fHora+":" + fMin + ":" + fSeg;
lcd.setCursor(11,2);
lcd.print(fTime);

delay(Espera1);

}

String Mes()
{
  //Serial.println(dt.month);
  switch (dt.month)
  {
    case 1:
    fMes="Ene";
    break;

    case 2:
    fMes="Feb";
    break;

    case 3:
    fMes="Mar";
    break;

    case 4:
    fMes="Abr";
    break;

    case 5:
    fMes="May";
    break;

    case 6:
    fMes="Jun";
    break;

    case 7:
    fMes="Jul";
    break;

    case 8:
    fMes="Ago";
    break;

    case 9:
    fMes="Sep";
    break;

    case 10:
    fMes="Oct";
    break;

    case 11:
    fMes="Nov";
    break;

    case 12:
    fMes="Dic";
    break;
  }

return fMes;
}
