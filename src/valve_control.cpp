#include <Arduino.h>

#include "valve_control.h"
#include "config.h"
#include "globals.h"


static bool estadoValvulaAtual = false;

#ifdef USAR_MOSFET

bool picoAtivo = false;


unsigned long inicioPico = 0;


#endif

void iniciarValvula()
{
    pinMode(PINO_RELE, OUTPUT);

#ifdef USAR_MOSFET

    analogWriteFreq(1000);
    analogWriteRange(1023);
    analogWrite(PINO_RELE, 0);

#else

    digitalWrite(PINO_RELE, LOW);

#endif
}

void abrirValvula()
{
    estadoValvulaAtual = true;

#ifdef USAR_RELE

    digitalWrite(PINO_RELE, HIGH);

#endif

#ifdef USAR_MOSFET

    analogWrite(PINO_RELE, PWM_PICO);

    inicioPico = millis();

    picoAtivo = true;

#endif
}

void fecharValvula()
{
    estadoValvulaAtual = false;

#ifdef USAR_RELE

    digitalWrite(PINO_RELE, LOW);

#endif

#ifdef USAR_MOSFET

    analogWrite(PINO_RELE, 0);

#endif
}

void atualizarValvula()
{
#ifdef USAR_MOSFET

if (estadoValvulaAtual)
{
    if (picoAtivo)
    {
        if (millis() - inicioPico > TEMPO_PICO_MS)
        {
            analogWrite(
                PINO_RELE,
                pwmRetencao
            );

            picoAtivo = false;
        }
    }

    static unsigned long ultimoAjuste = 0;

    if (millis() - ultimoAjuste > 2000)
    {
        ultimoAjuste = millis();

        if (
            millis() - ultimoFluxoDetectado
            > 3000
        )
        {
            if (pwmRetencao < 700)
            {
                pwmRetencao += 20;

                analogWrite(
                    PINO_RELE,
                    pwmRetencao
                );
            }
        }
    }
}

#endif
}

bool valvulaAberta()
{
    return estadoValvulaAtual;
}

void registrarFluxoDetectado()
{
    ultimoFluxoDetectado = millis();
}