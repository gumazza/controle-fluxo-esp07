#pragma once

// =========================
// PINOS
// =========================

// SENSOR FLUXO
#define PINO_SENSOR            2

// SAIDAS
#define PINO_RELE              13
#define PINO_OPTO              15

// ENCODER
#define ENCODER_PINO_A         12
#define ENCODER_PINO_B         14
#define ENCODER_PINO_BTN       16

// BOTAO MANUAL
#define PINO_BOTAO_MANUAL      0

// I2C LCD
#define LCD_SDA                4
#define LCD_SCL                5

// =========================
// LCD
// =========================

#define LCD_COLUNAS            16
#define LCD_LINHAS             2

#define TEMPO_LCD              120000UL
#define INTERVALO_LCD          400UL

// =========================
// BOTOES
// =========================

#define TEMPO_LONG_PRESS       3000UL
#define TEMPO_DUPLO_CLIQUE     400UL

// =========================
// SEGURANCA
// =========================

#define TEMPO_SEM_FLUXO        5000UL

#define TEMPO_MAX_ENCHIMENTO   1200000UL

// =========================
// FLUXO
// =========================

#define INTERVALO_FLUXO        1000UL

// =========================
// EEPROM
// =========================

#define EEPROM_SIZE            512

// =========================
// WIFI
// =========================

#define WIFI_AP_SSID           "controle_fluxo"