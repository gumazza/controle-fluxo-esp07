#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>

// =========================
// HEADERS
// =========================

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
#include "ota_update.h"
#include "web_interface.h"
#include "logs.h"

// =========================
// GLOBAIS
// =========================

float fluxo = 0;
float volume = 0;
float volume_total = 0;
float volume_limite = 1.0;
float fator_calibracao = 4.5;

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

unsigned long tempoPulsoOpto = 0;
unsigned long tempoTimer = 0;
unsigned long tempoRestanteTimer = 0;
unsigned long totalCiclos = 0;
unsigned long ultimoSaveEEPROM = 0;

unsigned long timeoutSemFluxo = TEMPO_SEM_FLUXO;
unsigned long timeoutEnchimento = TEMPO_MAX_ENCHIMENTO;

float litrosAcumulados = 0;

uint16_t pwmRetencao = 350;

EstadoSistema estadoSistema = STANDBY;

// =========================
// SETUP
// =========================

void setup()
{
    Serial.begin(115200);

    // =========================
    // LCD
    // =========================

    Wire.begin(LCD_SDA, LCD_SCL);

    iniciarLCD();

    // =========================
    // EEPROM
    // =========================

    iniciarEEPROM();

    carregarConfiguracoes();

    // =========================
    // HARDWARE
    // =========================

    iniciarSensorFluxo();

    iniciarEncoder();

    iniciarValvula();

    // =========================
    // BACKEND
    // =========================

    verificarModoBackend();

    // =========================
    // WIFI
    // =========================

    WiFi.mode(WIFI_AP_STA);

    WiFiManager wm;

    wm.setDebugOutput(false);

    wm.autoConnect("controle_fluxo");

    // =========================
    // WEB
    // =========================

    iniciarWebServer();

    // =========================
    // OTA
    // =========================

    iniciarOTA();

}

// =========================
// LOOP
// =========================

void loop() {

    atualizarFluxo();

    atualizarEncoder();

    controlarBotaoEncoder();

    atualizarTimer();

    //atualizarRele();

    verificarWatchdogs();

    atualizarLCD();

    atualizarValvula();

    atualizarWebSocket();

    verificarSalvarConfiguracoes();
    
}