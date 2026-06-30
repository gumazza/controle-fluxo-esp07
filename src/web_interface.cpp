#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <ESP.h>

#include "config.h"
#include "globals.h"
#include "storage.h"
#include "wifi_control.h"
#include "display_format.h"
#include "encoder_control.h"
#include "lcd_display.h"
#include "timer_control.h"
#include "web_index.h"
#include "web_logo.h"
#include "ota_update.h"

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

static bool webServerIniciado = false;

static bool rotasRegistradas = false;

static bool requerAuthRede(AsyncWebServerRequest *request) {

    if (request->authenticate(WEB_REDE_USER, WEB_REDE_PASSWORD)) {
        return true;
    }

    request->requestAuthentication("REDE");

    return false;
}

static void adicionarInfoWifi(JsonDocument &doc) {

    char ssid[33];
    char ipSta[16];
    char ipAp[16];
    int rssi = 0;

    obterInfoWifi(
        ssid,
        sizeof(ssid),
        ipSta,
        sizeof(ipSta),
        ipAp,
        sizeof(ipAp),
        &rssi
    );

    doc["wifi_connected"] = wifiConectado();
    doc["wifi_ssid"] = ssid;
    doc["wifi_ip"] = ipSta;
    doc["ap_ip"] = ipAp;
    doc["wifi_rssi"] = rssi;
}

static void adicionarValoresFormatados(JsonDocument &doc) {

    char volumeTxt[10];
    char setpointTxt[10];
    char fluxoTxt[10];

    formatarLitros(volumeTxt, sizeof(volumeTxt), volume_total);

    formatarLitros(setpointTxt, sizeof(setpointTxt), volume_limite);

    formatarFluxo(fluxoTxt, sizeof(fluxoTxt), fluxo);

    doc["volume_txt"] = volumeTxt;
    doc["setpoint_txt"] = setpointTxt;
    doc["fluxo_txt"] = fluxoTxt;
}

static void adicionarConfigJson(JsonDocument &doc) {

    doc["fator_calibracao"] = fator_calibracao;
    doc["timeout_sem_fluxo"] = timeoutSemFluxo;
    doc["timeout_enchimento"] = timeoutEnchimento;
    doc["total_ciclos"] = totalCiclos;
    doc["litros_acumulados"] = litrosAcumulados;
}

static bool validarConfigWeb(
    float calib,
    unsigned long wdFluxo,
    unsigned long wdEnc
) {

    if (calib < 1.0f || calib > 20.0f) {
        return false;
    }

    if (wdFluxo < 1000UL || wdFluxo > 60000UL) {
        return false;
    }

    if (wdEnc < 60000UL || wdEnc > 7200000UL) {
        return false;
    }

    return true;
}

static void obterEstadoWeb(char *buf, size_t len) {

    switch (estadoSistema) {

        case STANDBY:
            snprintf(buf, len, "STANDBY");
            break;

        case ENCHENDO:
            snprintf(buf, len, "ENCHENDO");
            break;

        case PAUSADO:
            snprintf(buf, len, "PAUSADO");
            break;

        case TIMER_OFF:
            snprintf(buf, len, "TIMER");
            break;

        case TIMER_ON:
            snprintf(buf, len, "TIMER ON");
            break;

        case TIMER_PAUSA:
            snprintf(buf, len, "TIMER PAUSA");
            break;

        case ERRO:

            if (erroSemFluxo) {
                snprintf(buf, len, "ERRO SEM FLUXO");
            }
            else if (erroTimeout) {
                snprintf(buf, len, "ERRO TIMEOUT");
            }
            else {
                snprintf(buf, len, "ERRO");
            }

            break;

        case BACKEND_MENU:
            snprintf(buf, len, "BACKEND");
            break;

        default:
            snprintf(buf, len, "—");
            break;
    }
}

