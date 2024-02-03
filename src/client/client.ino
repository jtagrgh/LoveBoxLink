#include "WiFiS3.h"
#include "secrets.h"
#include <Adafruit_ILI9341.h>
#include <Adafruit_TSC2007.h>
#include <Adafruit_GFX.h>

#include "shared.h"

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MIN_PRESSURE 200

Adafruit_TSC2007 ts; /* This is the screen? */
Adafruit_ILI9341 tft = Adafruit_ILI9341(10, 9); /* This is the controller of the screen? */
int oldColour, currentColour;
WiFiClient client;

void setup() {
  // Serial setup
  Serial.begin(9600);
  while (!Serial);

  // WiFi module setup
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed. Stopping execution.");
    while (1);
  }
  
  // WiFi network setup
  Serial.print("Attempting to connect to network SSID:");
  Serial.println(SECRET_SSID);
  while (WiFi.begin(SECRET_SSID, SECRET_PASS) != WL_CONNECTED) {
    Serial.println("Failed to connect. Waiting 10s and trying again");
    delay(10000);
  }

  // Touch screen setup
  tft.begin();
  if (!ts.begin()) {
    Serial.println("Communication with touchscreen controller failed. Stopping execution");
    while (1);
  }
  tft.fillScreen(ILI9341_BLACK);
  currentColour = ILI9341_RED;

  // Connect to server
  Serial.println("Attempting to connect to server.");
  if (!client.connect(IPAddress(192,168,1,198), 5341)) {
    Serial.println("Failed to connect. Stopping execution");
    while (1);
  }

  Serial.println("Client setup done.");
}

void loop() {
  uint16_t x, y, z1, z2;
  char debug[100];

  if (ts.read_touch(&x, &y, &z1, &z2) && z1 > TS_MIN_PRESSURE) {
    snprintf(debug, 100, "sent x %d, y %d, z1 %d, z2 %d", x, y, z1, z2);
    Serial.println(debug);

    struct Message message{x, y, currentColour};
    snprintf(debug, 100, "%X %X %X", message.x, message.y, message.colour);
    Serial.println(debug);

    const int screenX = map(x, screenMinX, screenMaxX, 0, tft.width());
    const int screenY = map(y, screenMinY, screenMaxY, 0, tft.height());

    tft.fillRect(screenX, screenY, 3, 3, currentColour);

    client.write((uint8_t*)&message, messageSize);
  }

  if (client.available() >= messageSize) { 
    struct Message recvMessageBuf;
    client.read((uint8_t*)&recvMessageBuf, messageSize);

    snprintf(debug, 50, "recv x %d y %d", recvMessageBuf.x, recvMessageBuf.y);
    Serial.println(debug);

    const int recvScreenX = map(recvMessageBuf.x, screenMinX, screenMaxX, 0, tft.width());
    const int recvScreenY = map(recvMessageBuf.y, screenMinY, screenMaxY, 0, tft.height());

    tft.fillRect(recvScreenX, recvScreenY, 3, 3, recvMessageBuf.colour);
  }

}

