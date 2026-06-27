#include <Arduino.h>

#include <EEPROM.h>

#include <math.h>

#include "config.h"
#include "globals.h"

// =========================
// ESTRUTURA
// =========================

struct Configuracoes {

    float volumeLimite;

    float fatorCalibracao;

    unsigned long timer;

    unsigned long totalCiclos;

    float litrosAcumulados;

    uint16_t pwm_retencao;
};

Configuracoes config;


// =========================
// INICIAR
// =========================

void iniciarEEPROM() {

    EEPROM.begin(EEPROM_SIZE);
}

// =========================
// CARREGAR
// =========================

void carregarConfiguracoes() {

    EEPROM.get(0, config);

    // =========================
    // VALIDACOES
    // =========================

    if (
        isnan(config.volumeLimite) ||
        config.volumeLimite < 0 ||
        config.volumeLimite > 999
    ) {

        config.volumeLimite = 0;
    }

    if (
        isnan(config.fatorCalibracao) ||
        config.fatorCalibracao < 1 ||
        config.fatorCalibracao > 20
    ) {

        config.fatorCalibracao = 7.5;
    }

    if (
        config.timer > 86400
    ) {

        config.timer = 0;
    }
    
    if (
        config.pwm_retencao < 200 ||
        config.pwm_retencao > 800
    ){
        config.pwm_retencao = 350;
    }

    // =========================
    // RESTAURA
    // =========================

    volume_limite =
        config.volumeLimite;

    fator_calibracao =
        config.fatorCalibracao;

    tempoTimer =
        config.timer;

    totalCiclos =
        config.totalCiclos;

    litrosAcumulados =
        config.litrosAcumulados;
        
    pwmRetencao =
        config.pwm_retencao;    
}

// =========================
// SALVAR
// =========================

void salvarConfiguracoes() {

    config.volumeLimite =
        volume_limite;

    config.fatorCalibracao =
        fator_calibracao;

    config.timer =
        tempoTimer;

    config.totalCiclos =
        totalCiclos;

    config.litrosAcumulados =
        litrosAcumulados;

    config.pwm_retencao =
        pwmRetencao;


    EEPROM.put(0, config);

    EEPROM.commit();

    ultimoSaveEEPROM = millis();
}

// =========================
// AUTO SAVE
// =========================

void verificarSalvarConfiguracoes() {

    static float ultimoVolumeLimite = -1;

    static unsigned long ultimoTimerSalvo = 0;

    // =========================
    // EVITA LOOP EEPROM
    // =========================

    if (
        millis() - ultimoSaveEEPROM
            < 3000
    ) {

        return;
    }

    bool salvar = false;

    // =========================
    // SETPOINT
    // =========================

    if (
        abs(
            volume_limite -
            ultimoVolumeLimite
        ) > 0.001
    ) {

        ultimoVolumeLimite =
            volume_limite;

        salvar = true;
    }

    // =========================
    // TIMER
    // =========================

    if (
        tempoTimer !=
        ultimoTimerSalvo
    ) {

        ultimoTimerSalvo =
            tempoTimer;

        salvar = true;
    }

    // =========================
    // SALVAR
    // =========================

    if (salvar) {

        salvarConfiguracoes();
    }
}