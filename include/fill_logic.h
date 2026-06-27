#pragma once

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

inline float obterPassoVolume(float valor) {
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