static void registrarRotas() {

    if (rotasRegistradas) {
        return;
    }

    server.on("/", HTTP_GET,
        [](AsyncWebServerRequest *request) {

            request->send_P(200, "text/html", INDEX_HTML);
        });

    server.on("/logo.png", HTTP_GET,
        [](AsyncWebServerRequest *request) {

            AsyncWebServerResponse *response = request->beginResponse_P(
                200,
                "image/png",
                LOGO_MAZZA_PNG,
                LOGO_MAZZA_PNG_LEN
            );

            response->addHeader("Cache-Control", "no-cache");

            request->send(response);
        });

    server.on("/api/wifi/status", HTTP_GET,
        [](AsyncWebServerRequest *request) {

            if (!requerAuthRede(request)) {
                return;
            }

            JsonDocument doc;

            adicionarInfoWifi(doc);

            char json[192];

            serializeJson(doc, json, sizeof(json));

            request->send(200, "application/json", json);
        });

    server.on("/api/wifi/scan", HTTP_GET,
        [](AsyncWebServerRequest *request) {

            if (!requerAuthRede(request)) {
                return;
            }

            JsonDocument doc;

            ScanWifiEstado estado = obterEstadoScanWifi();

            if (
                estado == SCAN_WIFI_IDLE ||
                estado == SCAN_WIFI_ERRO
            ) {

                iniciarScanWifi();

                estado = obterEstadoScanWifi();
            }

            if (estado == SCAN_WIFI_RODANDO) {

                doc["status"] = "scanning";
            }
            else if (estado == SCAN_WIFI_ERRO) {

                doc["status"] = "error";

                limparScanWifi();
            }
            else if (estado == SCAN_WIFI_PRONTO) {

                doc["status"] = "done";

                JsonArray redes = doc["networks"].to<JsonArray>();

                adicionarRedesScan(redes, 15);

                limparScanWifi();
            }
            else {

                doc["status"] = "scanning";
            }

            char json[768];

            serializeJson(doc, json, sizeof(json));

            request->send(200, "application/json", json);
        });

    server.on(
        "/api/wifi/connect",
        HTTP_POST,
        [](AsyncWebServerRequest *request) {

            if (!requerAuthRede(request)) {
                return;
            }
        },
        NULL,
        [](AsyncWebServerRequest *request,
           uint8_t *data,
           size_t len,
           size_t index,
           size_t total) {

            static String corpo;

            if (index == 0) {

                if (!requerAuthRede(request)) {
                    return;
                }

                corpo = "";
            }

            for (size_t i = 0; i < len; i++) {
                corpo += (char)data[i];
            }

            if (index + len < total) {
                return;
            }

            JsonDocument entrada;

            JsonDocument resposta;

            if (deserializeJson(entrada, corpo)) {

                resposta["ok"] = false;
                resposta["error"] = "JSON invalido";
            }
            else {

                const char *ssid = entrada["ssid"];

                const char *senha = entrada["password"] | "";

                if (ssid == nullptr || ssid[0] == '\0') {

                    resposta["ok"] = false;
                    resposta["error"] = "SSID obrigatorio";
                }
                else {

                    resposta["ok"] = conectarWifi(ssid, senha);
                }
            }

            char json[128];

            serializeJson(resposta, json, sizeof(json));

            request->send(200, "application/json", json);
        });

    server.on("/api/controle/clique", HTTP_POST,
        [](AsyncWebServerRequest *request) {

            cliqueEncoder();

            ligarBacklight();

            request->send(200, "application/json", "{\"ok\":true}");
        });

    server.on("/api/controle/zerar-volume", HTTP_POST,
        [](AsyncWebServerRequest *request) {

            zerarVolumeTotal();

            ligarBacklight();

            request->send(200, "application/json", "{\"ok\":true}");
        });

    server.on(
        "/api/controle/setpoint",
        HTTP_POST,
        [](AsyncWebServerRequest *request) {
        },
        NULL,
        [](AsyncWebServerRequest *request,
           uint8_t *data,
           size_t len,
           size_t index,
           size_t total) {

            static String corpoSetpoint;

            if (index == 0) {
                corpoSetpoint = "";
            }

            for (size_t i = 0; i < len; i++) {
                corpoSetpoint += (char)data[i];
            }

            if (index + len < total) {
                return;
            }

            JsonDocument entrada;

            JsonDocument resposta;

            if (deserializeJson(entrada, corpoSetpoint)) {

                resposta["ok"] = false;
                resposta["error"] = "JSON invalido";
            }
            else {

                int direcao = entrada["direcao"] | 0;

                if (direcao == 0) {

                    resposta["ok"] = false;
                    resposta["error"] = "Direcao invalida";
                }
                else {

                    ajustarSetpoint(direcao > 0 ? 1 : -1);

                    ligarBacklight();

                    resposta["ok"] = true;
                }
            }

            char json[128];

            serializeJson(resposta, json, sizeof(json));

            request->send(200, "application/json", json);
        });

    server.on(
        "/api/timer/ajustar",
        HTTP_POST,
        [](AsyncWebServerRequest *request) {
        },
        NULL,
        [](AsyncWebServerRequest *request,
           uint8_t *data,
           size_t len,
           size_t index,
           size_t total) {

            static String corpoTimer;

            if (index == 0) {
                corpoTimer = "";
            }

            for (size_t i = 0; i < len; i++) {
                corpoTimer += (char)data[i];
            }

            if (index + len < total) {
                return;
            }

            JsonDocument entrada;

            JsonDocument resposta;

            if (deserializeJson(entrada, corpoTimer)) {

                resposta["ok"] = false;
                resposta["error"] = "JSON invalido";
            }
            else {

                int direcao = entrada["direcao"] | 0;

                if (direcao == 0) {

                    resposta["ok"] = false;
                    resposta["error"] = "Direcao invalida";
                }
                else {

                    ajustarTempoTimer(direcao > 0 ? 1 : -1);

                    resposta["ok"] = true;
                }
            }

            char json[128];

            serializeJson(resposta, json, sizeof(json));

            request->send(200, "application/json", json);
        });

    server.on("/api/timer/acao", HTTP_POST,
        [](AsyncWebServerRequest *request) {

            JsonDocument resposta;

            if (acaoTimerWeb()) {

                resposta["ok"] = true;
            }
            else {

                resposta["ok"] = false;
                resposta["error"] = "Timer indisponivel";
            }

            char json[128];

            serializeJson(resposta, json, sizeof(json));

            request->send(200, "application/json", json);
        });

    server.on("/api/timer/cancelar", HTTP_POST,
        [](AsyncWebServerRequest *request) {

            cancelarTimer();

            request->send(200, "application/json", "{\"ok\":true}");
        });

    server.on("/api/config", HTTP_GET,
        [](AsyncWebServerRequest *request) {

            JsonDocument doc;

            adicionarConfigJson(doc);

            char json[192];

            serializeJson(doc, json, sizeof(json));

            request->send(200, "application/json", json);
        });

    server.on(
        "/api/config",
        HTTP_POST,
        [](AsyncWebServerRequest *request) {
        },
        NULL,
        [](AsyncWebServerRequest *request,
           uint8_t *data,
           size_t len,
           size_t index,
           size_t total) {

            static String corpoConfig;

            if (index == 0) {
                corpoConfig = "";
            }

            for (size_t i = 0; i < len; i++) {
                corpoConfig += (char)data[i];
            }

            if (index + len < total) {
                return;
            }

            JsonDocument entrada;

            JsonDocument resposta;

            if (deserializeJson(entrada, corpoConfig)) {

                resposta["ok"] = false;
                resposta["error"] = "JSON invalido";
            }
            else {

                float calib = entrada["fator_calibracao"] | fator_calibracao;

                unsigned long wdFluxo =
                    entrada["timeout_sem_fluxo"] | timeoutSemFluxo;

                unsigned long wdEnc =
                    entrada["timeout_enchimento"] | timeoutEnchimento;

                if (!validarConfigWeb(calib, wdFluxo, wdEnc)) {

                    resposta["ok"] = false;
                    resposta["error"] = "Valores invalidos";
                }
                else {

                    fator_calibracao = calib;

                    timeoutSemFluxo = wdFluxo;

                    timeoutEnchimento = wdEnc;

                    salvarConfiguracoes();

                    resposta["ok"] = true;
                }
            }

            char json[128];

            serializeJson(resposta, json, sizeof(json));

            request->send(200, "application/json", json);
        });

    server.on("/api/config/reset-stats", HTTP_POST,
        [](AsyncWebServerRequest *request) {

            litrosAcumulados = 0;

            totalCiclos = 0;

            salvarConfiguracoes();

            request->send(
                200,
                "application/json",
                "{\"ok\":true,\"message\":\"Estatisticas zeradas\"}"
            );
        });

    server.on("/api/config/reset-wifi", HTTP_POST,
        [](AsyncWebServerRequest *request) {

            request->send(
                200,
                "application/json",
                "{\"ok\":true,\"message\":\"Reiniciando...\"}"
            );

            resetarWifi();

            delay(200);

            ESP.restart();
        });

    server.on("/api/config/reboot", HTTP_POST,
        [](AsyncWebServerRequest *request) {

            request->send(
                200,
                "application/json",
                "{\"ok\":true,\"message\":\"Reiniciando...\"}"
            );

            delay(200);

            ESP.restart();
        });

    ws.onEvent(
        [](AsyncWebSocket *server,
           AsyncWebSocketClient *client,
           AwsEventType type,
           void *arg,
           uint8_t *data,
           size_t len) {
        });

    server.addHandler(&ws);

    rotasRegistradas = true;
}

