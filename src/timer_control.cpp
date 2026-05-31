#include <Arduino.h>

#include "globals.h"
#include "system_state.h"

// =========================
// TEMPOS
// =========================

unsigned long ultimoTimer = 0;

// =========================
// TEXTO TIMER
// =========================

const char* obterTextoTimer() {

    if (estadoSistema == TIMER_OFF)
        return "OFF";

    if (estadoSistema == TIMER_ON)
        return "ON";

    if (estadoSistema == TIMER_PAUSA)
        return "PAUSA";

    return "";
}

// =========================
// AJUSTE
// =========================

void incrementarTimer(int direcao) {

    // =========================
    // SEGUNDOS
    // =========================

    if (tempoTimer < 3600) {

        tempoTimer += direcao * 10;
    }

    // =========================
    // HORAS
    // =========================

    else {

        tempoTimer += direcao * 300;
    }

    if ((long)tempoTimer < 0)
        tempoTimer = 0;

    tempoRestanteTimer = tempoTimer;
}

// =========================
// TIMER
// =========================

void atualizarTimer() {

    // =========================
    // TIMER RODANDO
    // =========================

    if (estadoSistema != TIMER_ON)
        return;

    // =========================
    // 1 SEGUNDO
    // =========================

    if (
        millis() - ultimoTimer
            < 1000
    ) {

        return;
    }

    ultimoTimer = millis();

    // =========================
    // CONTAGEM
    // =========================

    if (tempoRestanteTimer > 0) {

        tempoRestanteTimer--;
    }

    // =========================
    // FINAL TIMER
    // =========================

    if (tempoRestanteTimer == 0) {

        estadoSistema = ENCHENDO;

        releLigado = true;

        timerRodando = false;

        timerPausado = false;
    }
}