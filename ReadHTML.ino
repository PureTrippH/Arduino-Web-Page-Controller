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
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
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
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if(Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Cable not connected");
  } else {
    server.begin();
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
  }

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("test.txt", FILE_WRITE);

  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to test.txt...");
    myFile.println("testing 1, 2, 3.");
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
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
            while (htmlFile.available()) {
              client.write(htmlFile.read());
            }
            // close the file:
            htmlFile.close();

          } else {
            Serial.println("Wont Work");
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
