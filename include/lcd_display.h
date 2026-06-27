#pragma once

#include <LiquidCrystal_I2C.h>

extern LiquidCrystal_I2C lcd;

void iniciarLCD();

void atualizarLCD();

void ligarBacklight();

void desligarBacklight();

void marcarLcdSujo();

void telaPrincipal();