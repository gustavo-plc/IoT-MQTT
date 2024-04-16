#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "DHT.h"
#include <WiFiClientSecure.h>

#define LED 32
#define SENSOR 4
#define DHTPIN 2
#define DHTTYPE DHT11

const char *ssid = "Gustavo's Galaxy M22";
const char *pass = "trovao07";
const char *brokerUser = "gustavoparreira";
const char *brokerPass = "trovao07";
const char *broker = "r6062f16.ala.us-east-1.emqxsl.com";

WiFiClientSecure espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

dht DHT;

const char* root_ca = 
"-----BEGIN CERTIFICATE-----\n" \
"MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
"QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
"b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
"9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n" \
"CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n" \
"nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n" \
"43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n" \
"T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n" \
"gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n" \
"BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n" \
"TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n" \
"DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n" \
"hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n" \
"06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n" \
"PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n" \
"YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n" \
"CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n" \
"-----END CERTIFICATE-----\n";

void setupWiFi() {
  delay(1000);
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("-");
  }
  Serial.print("\nConnected to: ");
  Serial.print(ssid);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("\nConnecting to ");
    Serial.println(broker);
    if (client.connect("ESP32", brokerUser, brokerPass)) {
      Serial.print("\nConnected to ");
      Serial.println(broker);
      client.subscribe("iot/lampada");
      client.subscribe("iot/temperatura");
      client.subscribe("iot/porta");
    } else {
      Serial.println("\nTrying to connect again!");
      delay(5000);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
    Serial.print((char)payload[i]);
  Serial.println();

  if ((char)payload[0] == 'L') {
    digitalWrite(LED, HIGH);
    snprintf(msg, MSG_BUFFER_SIZE, "O LED está aceso");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("iot/led", msg);
  }

  if ((char)payload[0] == 'l') {
    digitalWrite(LED, LOW);
    snprintf(msg, MSG_BUFFER_SIZE, "O LED está apagado");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("iot/led", msg);
  }
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(SENSOR, INPUT);

  espClient.setCACert(root_ca);

  client.setServer(broker, 8883);
  client.setCallback(callback);

  Serial.begin(115200);
  setupWiFi();
}

void loop() {
  double t = DHT.temperature;
  double h = DHT.humidity;

  int sns = digitalRead(LED);
  if (sns == 1) {
    Serial.println("Sensor de Porta Fechado");
    snprintf(msg, MSG_BUFFER_SIZE, "Porta Fechada");
    client.publish("iot/porta", msg);
  } else {
    Serial.println("Sensor de Porta Aberto");
    snprintf(msg, MSG_BUFFER_SIZE, "Porta Aberta");
    client.publish("iot/porta", msg);
  }

  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println(F("C"));
  snprintf(msg, MSG_BUFFER_SIZE, "%f", t);
  client.publish("iot/temperatura", msg);

  delay(3000);

  if (!client.connected())
    reconnect();
  client.loop();
}
