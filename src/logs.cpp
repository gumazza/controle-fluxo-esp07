#include "globals.h"
#include "storage.h"

void registrarEnchimento(float litros) {

    totalCiclos++;

    litrosAcumulados += litros;

    salvarConfiguracoes();
}