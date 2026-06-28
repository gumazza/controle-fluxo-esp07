#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <ESP.h>

extern AsyncWebServer server;

static bool otaAtivo = false;

static unsigned long ultimoLogOta = 0;

bool otaEmProgresso() {

    return otaAtivo;
}

void iniciarOTA() {

    ElegantOTA.setAutoReboot(true);

    ElegantOTA.onStart([]() {

        otaAtivo = true;

        ultimoLogOta = 0;

        Serial.println(F("OTA: inicio do upload"));
    });

    ElegantOTA.onProgress([](size_t atual, size_t total) {

        if (millis() - ultimoLogOta < 1000UL) {
            return;
        }

        ultimoLogOta = millis();

        if (total > 0) {
            Serial.printf(
                "OTA: %u%% (%u / %u bytes)\n",
                (unsigned)(atual * 100UL / total),
                (unsigned)atual,
                (unsigned)total
            );
        }
    });

    ElegantOTA.onEnd([](bool ok) {

        otaAtivo = false;

        if (ok) {
            Serial.println(F("OTA: concluido, reiniciando..."));
        }
        else {
            Serial.println(F("OTA: falhou"));
        }
    });

    ElegantOTA.begin(&server);

    Serial.print(F("OTA: espaco livre "));

    Serial.print(ESP.getFreeSketchSpace());

    Serial.println(F(" bytes"));
}

void atualizarOTA() {

    ElegantOTA.loop();
}
