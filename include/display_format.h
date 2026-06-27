#pragma once

#include <stdio.h>

inline void formatarLitros(char *buffer, size_t size, float valor) {

    if (valor < 1.0f) {
        snprintf(buffer, size, "%1.3f", valor);
    }
    else {
        snprintf(buffer, size, "%05.1f", valor);
    }
}

inline void formatarFluxo(char *buffer, size_t size, float valor) {

    snprintf(buffer, size, "%05.1f", valor);
}
