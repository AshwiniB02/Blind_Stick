#include <Gyver433.h>
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>

#include "webpage.h"

#define G433_SLOW

const int top_trig = 2;      // D4
const int top_echo = 14;     // D5
const int bottom_trig = 12;  // D6
const int bottom_echo = 13;  // D7
const int rftx_pin = 15;     // D8 (rftx pin->data pin of transmitter)

const int buzzer = 4;  // D2

const int button = 5;  //D1
const int LED = 3;     //RX
char data[] = "ALERT";

const char* ssid = "redmi 10";
const char* password = "ashu12345";
const int webSocketPort = 8080;

WebSocketsServer webSocket = WebSocketsServer(webSocketPort);
ESP8266WebServer server(80);

Gyver433_TX<rftx_pin> tx;


// WebSocket event handler
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  Serial.println("event");
  if (type == WStype_TEXT) {
    Serial.print("Received data: ");
    Serial.println((char*)payload);

    // send it to receiver
    tx.sendData(payload);

    // Process the received data here
    // You can parse the JSON and extract the latitude and longitude values

    // For example, assuming the payload is in the format {"latitude": 123, "longitude": 456}
    // You can use a JSON library like ArduinoJson to parse the payload
    // ArduinoJson library: https://arduinojson.org/
    // Include the library and add the necessary parsing code

    // JSON parsing example:
    // DynamicJsonDocument doc(256);
    // deserializeJson(doc, (char*)payload);
    // int latitude = doc["latitude"];
    // int longitude = doc["longitude"];

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

  Serial.println("");
  Serial.print("Connected to Wi-Fi. IP address: ");
  Serial.println(WiFi.localIP());
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

  connectToWiFi();
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
    // tone(buzzer, 1200, 30);
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
  // tx.sendData(data);
  webSocket.loop();
  server.handleClient();

  if (digitalRead(button)) {
    digitalWrite(LED, HIGH);
    tx.sendData(data);
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