#include <Arduino.h>

#include "config.h"
#include "globals.h"
#include "system_state.h"
#include "storage.h"
#include "lcd_display.h"
#include "flow_sensor.h"
#include "encoder_control.h"
#include "valve_control.h"
#include "timer_control.h"
#include "watchdogs.h"
#include "backend.h"
#include "backend_menu.h"
#include "wifi_control.h"
#include "web_interface.h"
#include "ota_update.h"

float fluxo = 0;
float volume = 0;
float volume_total = 0;
float volume_limite = 1.0;
float fator_calibracao = FATOR_CALIB_DEFAULT;

int backendIndex = 0;

bool releLigado = false;
bool modoAutomatico = false;
bool erroSemFluxo = false;
bool erroTimeout = false;
bool timerAtivo = false;
bool timerRodando = false;
bool timerPausado = false;
bool modoManual = false;
bool pulsoOptoEnviado = false;
bool backendEditando = false;
bool backendConfirmando = false;

bool configVolumeAtivo = false;

uint8_t configVolumeSelecao = 0;

unsigned long tempoPulsoOpto = 0;
unsigned long tempoTimer = 0;
unsigned long tempoRestanteTimer = 0;
unsigned long totalCiclos = 0;
unsigned long ultimoSaveEEPROM = 0;

unsigned long timeoutSemFluxo = TEMPO_SEM_FLUXO;
unsigned long timeoutEnchimento = TEMPO_MAX_ENCHIMENTO;

float litrosAcumulados = 0;

uint16_t pwmRetencao = PWM_RETENCAO;

EstadoSistema estadoSistema = STANDBY;

void setup() {

    Serial.begin(115200);

    Wire.begin(LCD_SDA, LCD_SCL);

    iniciarLCD();

    iniciarEEPROM();

    carregarConfiguracoes();

    iniciarSensorFluxo();

    iniciarEncoder();

    iniciarValvula();

    verificarModoBackend();

    iniciarWifi();

    marcarLcdSujo();
}

void loop() {

    atualizarFluxo();

    atualizarEncoder();

    controlarBotaoEncoder();

    atualizarTimer();

    verificarWatchdogs();

    atualizarAtuadores();

    atualizarWifi();

    atualizarLCD();

    atualizarWebSocket();

    if (servicosRedeAtivos()) {
        atualizarOTA();
    }

    verificarSalvarConfiguracoes();

    yield();
}
