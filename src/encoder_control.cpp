#include <Arduino.h>

#include <AiEsp32RotaryEncoder.h>

#include "config.h"
#include "globals.h"
#include "fill_logic.h"
#include "lcd_display.h"
#include "timer_control.h"
#include "backend_menu.h"
#include "valve_control.h"
#include "watchdogs.h"

AiEsp32RotaryEncoder encoder(
    ENCODER_PINO_A,
    ENCODER_PINO_B,
    ENCODER_PINO_BTN,
    -1,
    4
);

bool ultimoEstadoBotao = HIGH;

unsigned long tempoPressionado = 0;

unsigned long ultimoClique = 0;

bool aguardandoDuploClique = false;

static void sairConfigVolume() {

    configVolumeAtivo = false;

    marcarLcdSujo();
}

static void entrarConfigVolume() {

    configVolumeSelecao = 0;

    configVolumeAtivo = true;

    marcarLcdSujo();
}

static void processarRotacaoConfigVolume(int direcao) {

    if (direcao > 0) {
        configVolumeSelecao = 1;
    }
    else {
        configVolumeSelecao = 0;
    }
}

static void zerarVolumeSelecionado() {

    if (configVolumeSelecao == 0) {
        volume_limite = 0;
    }
    else {
        volume_total = 0;
    }
}

static unsigned long ultimoPulsoVolume = 0;

static void ajustarVolumeLimite(int direcao, unsigned long intervaloMs) {

    volume_limite = aplicarIncrementoSetpoint(
        volume_limite,
        calcularIncrementoVolume(
            volume_limite,
            direcao,
            intervaloMs
        )
    );

    marcarLcdSujo();
}

void iniciarEncoder() {

    pinMode(ENCODER_PINO_A, INPUT_PULLUP);

    pinMode(ENCODER_PINO_B, INPUT_PULLUP);

    pinMode(ENCODER_PINO_BTN, INPUT);
}

void atualizarEncoder() {

    static int ultimoCLK = HIGH;

    static unsigned long ultimoPulso = 0;

    int clk = digitalRead(ENCODER_PINO_A);

    if (ultimoCLK == HIGH && clk == LOW) {

        if (millis() - ultimoPulso < DEBOUNCE_ENCODER_MS) {

            ultimoCLK = clk;

            return;
        }

        ultimoPulso = millis();

        ligarBacklight();

        int direcao;

        if (digitalRead(ENCODER_PINO_B) != clk) {
            direcao = 1;
        }
        else {
            direcao = -1;
        }

        if (
            estadoSistema == TIMER_OFF ||
            estadoSistema == TIMER_ON ||
            estadoSistema == TIMER_PAUSA
        ) {
            incrementarTimer(direcao);

            ultimoCLK = clk;

            return;
        }

        if (estadoSistema == BACKEND_MENU) {

            processarRotacaoBackend(direcao);

            ultimoCLK = clk;

            return;
        }

        if (configVolumeAtivo) {

            processarRotacaoConfigVolume(direcao);

            ultimoCLK = clk;

            return;
        }

        if (
            estadoSistema != STANDBY &&
            estadoSistema != ENCHENDO &&
            estadoSistema != PAUSADO
        ) {

            ultimoCLK = clk;

            return;
        }

        unsigned long agora = millis();

        unsigned long intervalo = ENCODER_INTERVALO_MAX_MS;

        if (ultimoPulsoVolume > 0) {

            intervalo = agora - ultimoPulsoVolume;

            if (intervalo > ENCODER_INTERVALO_MAX_MS) {
                intervalo = ENCODER_INTERVALO_MAX_MS;
            }
        }

        ultimoPulsoVolume = agora;

        ajustarVolumeLimite(direcao, intervalo);
    }

    ultimoCLK = clk;
}

