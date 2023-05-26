// #include <Ultrasonic.h>

// OOP => Object Oriented Programming
// class object => Ultrasonic
// instances => make two instances
// Ultrasonic ultrasonic_top(4, 5);
// Ultrasonic ultrasonic_bottom(6, 7);

void setup() {
  Serial.begin(9600);
}

void loop() {
  Serial.print("Sensor top: ");
  // Serial.print(ultrasonic_top.read()); // Prints the distance on the default unit (centimeters)
  Serial.println("cm");

  Serial.print("Sensor bottom: ");
  // Serial.print(ultrasonic_bottom.read(CM)); // Prints the distance making the unit explicit
  Serial.println("cm");

  delay(200);
}