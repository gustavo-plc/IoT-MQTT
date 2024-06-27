#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <Wire.h>
#include <Keypad.h>
#include <WiFiClientSecure.h>
#include <EEPROM.h>
#include <ESP32Servo.h>

#define LED 32
#define LED_STATE_ADDRESS 0
#define Password_Length 8

const char *ssid = "Gustavo's Galaxy M22";
const char *pass = "trovao07";
const char *brokerUser = "gustavoparreira";
const char *brokerPass = "trovao07";
const char *broker = "r6062f16.ala.us-east-1.emqxsl.com";
bool lastLedState = false;

WiFiClientSecure espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

Servo servo1;
Servo servo2;

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

int signalPin = 12;
int wrongpass = 13;
int rainOut = 27;
int rainIn = 34;
unsigned long previousMillis = 0;
const long interval = 100; // Reduzir o intervalo para 100ms

char Data[Password_Length]; 
char Master[Password_Length] = "123A456"; 
byte data_count = 0, master_count = 0;
bool Pass_is_good;
char customKey;
const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'}, 
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {19, 18, 5, 17};
byte colPins[COLS] = {16, 4, 0, 2};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

void saveLedState(bool state) {
  EEPROM.put(LED_STATE_ADDRESS, state);
  EEPROM.commit();
}

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
      delay(500);
    }
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  for (int i = 0; i < length; i++)
    Serial.print((char)payload[i]);
  Serial.println();

  if ((char)payload[0] == 'L') {
    digitalWrite(LED, HIGH);
    saveLedState(true);
    snprintf(msg, MSG_BUFFER_SIZE, "A lâmpada está acesa!\n");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("iot/lampada", msg);
  }

  if ((char)payload[0] == 'D') {
    digitalWrite(LED, LOW);
    saveLedState(false);
    snprintf(msg, MSG_BUFFER_SIZE, "A lâmpada está apagada!\n");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("iot/lampada", msg);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  pinMode(signalPin, OUTPUT);
  pinMode(wrongpass, OUTPUT);
  pinMode(rainOut, OUTPUT);
  pinMode(rainIn, INPUT); 
  
  servo1.attach(rainOut);
  servo1.write(0); //ângulo inicial do servo da janela (condição de não chuva)
  servo2.attach(signalPin);
  servo2.write(90); //ângulo mínimo de abertura do servo da porta (condição de porta fechada)

  espClient.setCACert(root_ca);
  client.setServer(broker, 8883);
  client.setCallback(callback);
 
  EEPROM.begin(sizeof(bool));
  EEPROM.get(LED_STATE_ADDRESS, lastLedState);
  digitalWrite(LED, lastLedState);
  setupWiFi();
}

void clearData() {
  while(data_count != 0) {
    Data[data_count--] = 0; 
  }
}

void tryToConnect() {
  if (!client.connected()) {
    if (WiFi.status() == WL_CONNECTED) {
      reconnect();
    } else {
      Serial.println("WiFi não conectada, tentando reconectar...");
      setupWiFi();
    }
  }
}

void loop() {
  static int c = 0;
  static int nc = 0;
  static unsigned long prevMillisPassword = 0;
  static unsigned long prevMillisRain = 0;
  static unsigned long servoOpenMillis = 0;
  static bool isServo2Open = false;
  static bool wrongPassLedOn = false;
  
  unsigned long currentMillis = millis();

  tryToConnect();
  client.loop();

  char customKey = customKeypad.getKey();
  if (customKey) {
    Data[data_count++] = customKey;
    if (data_count == Password_Length - 1) {
      Data[data_count] = '\0';

      if (strcmp(Data, Master) == 0) {
        servo2.write(170); //ângulo max de abertura do servo da porta (condição de porta aberta)
        isServo2Open = true;
        servoOpenMillis = currentMillis;
        Serial.println("Senha correta! Servo acionado.");
      } else {
        digitalWrite(wrongpass, HIGH);
        wrongPassLedOn = true;
        prevMillisPassword = currentMillis;
        Serial.println("Senha incorreta!");
      }
      clearData();
    }
  }

  if (isServo2Open && (currentMillis - servoOpenMillis >= 7000)) {
    servo2.write(90); //ângulo min de abertura do servo da porta (condição de porta fechada)
    isServo2Open = false;
  }

  if (wrongPassLedOn && (currentMillis - prevMillisPassword >= 5000)) {
    digitalWrite(wrongpass, LOW);
    wrongPassLedOn = false;
  }

  if (currentMillis - prevMillisRain >= interval) {
    prevMillisRain = currentMillis;
    int value = analogRead(rainIn);

    // Adiciona a impressão do valor lido pelo sensor de chuva
    Serial.print("Valor lido pelo sensor de chuva: ");
    Serial.println(value);

    
    if (value < 3400) {
      c++;
      nc = 0;
      if (c > 5) { // Reduzi o limite para aumentar a sensibilidade
        servo1.write(90); //regulagem do ângulo de abertura da janela quando chuva
        Serial.println("Chuva detectada!");
      }
    } else {
      nc++;
      if (nc > 5) {
        servo1.write(0); //regulagem do ângulo de abertura da janela quando não chuva
      }
      c = 0;
    }
    
// Adiciona a impressão das posições dos servos
    Serial.print("Posição do servo1: ");
    Serial.println(servo1.read());
    Serial.print("Posição do servo2: ");
    Serial.println(servo2.read());


    // Adiciona a impressão dos valores digitados no keypad
    Serial.print("Tecla digitada: ");
    Serial.println(customKey);
  }
}
