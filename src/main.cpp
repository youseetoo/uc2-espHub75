#include <Arduino.h>
#include <Adafruit_Protomatter.h>
#include <ArduinoJson.h>

// For a 64x32 panel, one chain, example pin setup for MatrixPortal S3.
#define WIDTH       64
#define HEIGHT      32
#define CHAIN       1
#define BIT_DEPTH   2

uint8_t rgbPins[] = {
  42,
  41,
  40,
  38,
  39,
  37,
};
uint8_t addrPins[] = {
  45,
  36,
  48,
  35,
  21};

uint8_t clockPin = 2;
uint8_t latchPin = 47;
uint8_t oePin = 14;

Adafruit_Protomatter matrix(WIDTH, BIT_DEPTH, 1, rgbPins, 
    sizeof(addrPins) / sizeof(addrPins[0]), addrPins, clockPin, latchPin, oePin, true);




// Simple serial input buffer
String incomingData;

void setup() {
  Serial.begin(115200);
  matrix.begin();
  matrix.fillScreen(0);
  matrix.setDuty(4);
  matrix.show();
}

void drawCircleInPixels(int xPx, int yPx, int radiusPx, uint16_t color) {
  // Simple midpoint circle or library method
  // Here using Protomatter’s drawCircle() function
  matrix.fillScreen(0);
  matrix.fillCircle(xPx, yPx, radiusPx, color);
  matrix.show();
}

void handleJSON(const String &jsonString) {
  // Use ArduinoJson
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, jsonString);
  if (error) {
    Serial.println("{\"status\":\"error\",\"info\":\"JSON parse failed\"}");
    return;
  }

  // Check if task == /hub_act
  const char* task = doc["task"];
  // {"task":"/hub_act","x":100,"y":10,"intensity":[255,255,255],"radius":2,"qid":0}
  if (!task) {
    Serial.println("{\"status\":\"error\",\"info\":\"Missing task\"}");
    return;
  }
  if (strcmp(task, "/hub_act") != 0) {
    Serial.println("{\"status\":\"error\",\"info\":\"Unknown task\"}");
    return;
  }

  // Parse fields
  float xMm     = doc["x"] | 0.0;
  float yMm     = doc["y"] | 0.0;
  int r         = doc["intensity"][0] | 255;
  int g         = doc["intensity"][1] | 255;
  int b         = doc["intensity"][2] | 255;
  int radiusPx  = doc["radius"] | 2;
  int qid       = doc["qid"]     | 0;

  // Convert mm to pixel coords (panel has 3mm pitch)
  int xPx = round(xMm / 3.0f);
  int yPx = round(yMm / 3.0f);

  // Bounds check if you wish
  /*
  if (xPx < 0 || xPx >= WIDTH || yPx < 0 || yPx >= HEIGHT) {
    Serial.print("{\"task\":\"/hub_act\",\"status\":\"error\",\"qid\":");
    Serial.print(qid);
    Serial.println(",\"info\":\"Coordinates out of range\"}");
    return;
  }

  // Convert color to 565
  uint16_t color565 = matrix.color565(r, g, b);

  // Draw
  drawCircleInPixels(xPx, yPx, radiusPx, color565);

  // Return success
  Serial.print("{\"task\":\"/hub_act\",\"status\":\"success\",\"qid\":");
  Serial.print(qid);
  Serial.println("}");
}

void loop() {
  // Accumulate incoming serial data
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n') {
      handleJSON(incomingData);
      incomingData = "";
    } else {
      incomingData += c;
    }
  }
}
