#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "globals.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void iniciarWebServer() {

    server.on("/", HTTP_GET,
        [](AsyncWebServerRequest *request) {

        request->send(200,
            "text/plain",
            "Controle Fluxo Online");
    });

    ws.onEvent(
        [](AsyncWebSocket *server,
           AsyncWebSocketClient *client,
           AwsEventType type,
           void *arg,
           uint8_t *data,
           size_t len) {
    });

    server.addHandler(&ws);

    server.begin();
}

void atualizarWebSocket() {

    JsonDocument doc;

    doc["fluxo"] = fluxo;
    doc["volume"] = volume_total;
    doc["setpoint"] = volume_limite;

    String json;

    serializeJson(doc, json);

    ws.textAll(json);
}