const int top_trig = 2;      // D4
const int top_echo = 14;     // D5
const int bottom_trig = 12;  // D6
const int bottom_echo = 13;  // D7

void setup() {
  Serial.begin(9600);

  pinMode(top_trig, OUTPUT);
  pinMode(top_echo, INPUT);
  pinMode(bottom_trig, OUTPUT);
  pinMode(bottom_echo, INPUT);
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

void loop() {
  float top=readTopSensor();
  delay(200);
  float bottom=readBottomSensor();
  delay(200);
  Serial.print("Distance: top ");
  Serial.print(top);
  Serial.print(" Distance : bottom ");
  Serial.println(bottom);
}