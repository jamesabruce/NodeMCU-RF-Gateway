#include <MQTTClient.h>
#include <ESP8266WiFi.h>
#include <RCSwitch.h>

const char* ssid = "YOURWIFINAME";
const char* password = "YOURWIFIPASSWORD"; 

const char* subscribeTopic = "livingroom/control/#"; // subscribe to this topic; anything sent here will be passed into the messageReceived function
const char* server = "192.168.1.99"; // server or URL of MQTT broker
String clientName = "livingroomnodemcu-"; // just a name used to talk to MQTT broker - doesn't actually matter. 

unsigned long resetPeriod = 86400000; // 1 day - this is the period after which we restart the CPU, to deal with odd memory leak errors

WiFiClient wifiClient;
MQTTClient client;
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

void setup() {

  
  // Transmitter is connected to NodeMCU GPIO 2 / D4  
  mySwitch.enableTransmit(2);

  Serial.begin(115200);
  
  client.begin(server,wifiClient);
  Serial.print("Connecting to ");
  Serial.println(ssid);  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Generate client name based on MAC address and last 8 bits of microsecond counter
  uint8_t mac[6];
  WiFi.macAddress(mac);
  clientName += macToStr(mac);
  clientName += "-";
  clientName += String(micros() & 0xff, 16);

  Serial.print("Connecting to ");
  Serial.print(server);
  Serial.print(" as ");
  Serial.println(clientName);
  
  if (client.connect((char*) clientName.c_str())) {
    Serial.println("Connected to MQTT broker");
    Serial.print("Subscribed to: ");
    Serial.println(subscribeTopic);
    client.subscribe(subscribeTopic);

  }
  else {
    Serial.println("MQTT connect failed");
    Serial.println("Will reset and try again...");
    abort();
  }

  
}

void loop() {

  client.loop();

  // reset after a day to avoid memory leaks 
  if(millis()>resetPeriod){
    ESP.restart();
  }
}

/* This is where the magic happens, and you'll need to modify this to support custom items. At the moment, it listens
 *   to (first bit derived from channel name set at start):
 *   livingroom/control/switch/x/y (with payload value of on or off)
 *   and
 *   livingroom/control/screen (with payload value of up or down)
 *   You should be able to modify below to easily add more commands if needed
 */
    
void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
  Serial.print("incoming: ");
  Serial.print(topic);
  Serial.print(" - ");
  Serial.print(payload);
  Serial.println();

  if (topic.indexOf("switch") >=0){
    //switch control, parse out the channel and plug id 
    
    int channel = getValue(topic,'/',3).toInt();
    int plug = getValue(topic,'/',4).toInt();
    
    if(payload == "on"){   
      mySwitch.switchOn(channel, plug);
    }
    else{
      mySwitch.switchOff(channel, plug);
    }
  }
  else if (topic.indexOf("screen") >=0){
    //screen control
    if(payload == "up"){
      screenUp();
    }
    else if(payload == "down"){
      screenDown();
    }
  }
  /* add another else if here to listen for more commands (or just modify the one above if you dont want screen) */
}

// Custom functions for RCSwitch. Define your own, then link them to an MQTT channel. 
// Note that my screen actually requires TWO button presses (not a long press, but two physical presses), so I'm delaying a bit then sending the same signal again

void screenUp(){
  mySwitch.setPulseLength(358);
  mySwitch.send(8694273,24);
  delay(2000);
  mySwitch.send(8694273,24);
}

void screenDown(){
  mySwitch.setPulseLength(358);
  mySwitch.send(8694276,24);
  delay(2000);
  mySwitch.send(8694276,24);
}


/*Utility functions, no need to edit below here */

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

// getValue function taken from http://arduino.stackexchange.com/questions/1013/how-do-i-split-an-incoming-string/1237
String getValue(String data, char separator, int index)
{
 int found = 0;
  int strIndex[] = {
0, -1  };
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++){
  if(data.charAt(i)==separator || i==maxIndex){
  found++;
  strIndex[0] = strIndex[1]+1;
  strIndex[1] = (i == maxIndex) ? i+1 : i;
  }
 }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

