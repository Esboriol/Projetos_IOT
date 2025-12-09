#include <WiFi.h>
#include <HTTPClient.h>
#include <DHTesp.h>

const char* ssid = "AndroidAP0C94";
const char* pass = "paodequeijo";

const char* serverUrl = "https://dht-senai.onrender.com/api/readings";

DHTesp dht;
const int DHT_PIN = 5; 

void setup(){
  Serial.begin(115200);
  dht.setup(DHT_PIN, DHTesp::DHT11); 
  WiFi.begin(ssid, pass);
  Serial.print("Conectando WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");
}

void loop(){
  float humidity = dht.getHumidity();
  float temp = dht.getTemperature(); 
  if (isnan(humidity) || isnan(temp)) {
    Serial.println("Falha ao ler DHT");
  } else {
    Serial.printf("Temp: %.2f C, Hum: %.2f %%\n", temp, humidity);

    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/json");
      String payload = "{";
      payload += "\"temperature\": " + String(temp, 2) + ",";
      payload += "\"humidity\": " + String(humidity, 2) + ",";
      payload += "\"timestamp\": \"" + String((unsigned long)time(nullptr)) + "\"";
      payload += "}";
      int code = http.POST(payload);
      String resp = http.getString();
      Serial.printf("POST %d -> %s\n", code, resp.c_str());
      http.end();
    }
  }

  delay(30000);
}