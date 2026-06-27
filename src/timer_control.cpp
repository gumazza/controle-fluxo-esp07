#include <Arduino.h>

#include "globals.h"
#include "system_state.h"
#include "valve_control.h"
#include "lcd_display.h"

unsigned long ultimoTimer = 0;

static void notificarTimer() {

    ligarBacklight();

    marcarLcdSujo();
}

bool emModoTimer() {

    return (
        estadoSistema == TIMER_OFF ||
        estadoSistema == TIMER_ON ||
        estadoSistema == TIMER_PAUSA
    );
}

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

void ajustarTempoTimer(int direcao) {

    if (
        estadoSistema != STANDBY &&
        estadoSistema != TIMER_OFF
    ) {
        return;
    }

    incrementarTimer(direcao);

    notificarTimer();
}

void entrarModoTimer() {

    if (estadoSistema != STANDBY) {
        return;
    }

    estadoSistema = TIMER_OFF;

    tempoRestanteTimer = tempoTimer;

    timerRodando = false;

    timerPausado = false;

    notificarTimer();
}

void iniciarContagemTimer() {

    if (estadoSistema == STANDBY) {

        entrarModoTimer();
    }

    if (estadoSistema != TIMER_OFF || tempoTimer == 0) {
        return;
    }

    estadoSistema = TIMER_ON;

    tempoRestanteTimer = tempoTimer;

    timerRodando = true;

    timerPausado = false;

    ultimoTimer = millis();

    notificarTimer();
}

void pausarContagemTimer() {

    if (estadoSistema != TIMER_ON) {
        return;
    }

    estadoSistema = TIMER_PAUSA;

    timerPausado = true;

    notificarTimer();
}

void retomarContagemTimer() {

    if (estadoSistema != TIMER_PAUSA) {
        return;
    }

    estadoSistema = TIMER_ON;

    timerPausado = false;

    ultimoTimer = millis();

    notificarTimer();
}

void cancelarTimer() {

    if (!emModoTimer()) {
        return;
    }

    timerRodando = false;

    timerPausado = false;

    estadoSistema = STANDBY;

    tempoRestanteTimer = tempoTimer;

    notificarTimer();
}

bool acaoTimerWeb() {

    if (estadoSistema == TIMER_OFF) {

        iniciarContagemTimer();

        return true;
    }

    if (estadoSistema == TIMER_ON) {

        pausarContagemTimer();

        return true;
    }

    if (estadoSistema == TIMER_PAUSA) {

        retomarContagemTimer();

        return true;
    }

    if (estadoSistema == STANDBY && tempoTimer > 0) {

        iniciarContagemTimer();

        return true;
    }

    return false;
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

    marcarLcdSujo();

    if (tempoRestanteTimer == 0) {

        timerRodando = false;

        timerPausado = false;

        iniciarEnchimento();
    }
}
