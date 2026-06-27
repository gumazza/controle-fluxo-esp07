#include <Arduino.h>

#include <ESP8266WiFi.h>

#include <WiFiManager.h>

#include "wifi_control.h"
#include "config.h"
#include "lcd_display.h"
#include "web_interface.h"
#include "ota_update.h"

static WiFiManager wifiManager;

static bool servicosRedeIniciados = false;

static bool portalAtivo = false;

static bool portalIniciado = false;

static unsigned long ultimaTentativaWifi = 0;

static IPAddress ipAp() {

    return IPAddress(WIFI_AP_IP);
}

static bool apAtivo() {

    return (
        (WiFi.getMode() & WIFI_AP) &&
        WiFi.softAPIP() != IPAddress(0, 0, 0, 0)
    );
}

static void garantirSoftAP() {

    if (apAtivo() && WiFi.softAPSSID() == String(WIFI_AP_SSID)) {
        return;
    }

    WiFi.softAPdisconnect(true);

    IPAddress ip = ipAp();

    WiFi.softAPConfig(ip, ip, IPAddress(255, 255, 255, 0));

    WiFi.softAP(WIFI_AP_SSID, "");
}

static void pararPortal() {

    if (portalIniciado) {

        wifiManager.stopConfigPortal();

        portalIniciado = false;
    }

    portalAtivo = false;
}

static void pararServicosRede() {

    if (!servicosRedeIniciados) {
        return;
    }

    pararWebServer();

    servicosRedeIniciados = false;
}

static void iniciarServicosRede() {

    if (servicosRedeIniciados || portalAtivo) {
        return;
    }

    iniciarWebServer();

    iniciarOTA();

    servicosRedeIniciados = true;

    marcarLcdSujo();
}

static void reiniciarServicosRede() {

    pararServicosRede();

    iniciarServicosRede();
}

static void configurarWifiManager() {

    wifiManager.setDebugOutput(false);

    wifiManager.setConnectTimeout(WIFI_CONNECT_TIMEOUT);

    wifiManager.setConfigPortalTimeout(WIFI_PORTAL_TIMEOUT);

    wifiManager.setConfigPortalBlocking(false);

    wifiManager.setBreakAfterConfig(true);

    wifiManager.setCaptivePortalEnable(true);
}

void iniciarPortalConfiguracao() {

    pararServicosRede();

    configurarWifiManager();

    garantirSoftAP();

    if (!portalIniciado) {

        wifiManager.startConfigPortal(WIFI_AP_SSID, "");

        portalIniciado = true;
    }

    portalAtivo = true;
}

void iniciarWifi() {

    WiFi.persistent(true);

    WiFi.mode(WIFI_AP_STA);

    WiFi.setAutoReconnect(true);

    WiFi.hostname(WIFI_AP_SSID);

    garantirSoftAP();

    if (WiFi.SSID().length() == 0) {

        iniciarPortalConfiguracao();

        return;
    }

    WiFi.begin();

    ultimaTentativaWifi = millis();
}

void atualizarWifi() {

    if (portalAtivo) {

        if (!wifiManager.process()) {

            portalIniciado = false;

            portalAtivo = false;
        }

        if (WiFi.status() == WL_CONNECTED) {

            pararPortal();

            reiniciarServicosRede();
        }

        return;
    }

    garantirSoftAP();

    if (WiFi.status() == WL_CONNECTED) {

        iniciarServicosRede();

        return;
    }

    iniciarPortalConfiguracao();

    if (
        WiFi.SSID().length() > 0 &&
        millis() - ultimaTentativaWifi >= 30000UL
    ) {

        WiFi.reconnect();

        ultimaTentativaWifi = millis();
    }
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

bool portalConfiguracaoAtivo() {

    return portalAtivo;
}

void resetarWifi() {

    pararServicosRede();

    pararPortal();

    wifiManager.resetSettings();
}
