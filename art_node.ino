#include <ESP8266WiFi.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <ArtnetWifi.h>
#include <espDMX.h>
#include <Ticker.h>

//DMX Pin: 2 (WEMOS: D4)

const int dmx_led = 13;
const int wifi_led = 12;

Ticker ticker;

WiFiUDP UdpSend;
ArtnetWifi artnet;

void tick()
{
  int state = digitalRead(wifi_led);
  digitalWrite(wifi_led, !state);
}

void configModeCallback (WiFiManager *myWiFiManager) {
  WiFi.softAPIP();
  ticker.attach(0.2, tick);
}

void setup() {
  delay(10);
  pinMode(wifi_led, OUTPUT);
  pinMode(dmx_led, OUTPUT);
  ticker.attach(0.5, tick);
  
  WiFiManager wifiManager;

  wifiManager.setAPCallback(configModeCallback);

  if (!wifiManager.autoConnect("ArtNet-Node")) {
    ESP.reset();
    delay(1000);
  }
  
  ticker.detach();
  digitalWrite(dmx_led, LOW);

  dmxB.begin(dmx_led);
  
  artnet.setArtDmxCallback(onDmxFrame);
  artnet.begin();
}

void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data){
  if(universe == 0){
    digitalWrite(wifi_led, HIGH);
    dmxB.setChans(data);
    digitalWrite(wifi_led, LOW);
  }
}

void loop() {
  artnet.read();
  delay(5);
}
