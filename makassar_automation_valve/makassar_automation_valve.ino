#include <SPI.h>
#include <Ethernet.h>
#include <SoftwareSerial.h>

// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
IPAddress server(192,168,3,9);  
//char server[] = "www.google.com";    // name address for Google (using DNS)

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 3, 177);

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
EthernetClient myEthernet;

int relayPin = 7;
boolean startRead = false; 
char inString[32];

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip);
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  Serial.println("connecting...");

  // if you get a connection, report back via serial:
  if (myEthernet.connect(server, 80)) {
    Serial.println("connected");
    // Make a HTTP request:
    myEthernet.println("GET /makassar_automation/index.php/api/get_recent HTTP/1.1");
    myEthernet.println("Host: 192.168.3.9");
    myEthernet.println("Connection: close");
    myEthernet.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("connection failed");
  }

   pinMode(relayPin, OUTPUT);
   digitalWrite(relayPin, 0);
}

void decideOnOrOff(String command){
  if(command.equals("RELAY ON")){
     digitalWrite(relayPin, 1);
  }else if(command.equals("RELAY OFF")){
     digitalWrite(relayPin, 0);
  }
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  String res="";
  if (myEthernet.available()) {
  //    char c = client.read();
    res=bacaWebText();
    if(res.equals("")==false){
      Serial.println("Response dari Server : ");
      Serial.println(res);
      decideOnOrOff(res);
    }
  //    Serial.print(c);
  }

  // if the server's disconnected, stop the client:
  if (!myEthernet.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    myEthernet.stop();

    // do nothing forevermore:
    while (true);
  }
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
