
// This example uses an ESP-01 +  3V or 5v doorbell + Mobile charger(5V) + AWS1117-3.3V + Two 10uF capacitances + One 10K register(for pull up GPIO 0 ) 
// The current of my system about 30mA. Use 2000mAH 18650 battery, will run 3-days. So don't suggest use battery.
// installed Mosquitto broker on OpenWrt router or Raspberry Pi or other ARM linux device. "opkg install mosquitto" "ipkg install mosquitto" "apt-get install mosquitto" 
// to connect to 192.168.1.1:1883  *** It's only for intranet. because use plaintext communication. Please don't use on internet server (like your VPS etc) ***  
// GPIO 0 - Bell key
// by Xinren Dang
// https://github.com/Dangxinren/ESP8266

#include <ESP8266WiFi.h>
#include <MQTTClient.h>

const char *ssid = "..........";    //change it to your WiFi name
const char *pass = "..........";    // your WiFi password
int bellkey = 0;                    // Bellkey be set GPIO 0

WiFiClient net;
MQTTClient clientmqtt;

//void connect(); // <- predefine connect() for setup()

void setup() {    
  WiFi.begin(ssid, pass);
  delay(50);    //wait 50ms for WiFi connection.
  clientmqtt.begin("192.168.1.1", net);    //connect to MQTT broker
  pinMode(bellkey, OUTPUT);              // define bellkey output
  digitalWrite(bellkey, HIGH);     // Bell is Low-effect
  netconnect();
}

void netconnect() {
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
  while (!clientmqtt.connect("esp01", "bell", "bell")) {
    delay(1000);
  }
  clientmqtt.publish("/bell/state", "Connected");
  clientmqtt.subscribe("/bell/control");
}

void loop() {
  clientmqtt.loop();
  delay(200);      // 1 round per 200ms
  if (!clientmqtt.connected()) {
    netconnect();
  }
}

void messageReceived(String topic, String payload, char * bytes, unsigned int length) {
  if (payload[0] == 'B') {               // sim Bellkey pushed down. 
    digitalWrite(bellkey, LOW);
    delay(10);
    digitalWrite(bellkey, HIGH);
     clientmqtt.publish("/bell/state", "Bell singing.");
  }
  if (payload[0] == 'C') {                         // Check the state of ESP-01.
     clientmqtt.publish("/bell/state", "Connected");
  }
}



