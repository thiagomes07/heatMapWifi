#include "UbidotsEsp32Mqtt.h"

// Constantes de configuração

const char *WIFI_SSID = "AndroidAP"; // Coloque aqui o SSID do seu Wi-Fi
const char *WIFI_PASS = "onuz2204"; // Coloque aqui a senha do seu Wi-Fi

const char *UBIDOTS_TOKEN = "BBUS-YvRNja0v0CjGesChRsmWQBEJdlE61K"; 
const char *DEVICE_LABEL = "esp32_t13_iotrackers2"; 
const char *VARIABLE_LABEL = "wifi_signal"; // Variável para o RSSI (novo)
const char *CLIENT_ID = "batmen"; 

Ubidots ubidots(UBIDOTS_TOKEN, CLIENT_ID);

const int PUBLISH_FREQUENCY = 3000; // Update rate in milliseconds
unsigned long timer;
uint8_t pinLED = 2;
bool statusLED = false;

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Messagem recebida [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// Conexão WiFi, envio da mensagem ao Ubidots
void setup()
{
  // Configuração inicial
  Serial.begin(115200);
  ubidots.setDebug(true); // Ativa mensagens de depuração
  ubidots.connectToWifi(WIFI_SSID, WIFI_PASS);
  ubidots.setCallback(callback);
  ubidots.setup();
  ubidots.reconnect();
  pinMode(pinLED, OUTPUT);

  timer = millis();
}

void loop()
{
  // Reconecta ao MQTT, se necessário
  if (!ubidots.connected())
  {
    ubidots.reconnect();
  }

  // Publica os valores a cada X segundos
  if (millis() - timer > PUBLISH_FREQUENCY)
  {
    // Lê o RSSI
    int rssi = WiFi.RSSI(); // Obtém o valor do RSSI

    rssi = rssi + 90;

    Serial.print("RSSI: ");
    Serial.println(rssi);

    // Adiciona os valores às variáveis do Ubidots
    ubidots.add(VARIABLE_LABEL, rssi);   // RSSI (novo)

    // Publica as variáveis no Ubidots
    ubidots.publish(DEVICE_LABEL);

    // Atualiza o timer
    timer = millis();
  }

  // Processa pacotes MQTT
  ubidots.loop();
}