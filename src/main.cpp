//carregamento das bibliotecas a serem utilizadas:
#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <time.h>
#include "DHT.h"

// //nomeação dos pinos do hardware ESP32
#define O_IRRIGA 32 //variável de saída fuzzy: fechamento do relé e acionamento da irrigação. Pino 32
// #define I_TEMPUMI 4 //variável de entrada 1 fuzzy: sensor de temperatura/umidade: um só sensor fornecerá dados para duas variáveis de entrada: temperatura e umidade. Pino 4
// #define I_UMISOLO 34 //variável de entrada 2 fuzzy: sensor de umidade do solo. Pino 34
// #define DHTTYPE DHT11

// informações da rede wi-fi
const char *ssid = "Gustavo's Galaxy M22";
const char *pass = "trovao07";
const char *brokerUser = "8aiswz6279";
const char *brokerPass = "678gimorst";
const char *broker = "b37.mqtt.one";

WiFiClient espClient;
PubSubClient client(espClient);

DHT dht(I_TEMPUMI, DHTTYPE);   //inicializando o sensor. 

// funções de configuração
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
    } else {
      Serial.println("\nTrying to connect again!");
      delay(5000);
    }
  }
}

void setup() {
  // //CONFIGURAÇÃO DE CADA PINO DEFINIDO ANTERIORMENTE PARA FUNCIONAREM COMO ENTRADAS OU SAÍDAS
  // pinMode(O_IRRIGA,OUTPUT);
  // pinMode(I_TEMPUMI,INPUT);
  // pinMode(I_UMISOLO,INPUT);

  // // INICIALIZAÇÃO DA TRANSMISSÃO SERIAL NA TAXA DE 9600 bits por segundo
  // Serial.begin(9600);
  // Serial.println(F("Teste do DHT11!"));
  // dht.begin();

  Serial.begin(115200);
  setupWiFi();
  client.setServer(broker, 1883); //local para inserção da porta para conexão
}

void loop() {
  if (!client.connected())
    reconnect();
  client.loop();

  // digitalWrite(O_IRRIGA, LOW);
  // delay(2000);
  // //leitura de umidade do ar e temperatura do sensor DHT11
  // float umidadeAr = dht.readHumidity();
  // float temperatura = dht.readTemperature();

  // // Verificação de erros de leitura e informação via terminal
  // if (isnan(umidadeAr) || isnan(temperatura)) {
  //   Serial.println(F("Falha em ler dados do sensor!\n"));
  //   return;
  // }

}


// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}