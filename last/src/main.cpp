// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <Arduino_JSON.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <Preferences.h>

#define EEPROM_SIZE 4
#define ARDUINOJSON_ENABLE_ARDUINO_STRING 1

Preferences preferences;

const char *ssid = "Torkrobotik";
const char *password = "wiseking";

AsyncWebServer server(80);

AsyncWebSocket ws("/ws");

const char *http_username = "admin";
const char *http_password = "admin";

const char *speedInput = "speed";
const char *dataBitInput = "dataBit";
const char *parityInput = "parity";
const char *stopBitInput = "stopBit";

int serialState = 0;

void initSPIFFS()
{
    if (!SPIFFS.begin(true))
    {
        Serial2.println("An error has occurred while mounting SPIFFS");
    }
    Serial2.println("SPIFFS mounted successfully");
}

String convertToString(uint8_t *a, int size)
{
    int i;
    String s = "";
    for (i = 0; i < size; i++)
    {
        s = s + (char)a[i];
    }
    return s;
}

void initWiFi()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial2.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(1000);
    }
    Serial2.println(WiFi.localIP());
}

void notifyClients(String state)
{
    Serial2.println(state);
    ws.textAll(state);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = 0;

        String message = convertToString(data, len);

        notifyClients(message);

        StaticJsonDocument<1024> doc;
        DeserializationError error = deserializeJson(doc, data);

        if (error)
        {
            Serial2.print(F("deserializeJson() failed: "));
            Serial2.println(error.c_str());
        }

        String text = doc["text"].as<String>();
        String date = doc["date"].as<String>();

        Serial2.println("Data received: " + text + " " + date);
    }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial2.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial2.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
        handleWebSocketMessage(arg, data, len);
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

void initWebSocket()
{
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}

void serialDecider(int sN, int speed, int &serialOn)
{
    if (serialOn == 0)
    {
        switch (sN)
        {
        case 0:
            Serial2.begin(speed, SERIAL_7N1, 3, 1);
            serialOn = 1;
            break;
        case 1:
            Serial2.begin(speed, SERIAL_8N1, 3, 1);
            serialOn = 1;
            break;
        case 2:
            Serial2.begin(speed, SERIAL_7N2, 3, 1);
            serialOn = 1;
            break;
        case 3:
            Serial2.begin(speed, SERIAL_8N2, 3, 1);
            serialOn = 1;
            break;
        case 4:
            Serial2.begin(speed, SERIAL_7E1, 3, 1);
            serialOn = 1;
            break;
        case 5:
            Serial2.begin(speed, SERIAL_8E1, 3, 1);
            serialOn = 1;
            break;
        case 6:
            Serial2.begin(speed, SERIAL_7E2, 3, 1);
            serialOn = 1;
            break;
        case 7:
            Serial2.begin(speed, SERIAL_8E2, 3, 1);
            serialOn = 1;
            break;
        case 8:
            Serial2.begin(speed, SERIAL_7O1, 3, 1);
            serialOn = 1;
            break;
        case 9:
            Serial2.begin(speed, SERIAL_8O1, 3, 1);
            serialOn = 1;
            break;
        case 10:
            Serial2.begin(speed, SERIAL_7O2, 3, 1);
            serialOn = 1;
            break;
        case 11:
            Serial2.begin(speed, SERIAL_8O2, 3, 1);
            serialOn = 1;
            break;

        default:
            break;
        }
    }
    else
    {
        Serial2.end();
        serialOn = 0;
        serialDecider(sN, speed, serialOn);
    }
}

int serialNumerator(int dataBit, int parity, int stopBit)
{

    if (dataBit == 7 && parity == 0 && stopBit == 1)
    {
        return 0;
    }
    else if (dataBit == 8 && parity == 0 && stopBit == 1)
    {
        Serial.println("serialNum 1");
        return 1;
    }
    else if (dataBit == 7 && parity == 0 && stopBit == 2)
    {
        return 2;
    }
    else if (dataBit == 8 && parity == 0 && stopBit == 2)
    {
        return 3;
    }
    else if (dataBit == 7 && parity == 2 && stopBit == 1)
    {
        return 4;
    }
    else if (dataBit == 8 && parity == 2 && stopBit == 1)
    {
        return 5;
    }
    else if (dataBit == 7 && parity == 2 && stopBit == 2)
    {
        return 6;
    }
    else if (dataBit == 8 && parity == 2 && stopBit == 2)
    {
        return 7;
    }
    else if (dataBit == 7 && parity == 1 && stopBit == 1)
    {
        return 8;
    }
    else if (dataBit == 8 && parity == 1 && stopBit == 1)
    {
        return 9;
    }
    else if (dataBit == 7 && parity == 1 && stopBit == 2)
    {
        return 10;
    }
    else if (dataBit == 8 && parity == 1 && stopBit == 2)
    {
        return 11;
    }
    return 1;
}

void parityStringToInteger(String p, int &i)
{

    if (p == "None")
    {
        i = 0;
    }
    else if (p == "Odd")
    {
        i = 1;
    }
    else if (p == "Even")
    {
        i = 2;
    }
}

void setup()
{

    preferences.begin("serial-config", false);
    preferences.putInt("speed", 9600);
    preferences.putInt("dataBit", 8);
    preferences.putInt("parity", 0);
    preferences.putInt("stopBit", 1);

    int sN = serialNumerator(preferences.getInt("dataBit"), preferences.getInt("parity"), preferences.getInt("stopBit"));

    serialDecider(sN, preferences.getInt("speed"), serialState);

    initWiFi();
    initSPIFFS();
    initWebSocket();

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { if(!request->authenticate(http_username, http_password)) 
                    return request->requestAuthentication();               
                request->send(SPIFFS, "/index.html", "text/html", false); });

    server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                if(!request->authenticate(http_username, http_password)) 
                {
                    return request->requestAuthentication();
                } 
                else 
                {

                    String parityString;

                    int speed;
                    int dataBit;
                    int stopBit;
                    int parity;

                    if(request->hasParam(speedInput) && request->hasParam(dataBitInput) && 
                            request->hasParam(parityInput) && request->hasParam(stopBitInput)){

                        speed = request->getParam(speedInput)->value().toInt();
                        preferences.putInt("speed",speed);
                        dataBit = request->getParam(dataBitInput)->value().toInt();
                        preferences.putInt("dataBit",dataBit);
                        parityString = request->getParam(parityInput)->value();
                        parityStringToInteger(parityString,parity);
                        preferences.putInt("parity",parity);
                        stopBit = request->getParam(stopBitInput)->value().toInt();
                        preferences.putInt("stopBit",stopBit);

                        int sN = serialNumerator(dataBit,parity,stopBit);
                        
                        serialDecider(sN,speed,serialState);
                        
                        Serial2.println("if");

                     } else {} 

                        request->send(SPIFFS, "/index.html", "text/html", false);
                     
                     } });

    server.on("/logged-out", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/logged-out.html", "text/html", false); });

    server.on("/logout", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(401); });

    server.serveStatic("/", SPIFFS, "/");

    server.begin();
}

void loop()
{
    if (Serial2.available() > 0)
    {
        String r = Serial2.readString();
        Serial2.print("Data sent: ");
        Serial2.println(r);
        ws.textAll(JSON.stringify(r));
    }

    ws.cleanupClients();
}