#include <Arduino.h>
#include "secrets.h"
#include "WiFi.h"

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;

const int port = 10000;
WiFiServer server(port);

const int LED_PIN = 21;  // LED built-in sur la plupart des ESP32
bool ledState = 0;

void handleTCPClient() {
  WiFiClient client = server.available();
  uint8_t data[30];
  if (client)
  {
    
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);


    Serial.println("New client from IP address: " + client.remoteIP().toString() + ", port: " + String(client.remotePort()));
    /* check client is connected */
    while (client.connected())
    {
      

      if (client.available())
      {
        int len = client.read(data, 30);
        if (len < 30)
        {
          data[len] = '\0';
        }
        else
        {
          data[30] = '\0';
        }
        Serial.print("client sent: ");
        Serial.println((char *)data);
      }
    }
  }
}

String translateEncryptionType(wifi_auth_mode_t encryptionType)
{

  switch (encryptionType)
  {
  case (WIFI_AUTH_OPEN):
    return "Open";
  case (WIFI_AUTH_WEP):
    return "WEP";
  case (WIFI_AUTH_WPA_PSK):
    return "WPA_PSK";
  case (WIFI_AUTH_WPA2_PSK):
    return "WPA2_PSK";
  case (WIFI_AUTH_WPA_WPA2_PSK):
    return "WPA_WPA2_PSK";
  case (WIFI_AUTH_WPA2_ENTERPRISE):
    return "WPA2_ENTERPRISE";
  default:  
    return "UNKNOWN";
  }
  return "UNKNOWN";
}

void scanNetworks()
{

  int numberOfNetworks = WiFi.scanNetworks();

  // Add delay so the terminal can catch up
  delay(3000);

  Serial.print("Number of networks found: ");
  Serial.println(numberOfNetworks);

  for (int i = 0; i < numberOfNetworks; i++)
  {

    Serial.print("Network name: ");
    Serial.println(WiFi.SSID(i));

    Serial.print("Signal strength: ");
    Serial.println(WiFi.RSSI(i));

    Serial.print("MAC address: ");
    Serial.println(WiFi.BSSIDstr(i));

    Serial.print("Encryption type: ");
    String encryptionTypeDescription = translateEncryptionType(WiFi.encryptionType(i));
    Serial.println(encryptionTypeDescription);
    Serial.println("-----------------------");
  }
}

void connectToNetwork()
{
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }

  Serial.println("Connected to network");
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

  // Print MAC address
  Serial.println("MCU MAC address: " + WiFi.macAddress());

  scanNetworks();
  connectToNetwork();

  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  // Votre code principal ici
  handleTCPClient();
}
