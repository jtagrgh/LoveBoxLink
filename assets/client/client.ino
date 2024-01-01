#include "WiFiS3.h"
#include "secrets.h"

int status = WL_IDLE_STATUS;
WiFiClient client;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed.");
    while (1);
  }

  if (WiFi.firmwareVersion() < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Upgrade firmware.");
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting connection to SSID: ");
    Serial.println(SECRET_SSID);
    status = WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(10000);
  }

  Serial.println("Client setup done.");
}

void loop() {
  // put your main code here, to run repeatedly:
}