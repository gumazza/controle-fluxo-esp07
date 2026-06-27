#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#include "lcd_display.h"
#include "globals.h"
#include "config.h"
#include "system_state.h"
#include "timer_control.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

// =========================
// CONTROLE LCD
// =========================

bool lcdLigado = true;

unsigned long ultimoLCD = 0;

unsigned long ultimaInteracaoLCD = 0;

// =========================
// FORMATADORES
// =========================

static void formatarLitros(char* buffer, size_t size, float valor) {

    if (valor < 1.0f) {
        snprintf(buffer, size, "%1.3f", valor);
    }
    else {
        snprintf(buffer, size, "%05.1f", valor);
    }
}

static void formatarFluxo(char* buffer, size_t size, float valor) {

    snprintf(buffer, size, "%05.1f", valor);
}

// =========================
// LCD
// =========================

void iniciarLCD() {

    lcd.init();

    lcd.backlight();

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Controle Volume");

    lcd.setCursor(0, 1);
    lcd.print("Inicializando");

    delay(500);

    marcarLcdSujo();

    ultimaInteracaoLCD = millis();
}

void ligarBacklight() {

    lcd.backlight();

    lcdLigado = true;

    ultimaInteracaoLCD = millis();
}

void desligarBacklight() {

    lcd.noBacklight();

    lcdLigado = false;
}

void marcarLcdSujo() {

    ultimoLCD = 0;
}

static void escreverLinhaLcd(uint8_t linha, const char* texto) {

    char buffer[LCD_COLUNAS + 1];

    snprintf(
        buffer,
        sizeof(buffer),
        "%-*s",
        LCD_COLUNAS,
        texto
    );

    lcd.setCursor(0, linha);

    lcd.print(buffer);
}

// =========================
// DISPLAY NORMAL
// =========================

void telaPrincipal() {

    char limite[10];
    char total[10];
    char vazao[10];
    char linha1[LCD_COLUNAS + 1];
    char linha2[LCD_COLUNAS + 1];
    char campoSet[10];
    char campoTot[10];

    bool piscaVisivel =
        (millis() / INTERVALO_PISCAR_LCD) % 2 == 0;

    formatarLitros(limite, sizeof(limite), volume_limite);

    formatarLitros(total, sizeof(total), volume_total);

    formatarFluxo(vazao, sizeof(vazao), fluxo);

    size_t lenSet = strlen(limite);

    size_t lenTot = strlen(total);

    if (configVolumeAtivo && configVolumeSelecao == 0 && !piscaVisivel) {

        memset(campoSet, ' ', lenSet);

        campoSet[lenSet] = '\0';
    }
    else {
        strncpy(campoSet, limite, sizeof(campoSet) - 1);
        campoSet[sizeof(campoSet) - 1] = '\0';
    }

    if (configVolumeAtivo && configVolumeSelecao == 1 && !piscaVisivel) {

        memset(campoTot, ' ', lenTot);

        campoTot[lenTot] = '\0';
    }
    else {
        strncpy(campoTot, total, sizeof(campoTot) - 1);
        campoTot[sizeof(campoTot) - 1] = '\0';
    }

    snprintf(
        linha1,
        sizeof(linha1),
        "%sL     L/min",
        campoSet
    );

    snprintf(
        linha2,
        sizeof(linha2),
        "%sL     %s",
        campoTot,
        vazao
    );

    escreverLinhaLcd(0, linha1);

    escreverLinhaLcd(1, linha2);
}

// =========================
// PAUSADO
// =========================

void telaPausado() {

    escreverLinhaLcd(0, "    PAUSADO");

    escreverLinhaLcd(1, "PRESSIONE BTN");
}

