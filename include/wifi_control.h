#pragma once

#include <stddef.h>

#include <ArduinoJson.h>

enum ScanWifiEstado {
    SCAN_WIFI_IDLE = 0,
    SCAN_WIFI_RODANDO,
    SCAN_WIFI_PRONTO,
    SCAN_WIFI_ERRO
};

void iniciarWifi();

void atualizarWifi();

void resetarWifi();

bool conectarWifi(const char *ssid, const char *password);

bool wifiConectado();

bool redeAtiva();

bool servicosRedeAtivos();

void obterInfoWifi(
    char *ssid,
    size_t ssidLen,
    char *ipSta,
    size_t ipStaLen,
    char *ipAp,
    size_t ipApLen,
    int *rssi
);

void restaurarSoftAP();

void iniciarScanWifi();

void atualizarScanWifi();

ScanWifiEstado obterEstadoScanWifi();

void adicionarRedesScan(JsonArray &redes, int limite);

void limparScanWifi();
