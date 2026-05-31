#pragma once

#include "system_state.h"

// =========================
// FLUXO
// =========================

extern float fluxo;
extern float volume;
extern float volume_total;
extern float volume_limite;
extern float fator_calibracao;

// =========================
// ESTADOS
// =========================

extern bool releLigado;
extern bool modoAutomatico;
extern bool erroSemFluxo;
extern bool erroTimeout;
extern bool modoManual;
extern bool pulsoOptoEnviado;
extern unsigned long tempoPulsoOpto;

// =========================
// TIMER
// =========================

extern bool timerAtivo;
extern bool timerRodando;
extern bool timerPausado;
extern unsigned long tempoTimer;
extern unsigned long tempoRestanteTimer;
extern unsigned long timeoutSemFluxo;
extern unsigned long timeoutEnchimento;

// =========================
// ESTATISTICAS
// =========================

extern unsigned long totalCiclos;
extern float litrosAcumulados;

// =========================
// SISTEMA
// =========================

extern EstadoSistema estadoSistema;

// =========================
// EEPROM
// =========================

extern unsigned long ultimoSaveEEPROM;

// =========================
// BACKEND
// =========================

extern int backendIndex;
extern bool backendEditando;
extern bool backendConfirmando;

