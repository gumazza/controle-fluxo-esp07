#include <Arduino.h>
#include <stdio.h>

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

String formatarLitros(float valor) {

    char buffer[10];

    if (valor < 1.0) {

        sprintf(buffer, "%1.3f", valor);
    }
    else {

        sprintf(buffer, "%05.1f", valor);
    }

    return String(buffer);
}

String formatarFluxo(float valor) {

    char buffer[10];

    sprintf(buffer, "%05.1f", valor);

    return String(buffer);
}

// =========================
// LCD
// =========================

void iniciarLCD() {

    lcd.init();

    lcd.backlight();

    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Controle Fluxo");

    lcd.setCursor(0, 1);
    lcd.print("Inicializando");

    delay(1500);

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

// =========================
// DISPLAY NORMAL
// =========================

void telaPrincipal() {

    String limite = formatarLitros(volume_limite);

    String total = formatarLitros(volume_total);

    String vazao = formatarFluxo(fluxo);

    char linha1[17];
    char linha2[17];

    snprintf(
        linha1,
        sizeof(linha1),
        "%-5sL     L/min",
        limite.c_str()
    );

    snprintf(
        linha2,
        sizeof(linha2),
        "%-5sL     %s",
        total.c_str(),
        vazao.c_str()
    );

    lcd.setCursor(0,0);
    lcd.print(linha1);

    lcd.setCursor(0,1);
    lcd.print(linha2);

}

// =========================
// TIMER
// =========================

void telaTimer() {

    lcd.setCursor(0, 0);
    lcd.print("   TIMER   ON ");

    lcd.setCursor(0, 1);
    lcd.print("      60 seg.");
}

// =========================
// PAUSADO
// =========================

void telaPausado() {

    lcd.setCursor(0, 0);
    lcd.print("    PAUSADO    ");

    lcd.setCursor(0, 1);
    lcd.print("PRESSIONE BTN ");
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

    if (millis() - ultimoLCD < INTERVALO_LCD)
        return;

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

            telaPrincipal();

            break;

        case ENCHENDO:

            telaPrincipal();

            break;

        case PAUSADO:

            telaPausado();

            break;

        case TIMER_OFF:
        case TIMER_ON:
        case TIMER_PAUSA: {

            lcd.setCursor(0, 0);

            lcd.print("   TIMER ");

            lcd.print(obterTextoTimer());

            lcd.setCursor(0, 1);

            char tempoStr[17];

        if (tempoRestanteTimer >= 3600) {

            float horas =
                tempoRestanteTimer / 3600.0;

        sprintf(
            tempoStr,
            "      %4.1fhrs",
            horas
        );
        }
        else {

        sprintf(
            tempoStr,
            "      %4lus",
            tempoRestanteTimer
        );
        }

            lcd.print(tempoStr);

            break;
    }

        case ERRO:

            lcd.setCursor(0, 0);

            if (erroSemFluxo) {

            lcd.print(" ERRO SEM FLUX  ");
            }
            else if (erroTimeout) {

            lcd.print("     TIMEOUT    ");
            }
            else {

            lcd.print("  ERRO SISTEMA  ");
             }

            lcd.setCursor(0, 1);

            lcd.print("Pressione BTN   ");

            break;

        case BACKEND_MENU: {

            lcd.setCursor(0, 0);

        switch (backendIndex) {

            case 0:
                lcd.print("CALIB SENSOR ");
                break;

            case 1:
                lcd.print("RESET STATS ");
                break;

            case 2:
                lcd.print("RESET WIFI  ");
                break;

            case 3:
                lcd.print("REBOOT ESP  ");
                break;

            case 4:
                lcd.print("WD SEMFLUXO ");
                break;

            case 5:
                lcd.print("WD ENCHIME. ");
                break;
        }

            lcd.setCursor(0, 1);

        switch (backendIndex) {

            case 0:

                lcd.print("F=");
                lcd.print(fator_calibracao, 1);

                if (backendEditando)
                    lcd.print(" EDIT");

                break;

            case 1:

                if (backendConfirmando)
                    lcd.print("CONFIRMAR?");
                else
                    lcd.print("2CLICK CONF");

                break;

            case 2:

                if (backendConfirmando)
                    lcd.print("RESET WIFI?");
                else
                    lcd.print("2CLICK CONF");

                break;

            case 3:

                if (backendConfirmando)
                    lcd.print("REINICIAR?");
                else
                    lcd.print("2CLICK CONF");

                break;

            case 4:

                lcd.print(timeoutSemFluxo / 1000);
                lcd.print("s");

                if (backendEditando)
                    lcd.print(" EDIT");

                break;

            case 5:

                lcd.print(timeoutEnchimento / 60000);
                lcd.print("min");

                if (backendEditando)
                    lcd.print(" EDIT");

                break;
        }

        break;
    }
    }
}