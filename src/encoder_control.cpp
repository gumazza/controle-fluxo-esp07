#include <Arduino.h>

#include <AiEsp32RotaryEncoder.h>

#include "config.h"
#include "globals.h"
#include "lcd_display.h"
#include "timer_control.h"
#include "backend_menu.h"

// =========================
// ENCODER
// =========================

AiEsp32RotaryEncoder encoder(
    ENCODER_PINO_A,
    ENCODER_PINO_B,
    ENCODER_PINO_BTN,
    -1,
    4
);

// =========================
// CONTROLE BOTAO
// =========================

bool ultimoEstadoBotao = HIGH;

unsigned long tempoPressionado = 0;

unsigned long ultimoClique = 0;

bool aguardandoDuploClique = false;

// =========================
// FUNCOES
// =========================

float obterPassoVolume(float valor) {

    if (valor < 1.0)
        return 0.010;

    if (valor < 5.0)
        return 0.100;

    if (valor < 10.0)
        return 0.500;

    return 1.000;
}

// =========================
// INICIAR
// =========================

void iniciarEncoder() {

    encoder.begin();

    encoder.setup(
        [] { encoder.readEncoder_ISR(); }
    );

    encoder.setBoundaries(
        -100000,
        100000,
        false
    );

    encoder.setAcceleration(200);
}

// =========================
// ROTACAO
// =========================

void atualizarEncoder() {

    if (!encoder.encoderChanged())
        return;

    ligarBacklight();

    float passo = obterPassoVolume(volume_limite);

    int valor = encoder.readEncoder();

    if (
    estadoSistema == TIMER_OFF ||
    estadoSistema == TIMER_ON ||
    estadoSistema == TIMER_PAUSA
) {

    int valor = encoder.readEncoder();

    if (valor > 0)
        incrementarTimer(1);

    else
        incrementarTimer(-1);

    encoder.setEncoderValue(0);

    return;
}

    if (valor > 0) {

        volume_limite += passo;
    }
    else {

        volume_limite -= passo;
    }

    if (volume_limite < 0)
        volume_limite = 0;

    encoder.setEncoderValue(0);
}

// =========================
// CLIQUE SIMPLES
// =========================

void cliqueSimples() {

    ligarBacklight();

    if (estadoSistema == BACKEND_MENU) {

    backendClick();

    return;
}

    if (estadoSistema == TIMER_OFF) {

    if (tempoTimer > 0) {

        estadoSistema = TIMER_ON;

        tempoRestanteTimer = tempoTimer;

        timerRodando = true;
    }

    return;
}

if (estadoSistema == TIMER_ON) {

    estadoSistema = TIMER_PAUSA;

    timerPausado = true;

    return;
}

if (estadoSistema == TIMER_PAUSA) {

    estadoSistema = TIMER_ON;

    timerPausado = false;

    return;
}

    if (estadoSistema == ERRO) {

    erroSemFluxo = false;

    erroTimeout = false;

    estadoSistema = STANDBY;

    return;
}

    switch (estadoSistema) {

        // =========================
        // INICIAR ENCHIMENTO
        // =========================

        case STANDBY:

            releLigado = true;

            modoAutomatico = true;

            estadoSistema = ENCHENDO;

            break;

        // =========================
        // PAUSAR
        // =========================

        case ENCHENDO:

            releLigado = false;

            estadoSistema = PAUSADO;

            break;

        // =========================
        // RETOMAR
        // =========================

        case PAUSADO:

            if (volume_total < volume_limite) {

                releLigado = true;

                estadoSistema = ENCHENDO;
            }

            break;

        default:
            break;
    }
}

// =========================
// DUPLO CLIQUE
// =========================

void duploClique() {

    ligarBacklight();

    if (estadoSistema == BACKEND_MENU) {

    backendDoubleClick();

    return;
}

    // =========================
    // RESET SETPOINT
    // =========================

    if (volume_limite > 0) {

        volume_limite = 0;

        return;
    }

    // =========================
    // RESET TOTAL
    // =========================

    volume_total = 0;
}

// =========================
// LONG PRESS
// =========================

void longPress() {

    ligarBacklight();

    if (estadoSistema == BACKEND_MENU) {

    backendLongPress();

    return;
}

    // =========================
    // SAIR TIMER
    // =========================

    if (
        estadoSistema == TIMER_ON ||
        estadoSistema == TIMER_PAUSA
    ) {

        timerRodando = false;

        timerPausado = false;

        estadoSistema = STANDBY;

        return;
    }

    // =========================
    // ENTRAR TIMER
    // =========================

    estadoSistema = TIMER_OFF;
}

// =========================
// BOTAO
// =========================

void controlarBotaoEncoder() {

    bool estadoBotao =
        digitalRead(ENCODER_PINO_BTN);

    static unsigned long backendPress = 0;

if (
    digitalRead(PINO_BOTAO_MANUAL) == LOW &&
    digitalRead(ENCODER_PINO_BTN) == LOW
) {

    if (backendPress == 0)
        backendPress = millis();

    if (
        millis() - backendPress
            > 5000
    ) {

        entrarBackend();

        backendPress = 0;
    }
}
else {

    backendPress = 0;
}

    // =========================
    // PRESSIONADO
    // =========================

    if (
        ultimoEstadoBotao == HIGH &&
        estadoBotao == LOW
    ) {

        tempoPressionado = millis();
    }

    // =========================
    // SOLTO
    // =========================

    if (
        ultimoEstadoBotao == LOW &&
        estadoBotao == HIGH
    ) {

        unsigned long tempoClique =
            millis() - tempoPressionado;

        // =========================
        // LONG PRESS
        // =========================

        if (tempoClique >= TEMPO_LONG_PRESS) {

            longPress();
        }

        // =========================
        // CLIQUE
        // =========================

        else {

            if (
                aguardandoDuploClique &&
                millis() - ultimoClique
                    <= TEMPO_DUPLO_CLIQUE
            ) {

                duploClique();

                aguardandoDuploClique = false;
            }
            else {

                aguardandoDuploClique = true;

                ultimoClique = millis();
            }
        }
    }

    // =========================
    // CLIQUE SIMPLES TIMEOUT
    // =========================

    if (
        aguardandoDuploClique &&
        millis() - ultimoClique
            > TEMPO_DUPLO_CLIQUE
    ) {

        cliqueSimples();

        aguardandoDuploClique = false;
    }

    ultimoEstadoBotao = estadoBotao;
}