#include <RH_ASK.h>
#ifdef RH_HAVE_HARDWARE_SPI
#include <SPI.h>  // Not actually used but needed to compile
#endif #include < ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>

#include "webpage.h"

const int top_trig = 2;      // D4
const int top_echo = 14;     // D5
const int bottom_trig = 12;  // D6
const int bottom_echo = 13;  // D7
const int rftx_pin = 15;     // D8 (rftx pin->data pin of transmitter)

const int buzzer = 4;  // D2

const int button = 5;  //D1
const int LED = 3;     //RX

const char* ssid = "pooja 2";
const char* password = "pooja12345";
const int webSocketPort = 8080;

String my_ip;

WebSocketsServer webSocket = WebSocketsServer(webSocketPort);
ESP8266WebServer server(80);

// RH_ASK driver;
RH_ASK driver(2000, 16, 15, 0);  // ESP8266 or ESP32: do not use pin 11 or 2

// WebSocket event handler
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  Serial.println("event");
  if (type == WStype_TEXT) {
    Serial.print("Received data: ");
    Serial.println((char*)payload);

    // send it to receiver
    // char loc[] = "loc";
    // driver.send((uint8_t *)loc, 3);
    // driver.waitPacketSent();

    // Process the received data here
    // You can parse the JSON and extract the latitude and longitude values

    // For example, assuming the payload is in the format {"latitude": 123, "longitude": 456}
    // You can use a JSON library like ArduinoJson to parse the payload
    // ArduinoJson library: https://arduinojson.org/
    // Include the library and add the necessary parsing code

    // JSON parsing example:
    DynamicJsonDocument doc(256);
    deserializeJson(doc, payload);
    float latitude = doc["latitude"];
    float longitude = doc["longitude"];

    // send to receiver
    driver.send((uint8_t*)&latitude, 4);
    driver.waitPacketSent();
    driver.send((uint8_t*)&longitude, 4);
    driver.waitPacketSent();

    // debug
    Serial.println(latitude, 8);
    Serial.println(longitude, 8);

    // Do something with the latitude and longitude values
  }
}

// Connect to Wi-Fi
void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  my_ip = WiFi.localIP().toString();
  Serial.println("");
  Serial.print("Connected to Wi-Fi. IP address: ");
  Serial.println(my_ip);
}

void send_IP() {
  // send it to receiver
  driver.send((uint8_t*)my_ip.substring(7).c_str(), my_ip.substring(7).length());
  driver.waitPacketSent();
}

void handleRoot() {
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  pinMode(top_trig, OUTPUT);
  pinMode(top_echo, INPUT);
  pinMode(bottom_trig, OUTPUT);
  pinMode(bottom_echo, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(button, INPUT);

  if (!driver.init())
#ifdef RH_HAVE_SERIAL
    Serial.println("init failed");
#endif

  connectToWiFi();
  delay(100);
  send_IP();
  delay(100);
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  server.on("/", handleRoot);
  server.begin();
}

float readTopSensor() {
  digitalWrite(top_trig, LOW);
  delayMicroseconds(2);

  digitalWrite(top_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(top_trig, LOW);

  long duration = pulseIn(top_echo, HIGH);
  float distance = duration / 58.2;

  /*Serial.print("top distance: ");
  Serial.print(distance);
  Serial.println("cm");*/

  return distance;
}

float readBottomSensor() {
  digitalWrite(bottom_trig, LOW);
  delayMicroseconds(2);

  digitalWrite(bottom_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(bottom_trig, LOW);

  long duration = pulseIn(bottom_echo, HIGH);
  float distance = duration / 58.2;

  /*Serial.print("distance bottom: ");
  Serial.print(distance);
  Serial.println("cm");*/

  return distance;
}

void alarm(int num) {
  for (int i = 0; i < num; i++) {
    tone(buzzer, 1200, 30);
    delay(100);
  }
}

int getnum(float top, float bottom) {
  if (top < 10 || bottom < 10) {
    return 4;
  }
  if (top < 20 || bottom < 20) {
    return 3;
  }
  if (top < 30 || bottom < 30) {
    return 2;
  }
  if (top < 40 || bottom < 40) {
    return 1;
  }
  return 0;
}

void emergency() {
  webSocket.loop();
  server.handleClient();

  if (digitalRead(button)) {
    digitalWrite(LED, HIGH);
    uint8_t alert[6] = "ALERT";
    driver.send((uint8_t*)alert, 5);
    driver.waitPacketSent();
    webSocket.broadcastTXT("Alert");
    delay(500);
  } else {
    digitalWrite(LED, LOW);
  }
}

void loop() {
  float top = readTopSensor();
  delay(10);
  float bottom = readBottomSensor();
  delay(10);
  /*Serial.print("Distance: top ");
  Serial.print(top);
  Serial.print(" Distance : bottom ");
  Serial.println(bottom);*/

  int n = getnum(top, bottom);
  // Serial.println(n);
  if (n > 0) {
    alarm(n);
    // split big delay into small
    // to enable quick response on button
    for (int i = 0; i < 10; i++) {
      emergency();
      delay(100);
    }
  }
  emergency();
}