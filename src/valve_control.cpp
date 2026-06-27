#include <Arduino.h>

#include "valve_control.h"
#include "config.h"
#include "globals.h"
#include "system_state.h"
#include "fill_logic.h"
#include "lcd_display.h"
#include "storage.h"
#include "watchdogs.h"

static bool estadoValvulaAtual = false;

#ifdef USAR_MOSFET

static bool picoAtivo = false;
static unsigned long inicioPico = 0;

#endif

static void enviarPulsoOpto() {

    digitalWrite(PINO_OPTO, HIGH);

    tempoPulsoOpto = millis();

    pulsoOptoEnviado = true;
}

static void registrarEnchimento(float litros) {

    totalCiclos++;

    litrosAcumulados += litros;

    salvarConfiguracoes();
}

static void sincronizarValvula() {

    if (releLigado && !estadoValvulaAtual) {
        abrirValvula();
    }
    else if (!releLigado && estadoValvulaAtual) {
        fecharValvula();
    }
}

static void controlarBotaoManual() {

    if (
        estadoSistema == ENCHENDO ||
        estadoSistema == PAUSADO
    ) {
        return;
    }

    if (digitalRead(PINO_BOTAO_MANUAL) == LOW) {

        ligarBacklight();

        if (limiteVolumeAtingido(volume_total, volume_limite)) {

            releLigado = false;

            return;
        }

        modoManual = true;

        releLigado = true;

        return;
    }

    if (modoManual) {

        modoManual = false;

        releLigado = false;
    }
}

void iniciarValvula() {

    pinMode(PINO_RELE, OUTPUT);

    pinMode(PINO_OPTO, OUTPUT);

    digitalWrite(PINO_OPTO, LOW);

    pinMode(PINO_BOTAO_MANUAL, INPUT_PULLUP);

#ifdef USAR_MOSFET

    analogWriteFreq(1000);
    analogWriteRange(1023);
    analogWrite(PINO_RELE, 0);

#else

    digitalWrite(PINO_RELE, LOW);

#endif
}

void abrirValvula() {

    estadoValvulaAtual = true;

#ifdef USAR_RELE

    digitalWrite(PINO_RELE, HIGH);

#endif

#ifdef USAR_MOSFET

    analogWrite(PINO_RELE, PWM_PICO);

    inicioPico = millis();

    picoAtivo = true;

#endif
}

void fecharValvula() {

    estadoValvulaAtual = false;

#ifdef USAR_RELE

    digitalWrite(PINO_RELE, LOW);

#endif

#ifdef USAR_MOSFET

    analogWrite(PINO_RELE, 0);

    picoAtivo = false;

#endif
}

bool iniciarEnchimento() {

    if (!podeIniciarEnchimento(volume_limite)) {
        return false;
    }

    volume_total = 0;

    marcarInicioEnchimento();

    releLigado = true;

    modoAutomatico = true;

    modoManual = false;

    estadoSistema = ENCHENDO;

    return true;
}

void atualizarValvula() {

#ifdef USAR_MOSFET

    if (estadoValvulaAtual) {

        if (picoAtivo) {

            if (millis() - inicioPico > TEMPO_PICO_MS) {

                analogWrite(PINO_RELE, pwmRetencao);

                picoAtivo = false;
            }
        }

        static unsigned long ultimoAjuste = 0;

        if (millis() - ultimoAjuste > INTERVALO_AJUSTE_PWM) {

            ultimoAjuste = millis();

            if (
                millis() - ultimoFluxoDetectado
                    > TEMPO_SEM_FLUXO_PWM
            ) {

                if (pwmRetencao < PWM_RETENCAO_MAX) {

                    pwmRetencao += PWM_RETENCAO_STEP;

                    analogWrite(PINO_RELE, pwmRetencao);
                }
            }
        }
    }

#endif
}

void atualizarAtuadores() {

    controlarBotaoManual();

    if (
        releLigado &&
        limiteVolumeAtingido(volume_total, volume_limite)
    ) {

        releLigado = false;

        fecharValvula();

        estadoSistema = STANDBY;

        if (!modoManual) {
            enviarPulsoOpto();
        }

        registrarEnchimento(volume_total);

        return;
    }

    sincronizarValvula();

    atualizarValvula();

    if (
        pulsoOptoEnviado &&
        millis() - tempoPulsoOpto >= TEMPO_PULSO_OPTO
    ) {

        digitalWrite(PINO_OPTO, LOW);

        pulsoOptoEnviado = false;
    }
}

bool valvulaAberta() {

    return estadoValvulaAtual;
}

void registrarFluxoDetectado() {

    ultimoFluxoDetectado = millis();
}
