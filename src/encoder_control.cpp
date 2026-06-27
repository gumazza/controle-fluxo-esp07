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

void iniciarEncoder()
{
    pinMode(
        ENCODER_PINO_A,
        INPUT_PULLUP
    );

    pinMode(
        ENCODER_PINO_B,
        INPUT_PULLUP
    );

    pinMode(
        ENCODER_PINO_BTN,
        INPUT
    );
}

// =========================
// ROTACAO
// =========================

void atualizarEncoder()
{
    static int ultimoCLK = HIGH;
    static unsigned long ultimoPulso = 0;

    int clk = digitalRead(ENCODER_PINO_A);

    if (ultimoCLK == HIGH && clk == LOW)
    {
        if (millis() - ultimoPulso < 5)
        {
            ultimoCLK = clk;
            return;
        }

        ultimoPulso = millis();

        ligarBacklight();

        int direcao;

        if (digitalRead(ENCODER_PINO_B) != clk)
            direcao = 1;
        else
            direcao = -1;

        // =====================
        // TIMER
        // =====================

        if (
            estadoSistema == TIMER_OFF ||
            estadoSistema == TIMER_ON ||
            estadoSistema == TIMER_PAUSA
        )
        {
            incrementarTimer(direcao);

            ultimoCLK = clk;
            return;
        }

        // =====================
        // BACKEND
        // =====================

        if (estadoSistema == BACKEND_MENU)
        {
            ultimoCLK = clk;
            return;
        }

        // =====================
        // VOLUME
        // =====================

        float passo =
            obterPassoVolume(volume_limite);

        volume_limite +=
            direcao * passo;

        if (volume_limite < 0)
            volume_limite = 0;
    }

    ultimoCLK = clk;
}

// =========================
// CLIQUE SIMPLES
// =========================

void cliqueSimples() {

    Serial.println("CLIQUE");

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

    Serial.println("DUPLO");

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

    Serial.println("LONG");
    
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

void controlarBotaoEncoder()
{
    bool estadoBotao =
        digitalRead(ENCODER_PINO_BTN);

    static unsigned long backendPress = 0;

    // =========================
    // ENTRAR BACKEND
    // =========================

    if (
        digitalRead(PINO_BOTAO_MANUAL) == LOW &&
        estadoBotao == LOW
    )
    {
        if (backendPress == 0)
            backendPress = millis();

        if (
            millis() - backendPress >= 5000
        )
        {
            entrarBackend();

            backendPress = 0;
        }
    }
    else
    {
        backendPress = 0;
    }

    // =========================
    // BOTAO PRESSIONADO
    // =========================

    if (
        ultimoEstadoBotao == HIGH &&
        estadoBotao == LOW
    )
    {
        tempoPressionado = millis();
    }

    // =========================
    // BOTAO SOLTO
    // =========================

    if (
        ultimoEstadoBotao == LOW &&
        estadoBotao == HIGH
    )
    {
        unsigned long tempoClique =
            millis() - tempoPressionado;

        // =====================
        // LONG PRESS
        // =====================

        if (
            tempoClique >= TEMPO_LONG_PRESS
        )
        {
            longPress();
        }
        else
        {
            // =================
            // DUPLO CLIQUE
            // =================

            if (
                aguardandoDuploClique &&
                millis() - ultimoClique <=
                TEMPO_DUPLO_CLIQUE
            )
            {
                duploClique();

                aguardandoDuploClique = false;
            }
            else
            {
                aguardandoDuploClique = true;

                ultimoClique = millis();
            }
        }
    }

    // =========================
    // CLIQUE SIMPLES
    // =========================

    if (
        aguardandoDuploClique &&
        millis() - ultimoClique >
        TEMPO_DUPLO_CLIQUE
    )
    {
        cliqueSimples();

        aguardandoDuploClique = false;
    }

    ultimoEstadoBotao = estadoBotao;
}