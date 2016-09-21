/*
 *  GRCS_COMSv2
 *  By: Joey Hines
 *  
 *  For use on a ESP8266 connected to an Arduino witg GRCSv1-2
 *  via serial (May work on a WiFi sheild with proper 
 *  include)
 *  
 *  Listens for incoming UDP packets on the specified
 *  port and passes them to the Arduino over Serial
 *  
 *  Changes:
 *  +The ESP8266 now works as a WiFi-hotspot so an external 
 *  WiFi network is not required
 *    -This also means that the ESP always has a fixed IP
 *     address 
 */
 
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

unsigned int port = 80; //UDP Port

//Char array for storing incomung UDP packets
char  incomingPacket[UDP_TX_PACKET_MAX_SIZE];

//SSID and Password for AP
const char* ssid     = "GRCS";
//const char* password = "5896321837";

//WiFi UDP object
WiFiUDP Udp;

void setup() {
  //Begin serial at 9600 and wait to connect
  Serial.begin(9600);
  delay(100);

  //Set the status pin to output
  pinMode(0, OUTPUT);
  WiFi.mode(WIFI_AP);
  //Begin WiFi AP
  WiFi.softAP(ssid);

  /*
  //Print out connection status and IP address
  Serial.println("");
  Serial.println("AP Online");  
  Serial.println("IP address: ");
  Serial.println(WiFi.softAPIP());
  */

  //Begin listening for UDP packets on port
  Udp.begin(port);
  
  
}
 
void loop() {
  //Keep the status LED on
  digitalWrite(0, LOW);

  //Tru and parse a packet from the UDP stream
  int packetSize = Udp.parsePacket();

  //If a packet is detected
  if (packetSize) {
    //Read the packet into incomingPacket
    Udp.read(incomingPacket, UDP_TX_PACKET_MAX_SIZE);
    //Concert the incomungPacket c-string into a string
    //This step may not be required
    String dataRecv(incomingPacket);
    //Print the packet to serial
    Serial.println(dataRecv);
    
  }
  //Clear the incomingPacket array
  memset(incomingPacket, 0, UDP_TX_PACKET_MAX_SIZE);
}
