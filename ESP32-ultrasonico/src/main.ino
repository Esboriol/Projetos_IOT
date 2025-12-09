#include <Arduino.h>
#include <HCSR04.h>

const int PINO_TRIG = 22;
const int PINO_ECHO = 23;

HCSR04 hc(PINO_TRIG, PINO_ECHO);

void setup() {
  Serial.begin(115200);
  pinMode(PINO_TRIG, OUTPUT);
  pinMode(PINO_ECHO, INPUT);
}

void loop() {
  float dinstanciaCM = hc.dist();
  Serial.println(dinstanciaCM);
  delay(500);
}