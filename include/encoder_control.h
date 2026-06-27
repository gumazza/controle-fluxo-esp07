#pragma once

#include <AiEsp32RotaryEncoder.h>

extern AiEsp32RotaryEncoder encoder;

void iniciarEncoder();

void atualizarEncoder();

void controlarBotaoEncoder();

void cliqueEncoder();

void zerarVolumeTotal();

void ajustarSetpoint(int direcao);