void cliqueSimples() {

    Serial.println("CLIQUE");

    ligarBacklight();

    if (configVolumeAtivo) {

        zerarVolumeSelecionado();

        sairConfigVolume();

        return;
    }

    if (estadoSistema == BACKEND_MENU) {

        backendClick();

        return;
    }

    if (estadoSistema == TIMER_OFF) {

        iniciarContagemTimer();

        return;
    }

    if (estadoSistema == TIMER_ON) {

        pausarContagemTimer();

        return;
    }

    if (estadoSistema == TIMER_PAUSA) {

        retomarContagemTimer();

        return;
    }

    if (estadoSistema == ERRO) {

        sairErro();

        return;
    }

    switch (estadoSistema) {

        case STANDBY:

            iniciarEnchimento();

            break;

        case ENCHENDO:

            releLigado = false;

            estadoSistema = PAUSADO;

            break;

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

void duploClique() {

    Serial.println("DUPLO");

    ligarBacklight();

    if (estadoSistema == BACKEND_MENU) {

        backendDoubleClick();

        return;
    }

    if (configVolumeAtivo) {

        sairConfigVolume();

        return;
    }

    if (
        estadoSistema == STANDBY ||
        estadoSistema == ENCHENDO ||
        estadoSistema == PAUSADO
    ) {

        entrarConfigVolume();

        return;
    }
}

void longPress() {

    Serial.println("LONG");

    ligarBacklight();

    if (configVolumeAtivo) {
        return;
    }

    if (estadoSistema == BACKEND_MENU) {

        backendLongPress();

        return;
    }

    if (
        estadoSistema == STANDBY ||
        estadoSistema == ERRO
    ) {

        entrarBackend();

        return;
    }

    if (
        estadoSistema == TIMER_OFF ||
        estadoSistema == TIMER_ON ||
        estadoSistema == TIMER_PAUSA
    ) {

        cancelarTimer();

        return;
    }
}

void controlarBotaoEncoder() {

    bool estadoBotao = digitalRead(ENCODER_PINO_BTN);

    static unsigned long timerComboPress = 0;

    static bool longPressJaTratado = false;

    if (
        digitalRead(PINO_BOTAO_MANUAL) == LOW &&
        estadoBotao == LOW &&
        estadoSistema == STANDBY
    ) {
        if (timerComboPress == 0) {
            timerComboPress = millis();
        }

        if (millis() - timerComboPress >= TEMPO_LONG_PRESS) {

            entrarModoTimer();

            longPressJaTratado = true;

            timerComboPress = 0;
        }
    }
    else {
        timerComboPress = 0;
    }

    if (
        ultimoEstadoBotao == HIGH &&
        estadoBotao == LOW
    ) {
        tempoPressionado = millis();

        longPressJaTratado = false;
    }

    if (
        ultimoEstadoBotao == LOW &&
        estadoBotao == HIGH
    ) {
        unsigned long tempoClique =
            millis() - tempoPressionado;

        if (
            tempoClique >= TEMPO_LONG_PRESS &&
            !longPressJaTratado
        ) {
            longPress();
        }
        else if (tempoClique < TEMPO_LONG_PRESS) {

            if (
                aguardandoDuploClique &&
                millis() - ultimoClique <= TEMPO_DUPLO_CLIQUE
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

    if (
        aguardandoDuploClique &&
        millis() - ultimoClique > TEMPO_DUPLO_CLIQUE
    ) {
        cliqueSimples();

        aguardandoDuploClique = false;
    }

    ultimoEstadoBotao = estadoBotao;
}

void cliqueEncoder() {

    cliqueSimples();
}

void zerarVolumeTotal() {

    volume_total = 0;

    ligarBacklight();

    marcarLcdSujo();
}

void ajustarSetpoint(int direcao) {

    if (configVolumeAtivo) {
        return;
    }

    if (
        estadoSistema != STANDBY &&
        estadoSistema != ENCHENDO &&
        estadoSistema != PAUSADO
    ) {
        return;
    }

    ajustarVolumeLimite(direcao, ENCODER_INTERVALO_MAX_MS);

    ligarBacklight();
}
