#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "wifi_control.h"
#include "config.h"
#include "lcd_display.h"
#include "web_interface.h"
#include "ota_update.h"

static bool servicosRedeIniciados = false;

static unsigned long ultimaTentativaWifi = 0;

static unsigned long bootMs = 0;

static bool staIniciado = false;

static ScanWifiEstado scanEstado = SCAN_WIFI_IDLE;

static int scanTotal = 0;

static IPAddress ipAp() {

    return IPAddress(WIFI_AP_IP);
}

static bool apAtivo() {

    return WiFi.softAPIP() != IPAddress(0, 0, 0, 0);
}

static bool temCredenciaisSalvas() {

    return WiFi.SSID().length() > 0;
}

static void aplicarModoWifi() {

    if (temCredenciaisSalvas()) {
        WiFi.mode(WIFI_AP_STA);
    }
    else {
        WiFi.mode(WIFI_AP);
    }
}

static void configurarSoftAP() {

    IPAddress ip = ipAp();

    WiFi.softAPConfig(ip, ip, IPAddress(255, 255, 255, 0));

    WiFi.softAP(
        WIFI_AP_SSID,
        NULL,
        WIFI_AP_CHANNEL,
        false,
        4
    );
}

static void garantirSoftAP() {

    if (apAtivo()) {
        return;
    }

    aplicarModoWifi();

    configurarSoftAP();
}

static void iniciarServicosRede() {

    if (servicosRedeIniciados) {
        return;
    }

    iniciarWebServer();

    iniciarOTA();

    servicosRedeIniciados = true;

    marcarLcdSujo();
}

void restaurarSoftAP() {

    aplicarModoWifi();

    configurarSoftAP();
}

void iniciarWifi() {

    bootMs = millis();

    WiFi.persistent(true);

    WiFi.setAutoReconnect(false);

    WiFi.setSleepMode(WIFI_NONE_SLEEP);

    WiFi.setOutputPower(20.5f);

    WiFi.hostname(WIFI_AP_SSID);

    WiFi.mode(WIFI_OFF);

    delay(100);

    aplicarModoWifi();

    configurarSoftAP();

    delay(300);

    iniciarServicosRede();

    Serial.print(F("AP: "));

    Serial.print(WIFI_AP_SSID);

    Serial.print(F(" @ "));

    Serial.println(WiFi.softAPIP());
}

void atualizarWifi() {

    atualizarScanWifi();

    garantirSoftAP();

    if (WiFi.status() == WL_CONNECTED) {
        return;
    }

    if (!temCredenciaisSalvas()) {
        return;
    }

    if (!staIniciado && millis() - bootMs >= 2500UL) {

        WiFi.begin();

        staIniciado = true;

        ultimaTentativaWifi = millis();

        return;
    }

    if (
        staIniciado &&
        millis() - ultimaTentativaWifi >= 60000UL
    ) {

        WiFi.begin();

        ultimaTentativaWifi = millis();
    }
}

bool conectarWifi(const char *ssid, const char *password) {

    if (ssid == nullptr || ssid[0] == '\0') {
        return false;
    }

    WiFi.persistent(true);

    WiFi.mode(WIFI_AP_STA);

    configurarSoftAP();

    if (password != nullptr && password[0] != '\0') {
        WiFi.begin(ssid, password);
    }
    else {
        WiFi.begin(ssid);
    }

    staIniciado = true;

    ultimaTentativaWifi = millis();

    return true;
}

bool wifiConectado() {

    return WiFi.status() == WL_CONNECTED;
}

bool redeAtiva() {

    return apAtivo() || wifiConectado();
}

bool servicosRedeAtivos() {

    return servicosRedeIniciados;
}

void obterInfoWifi(
    char *ssid,
    size_t ssidLen,
    char *ipSta,
    size_t ipStaLen,
    char *ipAp,
    size_t ipApLen,
    int *rssi
) {

    String rede = WiFi.SSID();

    strncpy(ssid, rede.c_str(), ssidLen - 1);

    ssid[ssidLen - 1] = '\0';

    if (WiFi.status() == WL_CONNECTED) {

        String ip = WiFi.localIP().toString();

        strncpy(ipSta, ip.c_str(), ipStaLen - 1);

        ipSta[ipStaLen - 1] = '\0';

        if (rssi != nullptr) {
            *rssi = WiFi.RSSI();
        }
    }
    else {

        ipSta[0] = '\0';

        if (rssi != nullptr) {
            *rssi = 0;
        }
    }

    String ap = WiFi.softAPIP().toString();

    strncpy(ipAp, ap.c_str(), ipApLen - 1);

    ipAp[ipApLen - 1] = '\0';
}

void resetarWifi() {

    limparScanWifi();

    WiFi.disconnect(true);

    WiFi.persistent(true);

    staIniciado = false;

    WiFi.mode(WIFI_AP);

    configurarSoftAP();
}

void iniciarScanWifi() {

    if (scanEstado == SCAN_WIFI_RODANDO) {
        return;
    }

    limparScanWifi();

    WiFi.mode(WIFI_AP_STA);

    configurarSoftAP();

    WiFi.scanDelete();

    int resultado = WiFi.scanNetworks(true, true);

    if (resultado == WIFI_SCAN_RUNNING) {

        scanEstado = SCAN_WIFI_RODANDO;
    }
    else if (resultado >= 0) {

        scanTotal = resultado;

        scanEstado = SCAN_WIFI_PRONTO;
    }
    else {

        scanEstado = SCAN_WIFI_ERRO;

        restaurarSoftAP();
    }
}

void atualizarScanWifi() {

    if (scanEstado != SCAN_WIFI_RODANDO) {
        return;
    }

    int resultado = WiFi.scanComplete();

    if (resultado == WIFI_SCAN_RUNNING) {
        return;
    }

    if (resultado >= 0) {

        scanTotal = resultado;

        scanEstado = SCAN_WIFI_PRONTO;
    }
    else {

        scanEstado = SCAN_WIFI_ERRO;
    }

    if (temCredenciaisSalvas()) {
        WiFi.mode(WIFI_AP_STA);
    }
    else {
        WiFi.mode(WIFI_AP);
    }

    configurarSoftAP();
}

ScanWifiEstado obterEstadoScanWifi() {

    return scanEstado;
}

void adicionarRedesScan(JsonArray &redes, int limite) {

    if (scanEstado != SCAN_WIFI_PRONTO || scanTotal <= 0) {
        return;
    }

    int total = scanTotal;

    if (total > limite) {
        total = limite;
    }

    for (int i = 0; i < total; i++) {

        if (WiFi.SSID(i).length() == 0) {
            continue;
        }

        JsonObject rede = redes.add<JsonObject>();

        rede["ssid"] = WiFi.SSID(i);
        rede["rssi"] = WiFi.RSSI(i);
        rede["secure"] = WiFi.encryptionType(i) != ENC_TYPE_NONE;
    }
}

void limparScanWifi() {

    WiFi.scanDelete();

    scanEstado = SCAN_WIFI_IDLE;

    scanTotal = 0;
}
