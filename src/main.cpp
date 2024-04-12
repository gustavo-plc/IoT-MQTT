#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

// informações da rede wi-fi
const char *ssid = "Gustavo's Galaxy M22";
const char *pass = "trovao07";
const char *brokerUser = "8aiswz6279";
const char *brokerPass = "678gimorst";
const char *broker = "b37.mqtt.one";

WiFiClient espClient;
PubSubClient client(espClient);

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
  Serial.begin(115200);
  setupWiFi();
  client.setServer(broker, 1883); // Try with 1883 if TLS is not used
}

void loop() {
  if (!client.connected())
    reconnect();
  client.loop();
}


// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}