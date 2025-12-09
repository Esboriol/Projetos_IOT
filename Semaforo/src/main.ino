#include <Arduino.h>

const int VERDE_CARRO = 23;
const int AMARELO = 22;
const int VERMELHO_CARRO = 21;
const int VERDE_PEDESTRE = 19;
const int VERMELHO_PEDESTRE = 18;

int contador = 0;

void setup() {
  pinMode(VERDE_CARRO, OUTPUT);
  pinMode(AMARELO, OUTPUT);
  pinMode(VERMELHO_CARRO, OUTPUT);
  pinMode(VERDE_PEDESTRE, OUTPUT);
  pinMode(VERMELHO_PEDESTRE, OUTPUT);
}

void loop() {
  digitalWrite(VERDE_CARRO, HIGH);
  digitalWrite(AMARELO, LOW);
  digitalWrite(VERMELHO_CARRO, LOW);
  digitalWrite(VERMELHO_PEDESTRE, HIGH);
  digitalWrite(VERDE_PEDESTRE, LOW);
  delay(5000); 
  digitalWrite(VERDE_CARRO, LOW);
  digitalWrite(AMARELO, HIGH);
  delay(2000); 
  digitalWrite(AMARELO, LOW);
  digitalWrite(VERMELHO_CARRO, HIGH);
  digitalWrite(VERMELHO_PEDESTRE, LOW);
  digitalWrite(VERDE_PEDESTRE, HIGH);
  delay(5000); 
}


// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}