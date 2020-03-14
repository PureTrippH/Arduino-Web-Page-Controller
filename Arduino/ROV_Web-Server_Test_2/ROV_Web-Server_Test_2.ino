#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <Bridge.h>
 
#define SS_SD_CARD 4
#define SS_ETHERNET 20
 
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 2);
IPAddress subnet (255, 255, 255, 0);
IPAddress gateway(192, 168, 0, 1);
EthernetServer server(80);  

File webPage;
int RED = 8;              //LED to turn on/off
int tval = 0;
String HTTP_req;          // stores the HTTP request

void setup()
{
  pinMode(SS_SD_CARD, OUTPUT);
  pinMode(SS_ETHERNET, OUTPUT);
  digitalWrite(SS_SD_CARD, HIGH);  
  digitalWrite(SS_ETHERNET, LOW);
 
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Ethernet WebServer Example");
 
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
 
  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }
  //start the server
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
   
    // initialize SD card
    Serial.println("Initializing SD card...");
    if (!SD.begin(4)) {
        Serial.println("ERROR - SD card initialization failed!");
        return;    // init failed
    }
    Serial.println("SUCCESS - SD card initialized.");
    // check for index.htm file
    if (!SD.exists("index.htm")) {
        Serial.println("ERROR - Can't find index.htm file!");
        return;  // can't find index file
    }
    Serial.println("SUCCESS - Found index.htm file.");
    HTTP_req = "";
    //Set LED to output
    pinMode(RED, OUTPUT);
}

void loop()
{
  // listen for incoming clients
  Serial.println("Cheking for client...");
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read(); // read 1 byte (character) from client
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if ( HTTP_req.length() < 80)
          HTTP_req += c;  // save the HTTP request 1 char at a time
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          // send web page
          if (HTTP_req.indexOf("ajaxrefresh") >= 0 ) {
            ajaxRequest(client);  //update the analog values
            break;
          }
          else if (HTTP_req.indexOf("ledstatus") >= 0 ) {
            ledChangeStatus(client); //change the LED state
            break;
          } 
          else {
            tval = HTTP_req.indexOf("tval");
            throttleChangeStatus(client);
            Serial.print(HTTP_req);
            webPage = SD.open("index.htm");        // open web page file
            if (webPage) {
              while (webPage.available()) {
                client.write(webPage.read()); // send web page to client
              }
              webPage.close();
            }
            break;
          }
          if (c == '\n') {
            // you're starting a new line
            currentLineIsBlank = true;
          } else if (c != '\r') {
            // you've gotten a character on the current line
            currentLineIsBlank = false;
          }
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    HTTP_req = "";
    Serial.println("client disconnected");
  } // end if (client)
}

// send the state of the switch to the web browser
void ajaxRequest(EthernetClient client)
{
  for (int analogChannel = 0; analogChannel < 5; analogChannel++) {
    int sensorReading = analogRead(analogChannel);
    client.print("analog input ");
    client.print(analogChannel);
    client.print(" is ");
    client.print(sensorReading);
    client.println("<br />");
  }
}

void ledChangeStatus(EthernetClient client)
{
  int state = digitalRead(RED);
  Serial.println(state);
  if (state == 1) {
    digitalWrite(RED, LOW);
    client.print("OFF");
  }
  else {
    digitalWrite(RED, HIGH);
    client.print("ON");
  }
}

void throttleChangeStatus(EthernetClient client) 
{ 
  Serial.println(tval);
  client.print(tval);
}
