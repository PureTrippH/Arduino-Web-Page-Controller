#include <SPI.h>
#include <SD.h>

#include <Ethernet.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 2);
IPAddress subnet (255, 255, 255, 0);
IPAddress gateway(192, 168, 0, 1);
EthernetServer server(80);  
String valueKeyPairs[11][2]; //Array for Storing Key Value Pairs of Controller and inputs from AJAX

String readString; 
File myFile;
File root;

void setup() {
  pinMode(4, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(4, HIGH);
  Serial.begin(9600);
  //Test parse
  parseAJAXTwoD("post /?r=buttonOne=false?buttonTwo=false HTTP/1.1");
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
          Serial.println(readString);
          parseAJAXTwoD(readString);
          Serial.println(valueKeyPairs[0][0] + ": " + valueKeyPairs[0][1]);
          if(valueKeyPairs[0][1] == "true") {
            digitalWrite(13, HIGH);
          } else digitalWrite(13, LOW);
          if(valueKeyPairs[1][1] == "true") {
            digitalWrite(8, HIGH);
          } else digitalWrite(8, LOW);
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
  if(request.indexOf("led1=1") > 0) {
    digitalWrite(13, HIGH);
  }
  if(request.indexOf("led1=0") > 0) {
    digitalWrite(13, LOW);
  }
}


void parseAJAXTwoD(String request) {
  int oneDIndex = 0;
  int twoDStringIndex = 0;
  String valueKey = "";
  String var = "";
  
  if(request.substring(0, 4) == "POST") {
  for(auto &ch : request.substring(9)) { //Loops through requestString
    if(ch == '?' || ch == ' ') { //Detects key value and goes into nested loop       
        for(auto &chvar : valueKey) { //Loop through each character of key value
          if(chvar == '='  || chvar == ' ') { //Check for equal to split two strings into key and value
            Serial.println("ENTERED VALUE IF");
            valueKeyPairs[oneDIndex][0] = var; //Writes Key
            valueKeyPairs[oneDIndex][1] = valueKey.substring(twoDStringIndex + 1); //Writes Value
          } else {
            twoDStringIndex++;
            var += chvar;
          }
        }
        valueKey = "";
        var = "";
        twoDStringIndex=0;
        oneDIndex++; //increases the key-value array index
    } else {
      valueKey += ch; //records previous variables
    }
  }
  }
}