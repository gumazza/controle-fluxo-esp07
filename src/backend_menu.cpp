#include <Arduino.h>

#include <ESP8266WiFi.h>

#include <ESP.h>

#include <WiFiManager.h>

#include "globals.h"
#include "storage.h"
#include "system_state.h"
#include "lcd_display.h"

WiFiManager wm;

void entrarBackend() {

    configVolumeAtivo = false;

    backendConfirmando = false;

    backendEditando = false;

    backendIndex = 0;

    estadoSistema = BACKEND_MENU;

    marcarLcdSujo();
}

void sairBackend() {

    salvarConfiguracoes();

    estadoSistema = STANDBY;

    backendEditando = false;

    backendConfirmando = false;

    marcarLcdSujo();
}

void backendClick() {

    if (
        backendIndex == 1 &&
        backendConfirmando
    ) {

        litrosAcumulados = 0;

        totalCiclos = 0;

        salvarConfiguracoes();

        backendConfirmando = false;
    }

    else if (
        backendIndex == 2 &&
        backendConfirmando
    ) {

        wm.resetSettings();

        ESP.restart();
    }

    else if (
        backendIndex == 3 &&
        backendConfirmando
    ) {

        ESP.restart();
    }

    else {

        backendEditando = !backendEditando;
    }

    marcarLcdSujo();
}

void backendLongPress() {

    if (!backendEditando) {

        sairBackend();

        return;
    }

    backendEditando = false;

    salvarConfiguracoes();

    marcarLcdSujo();
}

void backendDoubleClick() {

    if (
        backendIndex == 1 ||
        backendIndex == 2 ||
        backendIndex == 3
    ) {

        backendConfirmando = !backendConfirmando;
    }

    marcarLcdSujo();
}

void processarRotacaoBackend(int direcao) {

    if (estadoSistema != BACKEND_MENU) {
        return;
    }

    if (!backendEditando) {

        if (direcao > 0) {
            backendIndex++;
        }
        else {
            backendIndex--;
        }

        if (backendIndex < 0) {
            backendIndex = 5;
        }

        if (backendIndex > 5) {
            backendIndex = 0;
        }

        backendConfirmando = false;
    }

    else {

        switch (backendIndex) {

            case 0:

                fator_calibracao += direcao > 0 ? 0.1f : -0.1f;

                if (fator_calibracao < 1) {
                    fator_calibracao = 1;
                }

                if (fator_calibracao > 20) {
                    fator_calibracao = 20;
                }

                break;

            case 4:

                timeoutSemFluxo += direcao > 0 ? 1000 : -1000;

                if (timeoutSemFluxo < 1000) {
                    timeoutSemFluxo = 1000;
                }

                if (timeoutSemFluxo > 60000) {
                    timeoutSemFluxo = 60000;
                }

                break;

            case 5:

                timeoutEnchimento += direcao > 0 ? 10000 : -10000;

                if (timeoutEnchimento < 60000) {
                    timeoutEnchimento = 60000;
                }

                if (timeoutEnchimento > 7200000) {
                    timeoutEnchimento = 7200000;
                }

                break;
        }
    }

    marcarLcdSujo();
}
