//carregamento das bibliotecas a serem utilizadas:
#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include "DHT.h"

// //nomeação dos pinos do hardware ESP32
#define LED 32 //variável de saída: fechamento do relé e acionamento do LED. Pino 32
#define SENSOR 4 //variável de entrada 1: sensor de temperatura/umidade: um só sensor fornecerá dados para duas variáveis de entrada: temperatura e umidade. Pino 4
#define DHTTYPE DHT11

// informações da rede wi-fi
const char *ssid = "Gustavo's Galaxy M22";
const char *pass = "trovao07";
const char *brokerUser = "gustavoparreira";
const char *brokerPass = "trovao07";
const char *broker = "r6062f16.ala.us-east-1.emqxsl.com";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

dht DHT; // inicializando o sensor.


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
      client.subscribe("8aiswz6279/publisher"); //SE CONECTADO AO SERVIDOR MQTT, INSCREVE-SE A ESSE TÓPICO
    } else {
      Serial.println("\nTrying to connect again!");
      delay(5000);
    }
  }
}



void callback (char* topic, byte* payload, unsigned int length) 
{
  Serial.print ("Message arrived [");
  Serial.print (topic);
  Serial.print (" ");
  for (int i = 0; i < length; i++)
    Serial.print((char) payload[i]);
  Serial.println();

if ((char) payload [0] == 'L') 
{
  digitalWrite(LED, HIGH); //caso receba um L, altera a variável TESTE_SAIDA para HIGH
  snprintf (msg, MSG_BUFFER_SIZE, "O LED está aceso");
  Serial.print("Publica mensagem: ");
  Serial.println(msg);
  client.publish("8aiswz6279/led", msg);
}


if ((char) payload [0] == 'l') //caso receba um l(éle minúsculo), altera a variável TESTE_SAIDA para LOW
{
  digitalWrite(LED, LOW);
  snprintf (msg, MSG_BUFFER_SIZE, "O LED está apagado");
  Serial.print("Publica mensagem: ");
  Serial.println(msg);
  client.publish("8aiswz6279/led", msg);
}
}


void setup() {
  // //CONFIGURAÇÃO DE CADA PINO DEFINIDO ANTERIORMENTE PARA FUNCIONAREM COMO ENTRADAS OU SAÍDAS
  pinMode(LED,OUTPUT);
  pinMode(SENSOR,INPUT);
  
  Serial.begin(115200);
  setupWiFi();
  client.setServer(broker, 8883); //local para inserção da porta para conexão
  client.setCallback(callback);
}

void loop() {

  double t = DHT.temperature;
  double h = DHT.humidity;

  int sns = digitalRead(LED);
  if (sns == 1)
  {
    Serial.println("Sensor de Porta Fechado");
    snprintf(msg, MSG_BUFFER_SIZE, "Porta Fechada");
    client.publish("8aiswz6279/porta", msg);
} else {

    Serial.println("Sensor de Porta Aberto");
    snprintf (msg, MSG_BUFFER_SIZE, "Porta Aberta");
    client.publish("8aiswz6279/porta", msg);
}
    Serial.print ("Temperatura: ");
    Serial.print(t);
    Serial.println(F("C"));
    sprintf(msg, "%f", t);
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