void iniciarWebServer() {

    registrarRotas();

    if (webServerIniciado) {
        return;
    }

    iniciarOTA();

    server.begin();

    webServerIniciado = true;
}

void pararWebServer() {

    if (!webServerIniciado) {
        return;
    }

    server.end();

    webServerIniciado = false;
}

void atualizarWebSocket() {

    if (!redeAtiva() || otaEmProgresso()) {
        return;
    }

    static unsigned long ultimoEnvio = 0;

    if (millis() - ultimoEnvio < INTERVALO_WEBSOCKET) {
        return;
    }

    ultimoEnvio = millis();

    if (ws.count() == 0) {
        return;
    }

    char estado[20];

    obterEstadoWeb(estado, sizeof(estado));

    JsonDocument doc;

    doc["fluxo"] = fluxo;
    doc["volume"] = volume_total;
    doc["setpoint"] = volume_limite;
    doc["estado"] = estado;
    doc["valvula"] = releLigado;

    adicionarValoresFormatados(doc);

    adicionarInfoWifi(doc);

    doc["timer_modo"] = emModoTimer();
    doc["timer_estado"] = obterTextoTimer();
    doc["timer_restante"] = tempoRestanteTimer;
    doc["timer_configurado"] = tempoTimer;

    char json[400];

    serializeJson(doc, json, sizeof(json));

    ws.textAll(json);
}
