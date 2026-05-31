#include <Arduino.h>

#include "config.h"
#include "globals.h"
#include "system_state.h"

// =========================
// TEMPOS
// =========================

unsigned long ultimoFluxoDetectado = 0;

unsigned long inicioEnchimento = 0;

// =========================
// WATCHDOGS
// =========================

void verificarWatchdogs() {

    // =========================
    // RESETA WATCHDOGS
    // =========================

    if (!releLigado) {

        ultimoFluxoDetectado = millis();

        inicioEnchimento = millis();

        erroSemFluxo = false;

        erroTimeout = false;

        return;
    }

    // =========================
    // DETECTOU FLUXO
    // =========================

    if (fluxo > 0.05) {

        ultimoFluxoDetectado = millis();
    }

    // =========================
    // SEM FLUXO
    // =========================

    if (
        millis() - ultimoFluxoDetectado
            >= TEMPO_SEM_FLUXO
    ) {

        erroSemFluxo = true;

        releLigado = false;

        estadoSistema = ERRO;

        return;
    }

    // =========================
    // TIMEOUT ENCHIMENTO
    // =========================

    if (
        millis() - inicioEnchimento
            >= TEMPO_MAX_ENCHIMENTO
    ) {

        erroTimeout = true;

        releLigado = false;

        estadoSistema = ERRO;

        return;
    }
}