/*
  SD card read/write

  This example shows how to read and write data to and from an SD card file
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

  created   Nov 2010
  by David A. Mellis
  modified 9 Apr 2012
  by Tom Igoe

  This example code is in the public domain.

*/

#include <SPI.h>
#include <SD.h>

#include <Ethernet.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 2);
IPAddress subnet (255, 255, 255, 0);
IPAddress gateway(192, 168, 0, 1);
EthernetServer server(80);  

String readString; 
File myFile;
File root;

void setup() {
  pinMode(4, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(4, HIGH);
  Serial.begin(9600);
  while (!Serial) {
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  Serial.println("Time to Init Ethernet");
  Ethernet.begin(mac, ip);
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); 
    }
  }
  if(Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Cable not connected");
  } else {
    server.begin();
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
  }
}

void loop() {
EthernetClient client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
          readString += c; 
          
        //if HTTP request has ended
        
        if (c == '\n') {
           readAJAX(readString);
          ///////////////
          //print to serial monitor for debuging 
          client.println("HTTP/1.1 200 OK"); //send new page
          client.println("Content-Type: text/html");
          client.println("Access-Control-Allow-Origin: *");
          client.println();

          File htmlFile = SD.open("index.htm");
          
          if (htmlFile) {
            //Serial.println("test.txt:");
            // read from the file until there's nothing else in it:
            byte clientBuffer[1400];
            int byteCount = 0;
            while (htmlFile.available()) {
              clientBuffer[byteCount] = htmlFile.read();
              byteCount++;
              if(byteCount > (sizeof(clientBuffer)-1)) {
                client.write(clientBuffer, 1400);
                byteCount = 0;
              }
            }
            if(byteCount > 0) client.write(clientBuffer, byteCount);
            // close the file:
            htmlFile.close();

          }
            //stopping client
            client.stop();
            readString="";
          //}
        }
      }
    }
  } 
}

void readAJAX(String request) {
  request.toLowerCase();
  Serial.println(request);
  if(request.indexOf("led1=1") > 0) {
    digitalWrite(13, HIGH);
  }
  if(request.indexOf("led1=0") > 0) {
    digitalWrite(13, LOW);
  }
}
