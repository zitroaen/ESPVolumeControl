#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //http/github.com/tzapu/WiFiManager
#include <ESP8266HTTPClient.h>


///////////////////////////////User Settings ////////////////////////////////////////
String restAddress = "http://192.168.111.111:8080/rest/items/";         //IP Adress of the OpenHAB server
String volumeItemName = "SonosPLAY5_Volume";                            //Name of the dimmer/volume item we want to control

///////////////////////////////Hardware Configuration/////////////////////////////////

//Configuration for KY-040 Rotary Encoder:
int pinA = 5;  // Connected to CLK on KY-040
int pinB = 4;  // Connected to DT on KY-040
int pinALast;
int aVal;
boolean bCW;

///////////////////////////////Other Variables////////////////////////////////////////
int delayTime = 100;    //Minimum delay time after one value has been sent to the openHAB server [ms]
int lastTransmit;       //used to store the millis() of the last time a value was sent

int pollingInterval = 2000; //how often is the actual volume of the Sonos fetched from the openHAB server [ms]
int lastVolumePoll;        //used to store the millis() of the last time a value was requested

int currentVolume;      //stores current Volume
boolean changed;        //Indicates if Volume has been changed since last send

HTTPClient http;  //Used to communicate with the REST interface of openHAB


//////////////////////End Variables////////////////////////

void transmit(String message) {  //Function to transmit Values to REST API
  if (changed) {               //Makes sure that messages are only transmitted if the value has been changed
    if (millis() > lastTransmit + delayTime) {  //checks if the delaytime has already been reached
      http.begin(restAddress + volumeItemName);
      http.addHeader("text/plain", "application/json");
      http.POST(message);         //Post value to openHAB server
      http.writeToStream(&Serial);  //If somethin goes wrong it will be written to Serial
      http.end();
      changed = false;          //Set the change Flag to false, so the value isn't constantly transmitted
      lastTransmit = millis();  //save the current time to variable so the delay can be calculated for the next transmit
    }
  }
}


void clockwise() {        //called, when encoder rotates clockwise...
  if (currentVolume < 100) { //don't raise volume above 100%
    currentVolume++;         //raise volume
    changed = true;          //set changeflag, so the transmit function knows that there's something to transmit
  }

}

void counterclockwise() {    //called, when encoder rotates clockwise...
  if (currentVolume > 0) {   //only accepts positive values
    currentVolume--;         //lower volume
    changed = true;          //set changeflag, so the transmit function knows that there's something to transmit
  }
}


int getCurrentVolume() {     //Function that gets current volume from openHAB server
  http.begin(restAddress + volumeItemName + "/state");    //Begin connection to the Server
  int httpCode = http.GET();          //Make GET-Request
  if (httpCode > 0) {                 //Check the returning code
    lastVolumePoll = millis();        //Remember when the Volume was fetched from the server
    if (httpCode = 200) {             //if HTTP-Response is OK
      String payload = http.getString();   //Get the request response payload
      http.end();                       //End HTTP-Connection
      return payload.toInt();           //Convert answer to integer and return that
    } else {         //If he HTTP-Response is not OK
      return 0;
    }
  }
  return 0; //HTTP-Code is 0
}


void setup() {
  Serial.begin(115200);
  pinMode (pinA, INPUT);
  pinMode (pinB, INPUT);
  pinALast = digitalRead(pinA);



  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AutoConnectAP");
  Serial.println("connected...yeey :)");

  Serial.println("Getting current Volume...");
  currentVolume = getCurrentVolume(); //Get current Volume from item on openHAB server and save it
  Serial.print("Current Volume is: ");
  Serial.print(currentVolume);
  Serial.println("%");
}


void loop() {

  transmit(String(currentVolume));      //Call transmit function each loop

  //////////////////Reading the rotary encoder
  aVal = digitalRead(pinA);
  if (aVal != pinALast) { // Means the knob is rotating
    // if the knob is rotating, we need to determine direction
    // We do that by reading pin B.
    if (digitalRead(pinB) != aVal) {  // Means pin A Changed first - We're Rotating clockwise
      clockwise();
    } else {// Otherwise B changed first and we're moving counter clockwise
      counterclockwise();
    }
    pinALast = aVal;
  }
  //////////////////End reading the rotary encoder

  if (millis() > lastVolumePoll + pollingInterval) {      //check if it's time to request the Volume from the Server again..
    currentVolume = getCurrentVolume();                   //request Volume and set it to current Volume
    Serial.print("Volume has been Updated from the server to: ");
    Serial.print(currentVolume);
    Serial.println("%");
  }
}
