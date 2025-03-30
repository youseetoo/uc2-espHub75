#include <Arduino.h>
#include "ESP32-HUB75-MatrixPanel-I2S-DMA.h"
#include <cJSON.h>

#define PANEL_WIDTH 64
#define PANEL_HEIGHT 32

MatrixPanel_I2S_DMA *dma_display = nullptr;

void setup()
{
    Serial.begin(115200);

    HUB75_I2S_CFG mxconfig;
    mxconfig.mx_width  = PANEL_WIDTH;
    mxconfig.mx_height = PANEL_HEIGHT;
    // Configure pins or chain length here if needed:
    // mxconfig.gpio_r1 = ...
    // mxconfig.gpio_g1 = ...
    // etc.

    dma_display = new MatrixPanel_I2S_DMA(mxconfig);
    dma_display->begin();
    dma_display->setPanelBrightness(64); 
    dma_display->clearScreen();
}

void loop()
{
    static String inputBuffer;

    while (Serial.available() > 0) {
      /*
      Usage:
      {"task":"/hub_act","x":0,"y":0,"diameter":10,"intensity":255,"r":255,"g":0,"b":0, "qid":0}
      */
        /*
        Example JSON:
        {
            "task": "/hub_act",
            "x": 0,
            "y": 0,
            "diameter": 10,
            "intensity": 255,
            "r": 255,
            "g": 0,
            "b": 0
        }
      */
        char c = (char)Serial.read();
        if (c == '\n') {
            cJSON *root = cJSON_Parse(inputBuffer.c_str());
            if (root) {
                cJSON *taskItem = cJSON_GetObjectItem(root, "task");
                if (taskItem && taskItem->valuestring && strcmp(taskItem->valuestring, "/hub_act") == 0) {
                    cJSON *xItem        = cJSON_GetObjectItem(root, "x");
                    cJSON *yItem        = cJSON_GetObjectItem(root, "y");
                    cJSON *diameterItem = cJSON_GetObjectItem(root, "diameter");
                    cJSON *intItem      = cJSON_GetObjectItem(root, "intensity");
                    cJSON *rItem        = cJSON_GetObjectItem(root, "r");
                    cJSON *gItem        = cJSON_GetObjectItem(root, "g");
                    cJSON *bItem        = cJSON_GetObjectItem(root, "b");
                    cJSON *qidItem      = cJSON_GetObjectItem(root, "qid");

                    if (xItem && yItem && diameterItem && intItem && rItem && gItem && bItem) {
                        int x         = xItem->valueint;
                        int y         = yItem->valueint;
                        int diameter  = diameterItem->valueint;
                        int radius    = diameter / 2;
                        int intensity = intItem->valueint;
                        int red       = rItem->valueint;
                        int green     = gItem->valueint;
                        int blue      = bItem->valueint;
                        int qid      = qidItem ? qidItem->valueint : 0;

                        if (intensity < 0)   intensity = 0;
                        if (intensity > 255) intensity = 255;

                        dma_display->setPanelBrightness(intensity);
                        uint16_t color = dma_display->color565(red, green, blue);
                        dma_display->fillCircle(x, y, radius, color);
                        log_i("Circle drawn at (%d, %d) with diameter %d and color (%d, %d, %d)", x, y, diameter, red, green, blue);
                        // Optionally, you can send a response back to the sender
                        // For example, you can send the qid back
                        if (qidItem) {
                            log_i("Response qid: %d", qid);
                        }
                    }
                }
                cJSON_Delete(root);
                /* return:
                ++
                {"qid":-1,"success":-1}
                --
                */
                cJSON *response = cJSON_CreateObject();
                cJSON_AddNumberToObject(response, "qid", 0);
                cJSON_AddNumberToObject(response, "success", 1);
                char *responseString = cJSON_PrintUnformatted(response);
                Serial.println(responseString);
                free(responseString);
                cJSON_Delete(response);
            
            }
            inputBuffer = "";
        } else {
            inputBuffer += c;
        }
    }
}
