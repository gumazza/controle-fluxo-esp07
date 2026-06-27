#include <Arduino.h>

#include <ESP8266WiFi.h>

#include "wifi_control.h"
#include "lcd_display.h"
#include "web_interface.h"
#include "ota_update.h"

static bool servicosRedeIniciados = false;

static unsigned long ultimaTentativaWifi = 0;

static void iniciarServicosRede() {

    if (servicosRedeIniciados) {
        return;
    }

    iniciarWebServer();

    iniciarOTA();

    servicosRedeIniciados = true;

    marcarLcdSujo();
}

void iniciarWifi() {

    WiFi.persistent(true);

    WiFi.mode(WIFI_STA);

    WiFi.setAutoReconnect(true);

    if (WiFi.SSID().length() > 0) {

        WiFi.begin();

        ultimaTentativaWifi = millis();
    }
}

void atualizarWifi() {

    if (WiFi.status() == WL_CONNECTED) {

        iniciarServicosRede();

        return;
    }

    if (WiFi.SSID().length() == 0) {
        return;
    }

    if (millis() - ultimaTentativaWifi >= 30000UL) {

        WiFi.reconnect();

        ultimaTentativaWifi = millis();
    }
}

bool wifiConectado() {

    return WiFi.status() == WL_CONNECTED;
}

bool servicosRedeAtivos() {

    return servicosRedeIniciados;
}
