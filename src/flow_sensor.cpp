#include <Arduino.h>

#include "config.h"
#include "globals.h"
#include "fill_logic.h"
#include "valve_control.h"

volatile unsigned long contadorPulsos = 0;

unsigned long ultimoCalculoFluxo = 0;

IRAM_ATTR void contadorPulso() {

    contadorPulsos++;
}

void iniciarSensorFluxo() {

    pinMode(PINO_SENSOR, INPUT_PULLUP);

    attachInterrupt(
        digitalPinToInterrupt(PINO_SENSOR),
        contadorPulso,
        FALLING
    );
}

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

#ifdef USAR_MOSFET

    if (pulsos > 0) {
        registrarFluxoDetectado();
    }

#endif

    interrupts();

    // YF-S201: F(Hz) = 7.5 * Q(L/min) => Q = pulsos/s / fator_calibracao
    fluxo = calcularFluxoLMin(pulsos, fator_calibracao);

    volume = calcularVolumeLitros(pulsos, fator_calibracao);

    if (releLigado) {
        volume_total += volume;
    }

    if (volume_total < 0) {
        volume_total = 0;
    }

    if (fluxo < 0) {
        fluxo = 0;
    }

    ultimoCalculoFluxo = millis();
}
