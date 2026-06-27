#include <Arduino.h>

#include <ESP8266WiFi.h>

#include <ESP.h>

#include <WiFiManager.h>

#include "globals.h"
#include "storage.h"
#include "system_state.h"
#include "encoder_control.h"

WiFiManager wm;

// =========================
// ENTRAR
// =========================

void entrarBackend() {

    estadoSistema = BACKEND_MENU;

    backendIndex = 0;

    backendEditando = false;

    backendConfirmando = false;

    encoder.setEncoderValue(0);
}

// =========================
// SAIR
// =========================

void sairBackend() {

    salvarConfiguracoes();

    estadoSistema = STANDBY;

    backendEditando = false;

    backendConfirmando = false;
}

// =========================
// CLICK
// =========================

void backendClick() {

    // =========================
    // RESET STATS
    // =========================

    if (
        backendIndex == 1 &&
        backendConfirmando
    ) {

        litrosAcumulados = 0;

        totalCiclos = 0;

        salvarConfiguracoes();

        backendConfirmando = false;
    }

    // =========================
    // RESET WIFI
    // =========================

    else if (
        backendIndex == 2 &&
        backendConfirmando
    ) {

        wm.resetSettings();

        ESP.restart();
    }

    // =========================
    // REBOOT ESP
    // =========================

    else if (
        backendIndex == 3 &&
        backendConfirmando
    ) {

        ESP.restart();
    }

    // =========================
    // ENTRAR/SAIR EDIÇÃO
    // =========================

    else {

        backendEditando =
            !backendEditando;
    }
}

// =========================
// LONG PRESS
// =========================

void backendLongPress() {

    // =========================
    // SAIR MENU
    // =========================

    if (!backendEditando) {

        sairBackend();

        return;
    }

    // =========================
    // FINALIZA EDIÇÃO
    // =========================

    backendEditando = false;

    salvarConfiguracoes();
}

// =========================
// DUPLO CLICK
// =========================

void backendDoubleClick() {

    if (
        backendIndex == 1 ||
        backendIndex == 2 ||
        backendIndex == 3
    ) {

        backendConfirmando =
            !backendConfirmando;
    }
}

// =========================
// MENU
// =========================

void atualizarBackendMenu() {

    if (
        estadoSistema != BACKEND_MENU
    ) {

        return;
    }

    int valor =
        encoder.readEncoder();

    if (valor == 0)
        return;

    // =========================
    // NAVEGAÇÃO
    // =========================

    if (!backendEditando) {

        if (valor > 0)
            backendIndex++;
        else
            backendIndex--;

        if (backendIndex < 0)
            backendIndex = 5;

        if (backendIndex > 5)
            backendIndex = 0;
    }

    // =========================
    // EDIÇÃO
    // =========================

    else {

        switch (backendIndex) {

            // =========================
            // CALIBRAÇÃO
            // =========================

            case 0:

                fator_calibracao +=
                    valor > 0 ? 0.1 : -0.1;
                    
                if (fator_calibracao < 1)
                    fator_calibracao = 1;

                if (fator_calibracao > 20)
                    fator_calibracao = 20;

                break;

            // =========================
            // WATCHDOG FLUXO
            // =========================

            case 4:

                timeoutSemFluxo +=
                    valor > 0 ? 1000 : -1000;

                if (timeoutSemFluxo < 1000)
                    timeoutSemFluxo = 1000;

                if (timeoutSemFluxo > 60000)
                    timeoutSemFluxo = 60000;

                break;

            // =========================
            // WATCHDOG ENCHIMENTO
            // =========================

            case 5:

                timeoutEnchimento +=
                    valor > 0 ? 10000 : -10000;

                if (timeoutEnchimento < 60000)
                    timeoutEnchimento = 60000;

                if (timeoutEnchimento > 7200000)
                    timeoutEnchimento = 7200000;

                break;
        }
    }

    encoder.setEncoderValue(0);
}