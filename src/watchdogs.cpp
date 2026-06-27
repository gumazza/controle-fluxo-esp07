#include <Arduino.h>

#include "config.h"
#include "globals.h"
#include "system_state.h"
#include "watchdogs.h"
#include "valve_control.h"

unsigned long ultimoFluxoDetectado = 0;

unsigned long inicioEnchimento = 0;

void marcarInicioEnchimento() {

    inicioEnchimento = millis();

    ultimoFluxoDetectado = millis();
}

void resetarWatchdogs() {

    ultimoFluxoDetectado = millis();

    inicioEnchimento = millis();

    erroSemFluxo = false;

    erroTimeout = false;
}

void verificarWatchdogs() {

    if (!releLigado) {

        resetarWatchdogs();

        return;
    }

    if (fluxo > FLUXO_MINIMO_LMIN) {

        ultimoFluxoDetectado = millis();
    }

    if (
        millis() - ultimoFluxoDetectado
            >= timeoutSemFluxo
    ) {

        erroSemFluxo = true;

        releLigado = false;

        fecharValvula();

        estadoSistema = ERRO;

        return;
    }

    if (
        millis() - inicioEnchimento
            >= timeoutEnchimento
    ) {

        erroTimeout = true;

        releLigado = false;

        fecharValvula();

        estadoSistema = ERRO;

        return;
    }
}
