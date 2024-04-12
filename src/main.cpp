#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "DHT.h"
#include "SPIFFS.h"
#include <BearSSLHelpers.h>
#include <CertStoreBearSSL.h>
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
    if (client.connect("koikoikoi", brokerUser, brokerPass)) {
      Serial.print("\nConnected to ");
      Serial.println(broker);
      client.subscribe("8aiswz6279/publisher");
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
    client.publish("8aiswz6279/led", msg);
  }

  if ((char)payload[0] == 'l') {
    digitalWrite(LED, LOW);
    snprintf(msg, MSG_BUFFER_SIZE, "O LED está apagado");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("8aiswz6279/led", msg);
  }
}

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(SENSOR, INPUT);

  if (!SPIFFS.begin(true)) {
    Serial.println("Erro ao montar sistema de arquivos SPIFFS!");
    return;
  }

  File caFile = SPIFFS.open("/emqxsl-ca.crt", "r");
  if (!caFile) {
    Serial.println("Erro ao abrir arquivo de certificado CA!");
    return;
  }

  size_t size = caFile.size();
  uint8_t certBuf[size];
  size_t bytesRead = caFile.read(certBuf, size);
  if (bytesRead != size) {
    Serial.println("Erro ao ler o arquivo de certificado CA!");
    return;
  }

  espClient.setTrustAnchors(new CertStoreBearSSL(certBuf, size));

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
    client.publish("8aiswz6279/porta", msg);
  } else {
    Serial.println("Sensor de Porta Aberto");
    snprintf(msg, MSG_BUFFER_SIZE, "Porta Aberta");
    client.publish("8aiswz6279/porta", msg);
  }

  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println(F("C"));
  snprintf(msg, MSG_BUFFER_SIZE, "%f", t);
  client.publish("8aiswz6279/temperatura", msg);

  delay(3000);

  if (!client.connected())
    reconnect();
  client.loop();
}


// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}