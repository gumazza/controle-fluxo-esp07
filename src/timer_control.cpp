#include <Arduino.h>

#include "globals.h"
#include "system_state.h"
#include "valve_control.h"

unsigned long ultimoTimer = 0;

const char* obterTextoTimer() {

    if (estadoSistema == TIMER_OFF) {
        return "OFF";
    }

    if (estadoSistema == TIMER_ON) {
        return "ON";
    }

    if (estadoSistema == TIMER_PAUSA) {
        return "PAUSA";
    }

    return "";
}

void incrementarTimer(int direcao) {

    if (tempoTimer < 3600) {
        tempoTimer += direcao * 10;
    }
    else {
        tempoTimer += direcao * 300;
    }

    if ((long)tempoTimer < 0) {
        tempoTimer = 0;
    }

    tempoRestanteTimer = tempoTimer;
}

void atualizarTimer() {

    if (estadoSistema != TIMER_ON) {
        return;
    }

    if (millis() - ultimoTimer < 1000) {
        return;
    }

    ultimoTimer = millis();

    if (tempoRestanteTimer > 0) {
        tempoRestanteTimer--;
    }

    if (tempoRestanteTimer == 0) {

        timerRodando = false;

        timerPausado = false;

        iniciarEnchimento();
    }
}
