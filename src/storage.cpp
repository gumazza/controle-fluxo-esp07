#include <Arduino.h>

#include <EEPROM.h>

#include <math.h>

#include "config.h"
#include "globals.h"
#include "fill_logic.h"

struct Configuracoes {

    uint32_t magic;

    uint8_t version;

    float volumeLimite;

    float fatorCalibracao;

    unsigned long timer;

    unsigned long totalCiclos;

    float litrosAcumulados;

    uint16_t pwm_retencao;

    unsigned long timeoutSemFluxo;

    unsigned long timeoutEnchimento;
};

static Configuracoes config;

static void aplicarDefaults() {

    config.magic = EEPROM_MAGIC;

    config.version = EEPROM_VERSION;

    config.volumeLimite = 0;

    config.fatorCalibracao = FATOR_CALIB_DEFAULT;

    config.timer = 0;

    config.totalCiclos = 0;

    config.litrosAcumulados = 0;

    config.pwm_retencao = PWM_RETENCAO;

    config.timeoutSemFluxo = TEMPO_SEM_FLUXO;

    config.timeoutEnchimento = TEMPO_MAX_ENCHIMENTO;
}

static void restaurarGlobals() {

    volume_limite = quantizarSetpointVolume(config.volumeLimite);

    fator_calibracao = config.fatorCalibracao;

    tempoTimer = config.timer;

    totalCiclos = config.totalCiclos;

    litrosAcumulados = config.litrosAcumulados;

    pwmRetencao = config.pwm_retencao;

    timeoutSemFluxo = config.timeoutSemFluxo;

    timeoutEnchimento = config.timeoutEnchimento;
}

static bool validarConfig(const Configuracoes& cfg) {

    if (
        isnan(cfg.volumeLimite) ||
        cfg.volumeLimite < 0 ||
        cfg.volumeLimite > 999
    ) {
        return false;
    }

    if (
        isnan(cfg.fatorCalibracao) ||
        cfg.fatorCalibracao < 1 ||
        cfg.fatorCalibracao > 20
    ) {
        return false;
    }

    if (cfg.timer > 86400) {
        return false;
    }

    if (
        cfg.pwm_retencao < 200 ||
        cfg.pwm_retencao > 800
    ) {
        return false;
    }

    if (
        cfg.timeoutSemFluxo < 1000 ||
        cfg.timeoutSemFluxo > 60000
    ) {
        return false;
    }

    if (
        cfg.timeoutEnchimento < 60000 ||
        cfg.timeoutEnchimento > 7200000
    ) {
        return false;
    }

    return true;
}

void iniciarEEPROM() {

    EEPROM.begin(EEPROM_SIZE);
}

void salvarConfiguracoes() {

    config.magic = EEPROM_MAGIC;

    config.version = EEPROM_VERSION;

    config.volumeLimite = volume_limite;

    config.fatorCalibracao = fator_calibracao;

    config.timer = tempoTimer;

    config.totalCiclos = totalCiclos;

    config.litrosAcumulados = litrosAcumulados;

    config.pwm_retencao = pwmRetencao;

    config.timeoutSemFluxo = timeoutSemFluxo;

    config.timeoutEnchimento = timeoutEnchimento;

    EEPROM.put(0, config);

    EEPROM.commit();

    ultimoSaveEEPROM = millis();
}

void carregarConfiguracoes() {

    EEPROM.get(0, config);

    if (
        config.magic != EEPROM_MAGIC ||
        config.version != EEPROM_VERSION ||
        !validarConfig(config)
    ) {

        aplicarDefaults();

        restaurarGlobals();

        salvarConfiguracoes();
    }
    else {
        restaurarGlobals();
    }
}

void verificarSalvarConfiguracoes() {

    static float ultimoVolumeLimite = -1;

    static float ultimoFator = -1;

    static unsigned long ultimoTimerSalvo = 0;

    static uint16_t ultimoPwm = 0;

    static unsigned long ultimoTimeoutFluxo = 0;

    static unsigned long ultimoTimeoutEnchimento = 0;

    if (
        millis() - ultimoSaveEEPROM
            < DEBOUNCE_EEPROM_MS
    ) {
        return;
    }

    bool salvar = false;

    if (
        abs(volume_limite - ultimoVolumeLimite)
            > 0.001
    ) {

        ultimoVolumeLimite = volume_limite;

        salvar = true;
    }

    if (abs(fator_calibracao - ultimoFator) > 0.001) {

        ultimoFator = fator_calibracao;

        salvar = true;
    }

    if (tempoTimer != ultimoTimerSalvo) {

        ultimoTimerSalvo = tempoTimer;

        salvar = true;
    }

    if (pwmRetencao != ultimoPwm) {

        ultimoPwm = pwmRetencao;

        salvar = true;
    }

    if (timeoutSemFluxo != ultimoTimeoutFluxo) {

        ultimoTimeoutFluxo = timeoutSemFluxo;

        salvar = true;
    }

    if (timeoutEnchimento != ultimoTimeoutEnchimento) {

        ultimoTimeoutEnchimento = timeoutEnchimento;

        salvar = true;
    }

    if (salvar) {
        salvarConfiguracoes();
    }
}
