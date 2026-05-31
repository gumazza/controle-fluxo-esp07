#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>

extern AsyncWebServer server;

// =========================
// INICIAR OTA
// =========================

void iniciarOTA() {

    ElegantOTA.begin(&server);
}

// =========================
// LOOP OTA
// =========================

void atualizarOTA() {

    ElegantOTA.loop();
}