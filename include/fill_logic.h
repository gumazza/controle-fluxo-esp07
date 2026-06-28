#pragma once

#include <math.h>

// Funcoes puras testaveis (sem dependencia de hardware)

inline bool podeIniciarEnchimento(float volume_limite) {
    return volume_limite > 0.0f;
}

inline bool limiteVolumeAtingido(float volume_total, float volume_limite) {
    return volume_limite > 0.0f && volume_total >= volume_limite;
}

inline float calcularFluxoLMin(unsigned long pulsos, float fator_calibracao) {
    if (fator_calibracao <= 0.0f) {
        return 0.0f;
    }
    return (float)pulsos / fator_calibracao;
}

inline float calcularVolumeLitros(unsigned long pulsos, float fator_calibracao) {
    if (fator_calibracao <= 0.0f) {
        return 0.0f;
    }
    return (float)pulsos / fator_calibracao / 60.0f;
}

inline float obterPassoVolumeBase(float valor) {

    if (valor < 1.0f) {
        return 0.010f;
    }

    if (valor < 5.0f) {
        return 0.100f;
    }

    if (valor < 10.0f) {
        return 0.500f;
    }

    return 1.000f;
}

inline float obterPassoVolume(float valor) {
    return obterPassoVolumeBase(valor);
}

inline float obterResolucaoSetpoint(float valor) {

    return obterPassoVolumeBase(valor);
}

inline int obterMultiplicadorVelocidadeEncoder(unsigned long intervaloMs) {

    if (intervaloMs <= 30UL) {
        return 8;
    }

    if (intervaloMs <= 60UL) {
        return 5;
    }

    if (intervaloMs <= 120UL) {
        return 3;
    }

    if (intervaloMs <= 200UL) {
        return 2;
    }

    return 1;
}

inline float quantizarSetpointVolume(float valor) {

    if (valor < 0.0f) {
        return 0.0f;
    }

    float resolucao = obterResolucaoSetpoint(valor);

    return roundf(valor / resolucao) * resolucao;
}

inline float calcularIncrementoVolume(
    float valor,
    int direcao,
    unsigned long intervaloMs
) {

    if (direcao == 0) {
        return 0.0f;
    }

    unsigned long intervalo = intervaloMs;

    if (intervalo > 350UL) {
        intervalo = 350UL;
    }

    float passoBase = obterPassoVolumeBase(valor);

    int multiplicador = obterMultiplicadorVelocidadeEncoder(intervalo);

    int maxMultiplos = 5;

    if (multiplicador > maxMultiplos) {
        multiplicador = maxMultiplos;
    }

    float passo = passoBase * (float)multiplicador;

    return (direcao > 0 ? 1.0f : -1.0f) * passo;
}

inline float aplicarIncrementoSetpoint(
    float valor,
    float delta
) {

    if (delta == 0.0f) {
        return quantizarSetpointVolume(valor);
    }

    float novo = valor + delta;

    if (novo < 0.0f) {
        return 0.0f;
    }

    return quantizarSetpointVolume(novo);
}

inline float arredondarSetpointVolume(float valor) {

    return quantizarSetpointVolume(valor);
}