static void telaBackend() {

    char linha1[LCD_COLUNAS + 1];
    char linha2[LCD_COLUNAS + 1];

    switch (backendIndex) {

        case 0:
            snprintf(linha1, sizeof(linha1), "CALIB SENSOR");
            break;

        case 1:
            snprintf(linha1, sizeof(linha1), "RESET STATS");
            break;

        case 2:
            snprintf(linha1, sizeof(linha1), "RESET WIFI");
            break;

        case 3:
            snprintf(linha1, sizeof(linha1), "REBOOT ESP");
            break;

        case 4:
            snprintf(linha1, sizeof(linha1), "WD SEM FLUXO");
            break;

        case 5:
            snprintf(linha1, sizeof(linha1), "WD ENCHIMENTO");
            break;

        default:
            snprintf(linha1, sizeof(linha1), "BACKEND");
            break;
    }

    switch (backendIndex) {

        case 0:

            if (backendEditando) {
                snprintf(
                    linha2,
                    sizeof(linha2),
                    "F=%.1f EDIT",
                    fator_calibracao
                );
            }
            else {
                snprintf(
                    linha2,
                    sizeof(linha2),
                    "F=%.1f",
                    fator_calibracao
                );
            }

            break;

        case 1:

            if (backendConfirmando) {
                snprintf(linha2, sizeof(linha2), "CONFIRMAR?");
            }
            else {
                snprintf(linha2, sizeof(linha2), "2 CLICK CONFIRMA");
            }

            break;

        case 2:

            if (backendConfirmando) {
                snprintf(linha2, sizeof(linha2), "RESET WIFI?");
            }
            else {
                snprintf(linha2, sizeof(linha2), "2 CLICK CONFIRMA");
            }

            break;

        case 3:

            if (backendConfirmando) {
                snprintf(linha2, sizeof(linha2), "REINICIAR?");
            }
            else {
                snprintf(linha2, sizeof(linha2), "2 CLICK CONFIRMA");
            }

            break;

        case 4:

            if (backendEditando) {
                snprintf(
                    linha2,
                    sizeof(linha2),
                    "%lus EDIT",
                    timeoutSemFluxo / 1000
                );
            }
            else {
                snprintf(
                    linha2,
                    sizeof(linha2),
                    "%lu s",
                    timeoutSemFluxo / 1000
                );
            }

            break;

        case 5:

            if (backendEditando) {
                snprintf(
                    linha2,
                    sizeof(linha2),
                    "%lumin EDIT",
                    timeoutEnchimento / 60000
                );
            }
            else {
                snprintf(
                    linha2,
                    sizeof(linha2),
                    "%lu min",
                    timeoutEnchimento / 60000
                );
            }

            break;

        default:
            snprintf(linha2, sizeof(linha2), "");
            break;
    }

    escreverLinhaLcd(0, linha1);

    escreverLinhaLcd(1, linha2);
}

// =========================
// ERRO
// =========================

void telaErro(
    const char* linha1,
    const char* linha2
) {

    lcd.setCursor(0, 0);
    lcd.print("                ");

    lcd.setCursor(0, 1);
    lcd.print("                ");

    lcd.setCursor(0, 0);
    lcd.print(linha1);

    lcd.setCursor(0, 1);
    lcd.print(linha2);
}

// =========================
// UPDATE LCD
// =========================

void atualizarLCD() {

    unsigned long intervalo =
        configVolumeAtivo ? INTERVALO_PISCAR_LCD : INTERVALO_LCD;

    if (millis() - ultimoLCD < intervalo) {
        return;
    }

    ultimoLCD = millis();

    // =========================
    // BACKLIGHT AUTO OFF
    // =========================

    if (
        lcdLigado &&
        millis() - ultimaInteracaoLCD >= TEMPO_LCD
    ) {

        desligarBacklight();
    }

    // =========================
    // ESTADOS
    // =========================

    switch (estadoSistema) {

        case STANDBY:
        case ENCHENDO:

            telaPrincipal();

            break;

        case PAUSADO:

            if (configVolumeAtivo) {
                telaPrincipal();
            }
            else {
                telaPausado();
            }

            break;

        case TIMER_OFF:
        case TIMER_ON:
        case TIMER_PAUSA: {

            char linha1[LCD_COLUNAS + 1];
            char linha2[LCD_COLUNAS + 1];

            snprintf(
                linha1,
                sizeof(linha1),
                "   TIMER %s",
                obterTextoTimer()
            );

            if (tempoRestanteTimer >= 3600) {

                float horas = tempoRestanteTimer / 3600.0f;

                snprintf(
                    linha2,
                    sizeof(linha2),
                    "      %4.1fhrs",
                    horas
                );
            }
            else {

                snprintf(
                    linha2,
                    sizeof(linha2),
                    "      %4lus",
                    tempoRestanteTimer
                );
            }

            escreverLinhaLcd(0, linha1);

            escreverLinhaLcd(1, linha2);

            break;
        }

        case ERRO: {

            if (erroSemFluxo) {
                escreverLinhaLcd(0, " ERRO SEM FLUX");
            }
            else if (erroTimeout) {
                escreverLinhaLcd(0, "    TIMEOUT");
            }
            else {
                escreverLinhaLcd(0, " ERRO SISTEMA");
            }

            escreverLinhaLcd(1, "Pressione BTN");

            break;
        }

        case BACKEND_MENU:

            telaBackend();

            break;
    }
}