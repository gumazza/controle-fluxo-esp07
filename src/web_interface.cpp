#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#include "config.h"
#include "globals.h"
#include "wifi_control.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

static bool webServerIniciado = false;

void iniciarWebServer() {

    if (webServerIniciado) {
        return;
    }

    server.on("/", HTTP_GET,
        [](AsyncWebServerRequest *request) {

            request->send(
                200,
                "text/plain",
                "Controle Volume Online"
            );
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

    webServerIniciado = true;
}

void pararWebServer() {

    if (!webServerIniciado) {
        return;
    }

    server.end();

    webServerIniciado = false;
}

void atualizarWebSocket() {

    if (!redeAtiva()) {
        return;
    }

    static unsigned long ultimoEnvio = 0;

    if (millis() - ultimoEnvio < INTERVALO_WEBSOCKET) {
        return;
    }

    ultimoEnvio = millis();

    if (ws.count() == 0) {
        return;
    }

    JsonDocument doc;

    doc["fluxo"] = fluxo;
    doc["volume"] = volume_total;
    doc["setpoint"] = volume_limite;
    doc["pwm"] = pwmRetencao;

    char json[128];

    serializeJson(doc, json, sizeof(json));

    ws.textAll(json);
}
