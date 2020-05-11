#include <LiquidCrystal.h>        //biblioteca do lcd
#include <Adafruit_Fingerprint.h> //biblioteca do sensor
#include <Servo.h>                //biblioteca do servo

LiquidCrystal lcd(4,5,10,11,12,13); //variavel do lcd/portas de dados do lcd
SoftwareSerial sensor(2, 3);        //variavel do sensor/portas de dados do sensor
Servo servo;                        //variavel do servor
int botao = 6;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);    //o lcd tem duas filas e 16 colunas
  servo.attach(8);  //porta de dados do servo motor

}

void loop() {
  // put your main code here, to run repeatedly:

}
