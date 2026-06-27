#include <Arduino.h>

#include <WiFiManager.h>

#include "config.h"

void verificarModoBackend() {

    bool encoder = digitalRead(ENCODER_PINO_BTN) == LOW;
    bool manual = digitalRead(PINO_BOTAO_MANUAL) == LOW;

    if (encoder && manual) {

        delay(3000);

        if (
            digitalRead(ENCODER_PINO_BTN) == LOW &&
            digitalRead(PINO_BOTAO_MANUAL) == LOW
        ) {

            WiFiManager wm;

            wm.setConfigPortalTimeout(180);

            wm.startConfigPortal(WIFI_AP_SSID);
        }
    }
}