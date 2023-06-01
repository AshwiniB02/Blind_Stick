const int top_trig = 2;      // D4
const int top_echo = 14;     // D5
const int bottom_trig = 12;  // D6
const int bottom_echo = 13;  // D7

const int buzzer = 4;  // D2

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

void alarm(int num) {
  for (int i = 0; i < num; i++) {
    tone(buzzer, 200, 10);
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
  //Serial.println(n);
  if (n > 0) {
    alarm(n);
  }
  delay(1000);
}