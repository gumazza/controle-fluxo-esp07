#pragma once

// =========================
// PINOS
// =========================

#define PINO_SENSOR            2
#define PINO_RELE              13
#define PINO_OPTO              15

#define ENCODER_PINO_A         12
#define ENCODER_PINO_B         14
#define ENCODER_PINO_BTN       16

#define PINO_BOTAO_MANUAL      0

#define LCD_SDA                4
#define LCD_SCL                5

// =========================
// LCD
// =========================

#define LCD_COLUNAS            16
#define LCD_LINHAS             2

#define TEMPO_LCD              120000UL
#define INTERVALO_LCD          400UL
#define INTERVALO_PISCAR_LCD   600UL

// =========================
// BOTOES
// =========================

#define TEMPO_LONG_PRESS       2000UL
#define TEMPO_DUPLO_CLIQUE     400UL
#define DEBOUNCE_ENCODER_MS    5UL
#define ENCODER_INTERVALO_MAX_MS 350UL

// =========================
// SEGURANCA
// =========================

#define TEMPO_SEM_FLUXO        5000UL
#define TEMPO_MAX_ENCHIMENTO   1200000UL
#define FLUXO_MINIMO_LMIN      0.05f
#define TEMPO_PULSO_OPTO       1000UL

// =========================
// FLUXO / CALIBRACAO
// =========================

#define INTERVALO_FLUXO        1000UL
#define FATOR_CALIB_DEFAULT    7.5f

// =========================
// EEPROM
// =========================

#define EEPROM_SIZE            512
#define EEPROM_MAGIC           0x56430F4CUL
#define EEPROM_VERSION         1

// =========================
// WIFI
// =========================

#define WIFI_AP_SSID           "controle_volume"
#define WIFI_AP_IP             192, 168, 4, 1
#define WIFI_AP_CHANNEL        6
#define WIFI_CONNECT_TIMEOUT   30UL

// =========================
// WEB
// =========================

#define INTERVALO_WEBSOCKET    500UL

// =========================
// PWM (MOSFET)
// =========================

#define PWM_PICO               1023
#define PWM_RETENCAO           450
#define TEMPO_PICO_MS          250UL
#define INTERVALO_AJUSTE_PWM   2000UL
#define TEMPO_SEM_FLUXO_PWM    3000UL
#define PWM_RETENCAO_MAX       700
#define PWM_RETENCAO_STEP      20

// =========================
// STORAGE
// =========================

#define DEBOUNCE_EEPROM_MS     3000UL
