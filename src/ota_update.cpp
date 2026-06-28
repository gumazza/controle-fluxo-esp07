#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Updater.h>
#include <ESP.h>

#include "ota_page.h"

extern AsyncWebServer server;

static bool otaAtivo = false;

bool otaEmProgresso() {

    return otaAtivo;
}

static void imprimirInfoOta() {

    uint32_t flashReal = ESP.getFlashChipRealSize();

    uint32_t flashCfg = ESP.getFlashChipSize();

    uint32_t sketch = ESP.getSketchSize();

    uint32_t otaLivre = ESP.getFreeSketchSpace();

    Serial.print(F("OTA flash real: "));

    Serial.print(flashReal);

    Serial.print(F(" · config: "));

    Serial.print(flashCfg);

    Serial.print(F(" · sketch: "));

    Serial.print(sketch);

    Serial.print(F(" · espaco OTA: "));

    Serial.println(otaLivre);

    if (flashReal != flashCfg) {

        Serial.println(
            F("AVISO: flash real != config. Recompile com board_build.flash_size correto.")
        );
    }

    if (sketch > otaLivre) {

        Serial.println(F("AVISO: sketch maior que espaco OTA livre."));
    }
}

static void registrarRotaInfoOta() {

    server.on("/api/ota/info", HTTP_GET,
        [](AsyncWebServerRequest *request) {

            JsonDocument doc;

            doc["flash_bytes"] = ESP.getFlashChipRealSize();

            doc["flash_mb"] = ESP.getFlashChipRealSize() / (1024UL * 1024UL);

            doc["flash_config_bytes"] = ESP.getFlashChipSize();

            doc["sketch_bytes"] = ESP.getSketchSize();

            doc["sketch_kb"] = ESP.getSketchSize() / 1024UL;

            doc["ota_livre_bytes"] = ESP.getFreeSketchSpace();

            doc["ota_livre_kb"] = ESP.getFreeSketchSpace() / 1024UL;

            doc["ota_ok"] = ESP.getSketchSize() <= ESP.getFreeSketchSpace();

            doc["flash_ok"] = (
                ESP.getFlashChipRealSize() == ESP.getFlashChipSize()
            );

            char json[220];

            serializeJson(doc, json, sizeof(json));

            request->send(200, "application/json", json);
        });
}

static void registrarRotaUploadOta() {

    server.on("/update", HTTP_GET,
        [](AsyncWebServerRequest *request) {

            request->send_P(200, "text/html", OTA_PAGE_HTML);
        });

    server.on(
        "/update",
        HTTP_POST,
        [](AsyncWebServerRequest *request) {

            otaAtivo = false;

            if (Update.hasError()) {

                Serial.print(F("OTA erro: "));

                Update.printError(Serial);

                request->send(
                    500,
                    "text/plain",
                    "Falha na gravacao. Verifique o Serial."
                );

                return;
            }

            request->send(
                200,
                "text/plain",
                "OK. Reiniciando..."
            );

            delay(300);

            ESP.restart();
        },
        [](AsyncWebServerRequest *request,
           String filename,
           size_t index,
           uint8_t *data,
           size_t len,
           bool final) {

            if (index == 0) {

                otaAtivo = true;

                Serial.print(F("OTA arquivo: "));

                Serial.println(filename);

                uint32_t espaco = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;

                Serial.print(F("OTA espaco max: "));

                Serial.println(espaco);

                if (!Update.begin(espaco)) {

                    Update.printError(Serial);
                }
            }

            if (Update.hasError()) {
                return;
            }

            if (len > 0) {

                if (Update.write(data, len) != len) {

                    Update.printError(Serial);
                }
            }

            if (final) {

                if (Update.end(true)) {

                    Serial.printf(
                        "OTA OK: %u bytes\n",
                        (unsigned)(index + len)
                    );
                }
                else {

                    Update.printError(Serial);
                }
            }
        });
}

void iniciarOTA() {

    registrarRotaInfoOta();

    registrarRotaUploadOta();

    imprimirInfoOta();
}

void atualizarOTA() {
}
