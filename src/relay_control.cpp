#include <Arduino.h>

#include "config.h"
#include "globals.h"
#include "system_state.h"
#include "lcd_display.h"

// =========================
// INICIAR
// =========================

void iniciarRele() {

    pinMode(PINO_RELE, OUTPUT);

    digitalWrite(PINO_RELE, LOW);

    pinMode(PINO_OPTO, OUTPUT);

    digitalWrite(PINO_OPTO, LOW);

    pinMode(PINO_BOTAO_MANUAL, INPUT_PULLUP);
}

// =========================
// PULSO OPTO
// =========================

void enviarPulsoOpto() {

    digitalWrite(PINO_OPTO, HIGH);

    tempoPulsoOpto = millis();

    pulsoOptoEnviado = true;
}

// =========================
// BOTAO MANUAL
// =========================

void controlarBotaoManual() {

    // =========================
    // BLOQUEADO AUTOMATICO
    // =========================

    if (
        estadoSistema == ENCHENDO ||
        estadoSistema == PAUSADO
    ) {

        return;
    }

    // =========================
    // BOTAO PRESSIONADO
    // =========================

    if (
        digitalRead(PINO_BOTAO_MANUAL)
            == LOW
    ) {

        ligarBacklight();

        // =========================
        // LIMITE ATINGIDO
        // =========================

        if (volume_total >= volume_limite) {

            releLigado = false;

            digitalWrite(PINO_RELE, LOW);

            return;
        }

        modoManual = true;

        releLigado = true;

        digitalWrite(PINO_RELE, HIGH);

        return;
    }

    // =========================
    // SOLTO
    // =========================

    if (modoManual) {

        modoManual = false;

        releLigado = false;

        digitalWrite(PINO_RELE, LOW);
    }
}

// =========================
// CONTROLE RELE
// =========================

void atualizarRele() {

    controlarBotaoManual();

    // =========================
    // LIMITE VOLUME
    // =========================

    if (
        releLigado &&
        volume_total >= volume_limite
    ) {

        releLigado = false;

        digitalWrite(PINO_RELE, LOW);

        estadoSistema = STANDBY;

        // =========================
        // PULSO FINAL
        // =========================

        if (!modoManual) {

            enviarPulsoOpto();
        }

        totalCiclos++;

        litrosAcumulados += volume_total;

        return;
    }

    // =========================
    // RELE
    // =========================

    digitalWrite(
        PINO_RELE,
        releLigado
    );

    // =========================
    // DESLIGAR OPTO
    // =========================

    if (
        pulsoOptoEnviado &&
        millis() - tempoPulsoOpto >= 1000
    ) {

        digitalWrite(PINO_OPTO, LOW);

        pulsoOptoEnviado = false;
    }
}