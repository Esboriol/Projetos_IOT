#include <Arduino.h>

int led = 22;
int echoPin = 23;
int trigPin = 5;
int ledvermelho = 15;
int buzzer = 18;

#define SOUND_SPEED 0.034

long duration;
float distanciaCm;

void setup() {
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledvermelho, OUTPUT);
  pinMode(buzzer, OUTPUT);
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Mantem trigPin em estado HIGH por 10 microssegundos
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
   
  // Lê o echoPin, retorna o tempo de viagem da onda sonora em microssegundos
  duration = pulseIn(echoPin, HIGH);
   
  // Calcula a distância
  distanciaCm = duration * SOUND_SPEED/2;

  if (distanciaCm >= 30) {
    digitalWrite(ledvermelho, LOW);
    digitalWrite(led, HIGH);
    noTone(buzzer);
  } else {
    tone(buzzer, 10000);
    digitalWrite(ledvermelho, HIGH);
    digitalWrite(led, LOW);
  }
   
  // Imprime a distância no monitor serial
  Serial.print("Distancia (cm): ");
  Serial.println(distanciaCm);
  delay(500);
}
