#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define LED_PIN 3
#define LED_COUNT 114

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

struct Line {
  int row;
  int begin;
  int end;
};

int R = 255;
int G = 255;
int B = 255;

int Brightness = 150;
int lastBrightness = 150; 

Line ES = {1, 1, 2};
Line ISCH = { 1, 4, 7 };
Line FÜF = { 1, 9, 11 };
Line VIERTU = { 2, 1, 6 };
Line ZÄÄ = { 2, 9, 11 };
Line ZWÄNZG = { 3, 1, 6 };
Line VOR = { 3, 9, 11 };
Line AB = { 4, 1, 2 };
Line HAUBI = { 4, 4, 8 };
Line EIS = { 5, 1, 3 };
Line ZWÖI = { 5, 4, 7 };
Line DRÜ = { 5, 9, 11 };
Line VIERI = { 6, 1, 5 };
Line FÜFI = { 6, 6, 9 };
Line SÄCHSI = { 7, 1, 6 };
Line SBINI = { 7, 7, 11 };
Line ACHTI = { 8, 1, 5 };
Line NÜNI = { 8, 6, 9 };
Line ZÄNI = { 9, 1, 4 };
Line EUFI = { 9, 8, 11 };
Line ZWÖUFI = { 10, 1, 6 };

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

const char *ssid = "WIFI NAME";
const char *password = "WIFI PASSWORD";
const char *mqtt_server = "SERVER IP";
const char *mqtt_user = "SERVER USERNAME";
const char *mqtt_password = "SERVER PASSWORD";

const char *light_status_topic = "Nils/WortUhr/light/status";
const char *light_switch_topic = "Nils/WortUhr/light/switch";
const char *brightness_status_topic = "Nils/WortUhr/brightness/status";
const char *brightness_set_topic = "Nils/WortUhr/brightness/set";
const char *rgb_status_topic = "Nils/WortUhr/rgb/status";
const char *rgb_set_topic = "Nils/WortUhr/rgb/set";

WiFiClient espClient;
PubSubClient client(espClient);

