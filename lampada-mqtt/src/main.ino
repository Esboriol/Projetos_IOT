#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "AndroidAP0C94";
const char* password = "paodequeijo";

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

const int lampadaSala = 23;
const int lampadaCozinha = 22;

void setup_wifi() {
  delay(10);
  Serial.println("Conectando ao WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado, IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println(topic);
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  if (String(topic) == "br2/lampada/sala/ligar") {
    Serial.println("Ligando lâmpada da sala");
    digitalWrite(lampadaSala, HIGH);  
  }
  else if (String(topic) == "br2/lampada/sala/desligar") {
    Serial.println("Desligando lâmpada da sala");
    digitalWrite(lampadaSala, LOW);   
  }

  if (String(topic) == "br2/lampada/cozinha/ligar") {
    Serial.println("Ligando lâmpada da cozinha");
    digitalWrite(lampadaCozinha, HIGH);
  }
  else if (String(topic) == "br2/lampada/cozinha/desligar") {
    Serial.println("Desligando lâmpada da cozinha");
    digitalWrite(lampadaCozinha, LOW);
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    String clientId = "ESP32-" + String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");

      client.subscribe("br2/lampada/sala/ligar");
      client.subscribe("br2/lampada/sala/desligar");
      client.subscribe("br2/lampada/cozinha/ligar");
      client.subscribe("br2/lampada/cozinha/desligar");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5s...");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(lampadaSala, OUTPUT);
  pinMode(lampadaCozinha, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 
}