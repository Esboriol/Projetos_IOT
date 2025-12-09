// main.cpp - ESP32 MQTT (PubSubClient) + semáforo sem delay()
#include <WiFi.h>
#include <PubSubClient.h>

// ====== CONFIGURE AQUI ======
const char* ssid = "AndroidAP0C94";
const char* password = "paodequeijo";

const char* mqtt_server = "broker.hivemq.com"; // IP ou domínio do broker
const uint16_t mqtt_port = 1883;                // 1883 (tcp) ou 8883 (tls)
const char* mqtt_user = "";          // ou NULL
const char* mqtt_pass = "";       // ou NULL
// ============================

WiFiClient espClient;       // use WiFiClientSecure se for TLS
PubSubClient mqtt(espClient);

// Tópicos
const char* TOPIC_CTRL   = "semaforo/controle"; // receber 'start' / 'stop'
const char* TOPIC_STATUS = "semaforo/status";   // publica estado: "running"/"stopped"

// Pinos do semáforo
const int VERDE_CARRO = 23;
const int AMARELO = 22;
const int VERMELHO_CARRO = 21;
const int VERDE_PEDESTRE = 19;
const int VERMELHO_PEDESTRE = 18;

// Tempos (ms)
const unsigned long DURA_VERDE = 4000;
const unsigned long DURA_AMARELO = 4000;
const unsigned long DURA_VERMELHO = 4000;

// Estado do semáforo
bool running = false;
unsigned long lastChange = 0;
int state = 0; // 0 idle, 1 verde carro, 2 amarelo, 3 vermelho carro / verde pedestre

// Helpers
void publishStatus(const char* status, bool retained=true) {
  mqtt.publish(TOPIC_STATUS, status, retained);
}

// Callback MQTT (quando recebe mensagem)
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  msg.trim();
  Serial.printf("MQTT Received on %s: %s\n", topic, msg.c_str());

  if (String(topic) == TOPIC_CTRL) {
    if (msg.equalsIgnoreCase("start")) {
      if (!running) {
        running = true;
        state = 1;
        lastChange = millis();
        // set initial lights
        digitalWrite(VERDE_CARRO, HIGH);
        digitalWrite(AMARELO, LOW);
        digitalWrite(VERMELHO_CARRO, LOW);
        digitalWrite(VERMELHO_PEDESTRE, HIGH);
        digitalWrite(VERDE_PEDESTRE, LOW);
        publishStatus("running");
      }
    } else if (msg.equalsIgnoreCase("stop")) {
      running = false;
      state = 0;
      // apagar leds
      digitalWrite(VERDE_CARRO, LOW);
      digitalWrite(AMARELO, LOW);
      digitalWrite(VERMELHO_CARRO, LOW);
      digitalWrite(VERMELHO_PEDESTRE, LOW);
      digitalWrite(VERDE_PEDESTRE, LOW);
      publishStatus("stopped");
    }
  }
}

void connectWiFi() {
  Serial.printf("Conectando WiFi %s ...\n", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi conectado. IP: ");
  Serial.println(WiFi.localIP());
}

void connectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    // unique client id
    String clientId = "ESP32-Semaforo-";
    clientId += String((uint32_t)ESP.getEfuseMac(), HEX);

    // se quiser usar Last Will: aqui usamos a versão connect com will
    if (mqtt.connect(clientId.c_str(), mqtt_user, mqtt_pass, TOPIC_STATUS, 1, true, "offline")) {
      Serial.println("conectado");
      // subscribes
      mqtt.subscribe(TOPIC_CTRL);
      // publica status retained (ao conectar)
      publishStatus(running ? "running" : "stopped", true);
    } else {
      Serial.print("falhou, rc=");
      Serial.print(mqtt.state());
      Serial.println(" tentando de novo em 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // pinos
  pinMode(VERDE_CARRO, OUTPUT);
  pinMode(AMARELO, OUTPUT);
  pinMode(VERMELHO_CARRO, OUTPUT);
  pinMode(VERDE_PEDESTRE, OUTPUT);
  pinMode(VERMELHO_PEDESTRE, OUTPUT);

  // desligar todos inicialmente
  digitalWrite(VERDE_CARRO, LOW);
  digitalWrite(AMARELO, LOW);
  digitalWrite(VERMELHO_CARRO, LOW);
  digitalWrite(VERDE_PEDESTRE, LOW);
  digitalWrite(VERMELHO_PEDESTRE, LOW);

  connectWiFi();

  mqtt.setServer(mqtt_server, mqtt_port);
  mqtt.setCallback(mqttCallback);

  connectMQTT();

  Serial.println("Setup completo.");
}

void loop() {
  if (!mqtt.connected()) {
    connectMQTT();
  }
  mqtt.loop();

  if (!running) return;

  unsigned long now = millis();
  switch (state) {
    case 1: // verde carro
      if (now - lastChange >= DURA_VERDE) {
        // vai para amarelo
        digitalWrite(VERDE_CARRO, LOW);
        digitalWrite(AMARELO, HIGH);
        lastChange = now;
        state = 2;
      }
      break;
    case 2: // amarelo
      if (now - lastChange >= DURA_AMARELO) {
        // vai para vermelho carro / verde pedestre
        digitalWrite(AMARELO, LOW);
        digitalWrite(VERMELHO_CARRO, HIGH);
        digitalWrite(VERMELHO_PEDESTRE, LOW);
        digitalWrite(VERDE_PEDESTRE, HIGH);
        lastChange = now;
        state = 3;
      }
      break;
    case 3: // vermelho carro / verde pedestre
      if (now - lastChange >= DURA_VERMELHO) {
        // volta ao verde carro
        digitalWrite(VERDE_PEDESTRE, LOW);
        digitalWrite(VERMELHO_PEDESTRE, HIGH);
        digitalWrite(VERMELHO_CARRO, LOW);
        digitalWrite(VERDE_CARRO, HIGH);
        lastChange = now;
        state = 1;
      }
      break;
    default:
      break;
  }
}