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
#include <Ethernet.h>
#include <SPI.h>
#include<LiquidCrystal.h>

#define analogInPin  A5  
#define relay_1 7
#define relay_2 6
#define STATUS_CONNECTED 1
#define STATUS_DISCONNECTED 0

char namaServer[] = "169.254.110.195";
byte IP_eth[] = {169,254,110,196};
byte MAC_eth[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

int respon_dht11;
int counter = 0;
int iterasi = 0;

boolean startRead = false; 

char inString[32];
char charFromWeb[9];

EthernetClient myEthernet;

LiquidCrystal lcd(25 ,27 ,29 ,31 ,33 ,35); 

void setup() {
  lcd.begin(16,2);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Hello . . .");
  Serial.begin(9600);
  Serial.println("--------------------------------------------------"); 
  Serial.println("Setting Perangkat");
  Serial.println("Mohon menunggu . . . ");
  Serial.println("Setting Ethernet MAC Address dan IP Address");
  Serial.println("Mohon menunggu . . . ");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Setup perangkat");
  lcd.setCursor(0,1);
  lcd.print("Loading  . . .");
  if (Ethernet.begin(MAC_eth) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    Ethernet.begin(MAC_eth,IP_eth);
  }
//  Ethernet.begin(MAC_eth,IP_eth);
  pinMode(relay_1,OUTPUT);
  pinMode(relay_2,OUTPUT);
}

void loop() {
  iterasi++;
  Serial.print("Iterasi ke : ");
  Serial.println(iterasi);
  int resultBukaKoneksi = bukaKoneksi();
  String data = sensorData();
  if(resultBukaKoneksi==1){
    kirimData(data);
    Serial.println();
  }
  Serial.println("--------------------------------------------------");
  delay(3000);
}

int bukaKoneksi(){
  Serial.print("Mencoba sambungan ke server http://"); 
  Serial.println(namaServer);  
  Serial.println("Mohon menunggu . . . ");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Menghubungkan");
  lcd.setCursor(0,1);
  lcd.print("ke server . . ");
  if(myEthernet.connect(namaServer,80)){
    Serial.println("Sambungan ke server berhasil!");\
    return STATUS_CONNECTED; 
  }else{
    Serial.print("Sambungan ke server gagal!");
    Serial.println();
    return STATUS_DISCONNECTED;
  }
}

void kirimData(String a){
    Serial.println("Menjalankan perintah kirim data");
    String uri_segment;
    uri_segment = "/makassar_automation/index.php/api/save_data" + a;
    myEthernet.print("GET ");
    myEthernet.print(uri_segment);
    myEthernet.println(" HTTP/1.0");
    myEthernet.print( "Host: " );
    myEthernet.println(" 169.254.110.195 \r\n");
    delay(1000);
    Serial.println("Host OK");
    myEthernet.println( "Content-Type: application/x-www-form-urlencoded \r\n" );
    Serial.println("Content type OK");
//    myEthernet.print( "Content-Length: " );
//    myEthernet.print(ln);
    myEthernet.print(" \r\n");
    myEthernet.println( "Connection: close" );
    myEthernet.println();
    String res;
    res = bacaWebText();
    if(res.equals("")==false){
      Serial.println("Resonse dari Server : ");
      Serial.println(res);
    }
//------------warning selalu tutup koneksi ya........
//    myEthernet.stop();
//    myEthernet.flush();
//--------------------------------------------------
}

String bacaWebText(){
  unsigned int time;
  Serial.println("Baca respon dari server . . . "); 
  Serial.println("Mohon menunggu . . . ");
  time = millis();
  Serial.print("Timer Millis () : ");
  Serial.println(time);
  int stringPos = 0;
  memset( &inString, 0, 32 );
  int unvailable_ctr = 0;
  while(true){
    if (myEthernet.available()) {
      char c = myEthernet.read();
      Serial.print(c);
      if (c == '#' ) { 
        Serial.print("Menemukan start key # dengan isi : ");
        startRead = true;  
      }else if(startRead){
        if(c != '^'){ 
          inString[stringPos] = c;
          stringPos ++;
        }else{
          startRead = false;
          Serial.println();
          Serial.println("Baca respon dari server selesai!");
          myEthernet.stop();
          myEthernet.flush();
          Serial.println("Sambungan diputuskan . . . ");
          return inString;
        }
      }
    }else{
       //Serial.println("ethernet unavailable");
       delay(50);
       unvailable_ctr++;
       if(unvailable_ctr == 25){
         myEthernet.stop();
         myEthernet.flush();
         Serial.println("Koneksi mengalami time out");
         Serial.println("Sambungan diputuskan . . . ");
         Serial.println("Reset...");
         return inString;
       }
    }
  }
}

String sensorData(){
  int sensorValue; 
  float outputValueConductivity; 
  float outputValueTDS; 
  String sensor_data = "";
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
  Serial.print("sensor ADC = ");
  Serial.print(sensorValue);
  Serial.print("  conductivity (uSiemens)= ");
  Serial.print(outputValueConductivity);
  Serial.print("  TDS(ppm)= ");
  Serial.println(outputValueTDS);  
  sensor_data = "/" + String(outputValueConductivity) + "/" + String(outputValueTDS) + "/";
  return sensor_data;
}

