#include <Arduino.h>

#include "config.h"
#include "globals.h"

// =========================
// PULSOS
// =========================

volatile unsigned long contadorPulsos = 0;

// =========================
// TEMPO
// =========================

unsigned long ultimoCalculoFluxo = 0;

// =========================
// INTERRUP
// =========================

IRAM_ATTR void contadorPulso() {

    contadorPulsos++;
}

// =========================
// INICIAR
// =========================

void iniciarSensorFluxo() {

    pinMode(PINO_SENSOR, INPUT_PULLUP);

    attachInterrupt(
        digitalPinToInterrupt(PINO_SENSOR),
        contadorPulso,
        FALLING
    );
}

// =========================
// CALCULO
// =========================

void atualizarFluxo() {

    if (
        millis() - ultimoCalculoFluxo
            < INTERVALO_FLUXO
    ) {

        return;
    }

    noInterrupts();

    unsigned long pulsos = contadorPulsos;

    contadorPulsos = 0;

    interrupts();

    // =========================
    // YF-S201
    //
    // Frequencia:
    //
    // F = 7.5 * Q
    //
    // Q = L/min
    // =========================

    fluxo =
        ((float)pulsos / 7.5);

    // =========================
    // L/min -> L/s
    // =========================

    float litrosSegundo =
        fluxo / 60.0;

    // =========================
    // VOLUME
    // =========================

    volume =
        litrosSegundo *
        (INTERVALO_FLUXO / 1000.0);

    // =========================
    // SOMA TOTAL
    // =========================

    if (releLigado) {

        volume_total += volume;
    }

    // =========================
    // LIMITES
    // =========================

    if (volume_total < 0)
        volume_total = 0;

    if (fluxo < 0)
        fluxo = 0;

    ultimoCalculoFluxo = millis();
}