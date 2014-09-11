/*
  SerialLogger
  Turn an LED on, and log the value of a photoresistor every 2 milliseconds.
 
  This example code is in the public domain.
*/

const int analogInPin = A0;  // Analog input pin that the photoresistor is connected to
const int led = 13;   // Blue wire is positive

int sensorValue = 0;    // Value read from the photoresistor

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  Serial.begin(9600); 
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  // read the analog in value:
  sensorValue = analogRead(analogInPin);

  // print the results to the serial monitor:
  Serial.print("pr = " );
  Serial.print(sensorValue);

  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(2);
}