int lastHour = -1;
int lastMinute = -1;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");

  // Connect to MQTT broker
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  reconnect();

  timeClient.begin();

  strip.begin();
  strip.show();  // Initialize the strip once and show it
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();

  int hours = ((epochTime % 86400L) / 3600) + 1;
  int minutes = (epochTime % 3600) / 60;


  if (hours >= 12) {
    hours -= 12;
  }

  if (hours != lastHour || minutes != lastMinute) {
    updateTime(hours, minutes);

    lastHour = hours;
    lastMinute = minutes;
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("WortUhrClient", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe(light_switch_topic);
      client.subscribe(brightness_set_topic);
      client.subscribe(rgb_set_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

int roundTo5(int value) {
  return (value / 5) * 5;
}

void updateTime(int hours, int minutes) {
  int remainder = 0;
  Serial.println("updated watchface");

  strip.clear();

  remainder = minutes % 5;

  if (!remainder == 0) {
   for (int i = 1; i <= remainder; i++) {
     strip.setPixelColor(110 + i - 1, strip.Color(R, G, B));
    }
  }

  minutes = roundTo5(minutes);

  lightLine(ES);
  lightLine(ISCH);

  switch (minutes) {
    case 5:
      lightLine(FÜF);
      lightLine(AB);
      break;
    case 10:
      lightLine(ZÄÄ);
      lightLine(AB);
      break;
    case 15:
      lightLine(VIERTU);
      lightLine(AB);
      break;
    case 20:
      lightLine(ZWÄNZG);
      lightLine(AB);
      break;
    case 25:
      lightLine(FÜF);
      lightLine(VOR);
      lightLine(HAUBI);
      break;
    case 30:
      lightLine(HAUBI);
      hours++;
      break;
    case 35:
      lightLine(FÜF);
      lightLine(AB);
      lightLine(HAUBI);
      hours++;
      break;
    case 40:
      lightLine(ZWÄNZG);
      lightLine(VOR);
      hours++;
      break;
    case 45:
      lightLine(VIERTU);
      lightLine(VOR);
      hours++;
      break;
    case 50:
      lightLine(ZÄÄ);
      lightLine(VOR);
      hours++;
      break;
    case 55:
      lightLine(FÜF);
      lightLine(VOR);
      hours++;
      break;
  }

  if (hours == 12) {
    hours = 0;
  }

  switch (hours) {
    case 0:
      lightLine(ZWÖUFI);
      break;
    case 1:
      lightLine(EIS);
      break;
    case 2:
      lightLine(ZWÖI);
      break;
    case 3:
      lightLine(DRÜ);
      break;
    case 4:
      lightLine(VIERI);
      break;
    case 5:
      lightLine(FÜFI);
      break;
    case 6:
      lightLine(SÄCHSI);
      break;
    case 7:
      lightLine(SBINI);
      break;
    case 8:
      lightLine(ACHTI);
      break;
    case 9:
      lightLine(NÜNI);
      break;
    case 10:
      lightLine(ZÄNI);
      break;
    case 11:
      lightLine(EUFI);
      break;
  }

  strip.show();
}

void lightLine(const Line &line) {
  for (int i = line.begin; i <= line.end; i++) {
    lightLed(i, line.row);
  }
}

void lightLed(int x, int y) {
  strip.setBrightness(Brightness);
  int LEDnum = 0;

  if (x <= 11 && y <= 10) {
    if (x % 2 != 0) {
      LEDnum = (y + ((x - 1) * 10));
    } else {
      LEDnum = ((11 - y) + ((x - 1) * 10));
    }
    strip.setPixelColor(LEDnum - 1, strip.Color(R, G, B));
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  payload[length] = '\0'; // Null-terminate the payload
  String message = String((char *)payload);

  if (String(topic) == light_switch_topic) {
    handleLightSwitch(message);
  } else if (String(topic) == brightness_set_topic) {
    handleBrightnessSet(message);
  } else if (String(topic) == rgb_set_topic) {
    handleRGBSet(message);
  }

  timeClient.update();
  unsigned long epochTime = timeClient.getEpochTime();

  int hours = ((epochTime % 86400L) / 3600) + 1;
  int minutes = (epochTime % 3600) / 60;


  if (hours >= 12) {
    hours -= 12;
  }

  updateTime(hours, minutes);
}

void handleLightSwitch(String message) {
  if (message == "ON") {
    // If turning ON, set brightness to the last known brightness
    Brightness = lastBrightness;
    client.publish(brightness_status_topic, String(Brightness).c_str(), true);
    client.publish(light_status_topic, "ON", true);
  } else if (message == "OFF") {
    // If turning OFF, store the current brightness and set it to 0
    lastBrightness = Brightness;
    Brightness = 0;
    client.publish(brightness_status_topic, String(Brightness).c_str(), true);
    client.publish(light_status_topic, "OFF", true);
  }
}

void handleBrightnessSet(String message) {
  int brightnessValue = message.toInt();
  if (brightnessValue >= 0 && brightnessValue <= 255) {
    Brightness = brightnessValue;
    lastBrightness = Brightness;  

    StaticJsonDocument<200> jsonDocument;
    jsonDocument["brightness"] = Brightness;

    String jsonString;
    serializeJson(jsonDocument, jsonString);

    client.publish(brightness_status_topic, jsonString.c_str(), true);
  }
}

void handleRGBSet(String message) {
  int comma1 = message.indexOf(',');
  int comma2 = message.indexOf(',', comma1 + 1);

  if (comma1 != -1 && comma2 != -1) {
    int rValue = message.substring(0, comma1).toInt();
    int gValue = message.substring(comma1 + 1, comma2).toInt();
    int bValue = message.substring(comma2 + 1).toInt();

    R = rValue;
    G = gValue;
    B = bValue;

    // Create a JSON object
    StaticJsonDocument<200> jsonDocument;
    jsonDocument["rgb"][0] = R;
    jsonDocument["rgb"][1] = G;
    jsonDocument["rgb"][2] = B;

    // Serialize the JSON object to a string
    String jsonString;
    serializeJson(jsonDocument, jsonString);

    // Publish the JSON string to the MQTT topic
    client.publish(rgb_status_topic, jsonString.c_str(), true);
  }
}