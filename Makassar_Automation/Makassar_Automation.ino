/*-----------Deskripsi Pin Pada LCD-----------------
Pin 1 : GND
Pin 2 : Vcc
Pin 3 : VEE
Pin 4 : Register Selecy
Pin 5 : Read / Write
Pin 6 : Enable
Pin 7 : Data 0
Pin 8 : Data 1
Pin 9 : Data 2
Pin 10 : Data 3
Pin 11 : Data 4
Pin 12 : Data 5
Pin 13 : Data 6
Pin 14 : Data 7
Pin 15 : LCD Light +
Pin 16 : LCD Light -
// lcd(RS, Enable, D4, D5, D6, D7);
---------------------------------------------------*/

#include<LiquidCrystal.h>
#define analogInPin  A5  
#define relay_1 7
#define relay_2 6

LiquidCrystal lcd(25 ,27 ,29 ,31 ,33 ,35); 

int sensorValue; 
float outputValueConductivity; 
float outputValueTDS; 

void setup() {
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Hello . . .");
  pinMode(relay_1,OUTPUT);
  pinMode(relay_2,OUTPUT);
}

void loop() {
  sensorValue = analogRead(analogInPin);
  lcd.clear();  
  lcd.setCursor(0,0);
  lcd.print("Baca Analog : ");
  lcd.setCursor(0,1);
  lcd.print(sensorValue);
  delay(1000);
  outputValueConductivity = (0.2142*sensorValue)+494.93;
  outputValueTDS = (0.3417*sensorValue)+281.08;
  String str_conductivity, str_tds;
  str_conductivity = String(outputValueConductivity) + (" uSiemens");
  str_tds = String(outputValueTDS) + (" ppm");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(str_conductivity);
  lcd.setCursor(0,1);
  lcd.print(str_tds);
  action(outputValueConductivity);
  Serial.print("sensor ADC = ");
  Serial.print(sensorValue);
  Serial.print("  conductivity (uSiemens)= ");
  Serial.print(outputValueConductivity);
  Serial.print("  TDS(ppm)= ");
  Serial.println(outputValueTDS);  
  delay(3000);
}

void action(float conductivity){
  if(conductivity > 600){
    digitalWrite(relay_1,1);
  }else{
    digitalWrite(relay_1,0);
  }
}

