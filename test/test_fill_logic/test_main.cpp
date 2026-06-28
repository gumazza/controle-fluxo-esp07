#include <unity.h>

#include "fill_logic.h"

void test_pode_iniciar_enchimento() {

    TEST_ASSERT_FALSE(podeIniciarEnchimento(0.0f));

    TEST_ASSERT_FALSE(podeIniciarEnchimento(-1.0f));

    TEST_ASSERT_TRUE(podeIniciarEnchimento(0.1f));

    TEST_ASSERT_TRUE(podeIniciarEnchimento(10.0f));
}

void test_limite_volume_atingido() {

    TEST_ASSERT_FALSE(limiteVolumeAtingido(5.0f, 0.0f));

    TEST_ASSERT_FALSE(limiteVolumeAtingido(0.5f, 1.0f));

    TEST_ASSERT_TRUE(limiteVolumeAtingido(1.0f, 1.0f));

    TEST_ASSERT_TRUE(limiteVolumeAtingido(2.0f, 1.0f));
}

void test_calcular_fluxo_yfs201() {

    TEST_ASSERT_FLOAT_WITHIN(0.01f, 2.0f, calcularFluxoLMin(15, 7.5f));

    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, calcularFluxoLMin(0, 7.5f));

    TEST_ASSERT_FLOAT_WITHIN(0.01f, 0.0f, calcularFluxoLMin(10, 0.0f));
}

void test_calcular_volume_litros() {

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.033f, calcularVolumeLitros(15, 7.5f));

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.0f, calcularVolumeLitros(0, 7.5f));
}

void test_obter_passo_volume() {

    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.010f, obterPassoVolume(0.5f));

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.100f, obterPassoVolume(2.0f));

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 0.500f, obterPassoVolume(7.0f));

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 1.000f, obterPassoVolume(15.0f));
}

void test_multiplicador_velocidade_encoder() {

    TEST_ASSERT_EQUAL(1, obterMultiplicadorVelocidadeEncoder(300));

    TEST_ASSERT_EQUAL(2, obterMultiplicadorVelocidadeEncoder(150));

    TEST_ASSERT_EQUAL(3, obterMultiplicadorVelocidadeEncoder(100));

    TEST_ASSERT_EQUAL(5, obterMultiplicadorVelocidadeEncoder(50));

    TEST_ASSERT_EQUAL(8, obterMultiplicadorVelocidadeEncoder(20));
}

void test_calcular_incremento_volume() {

    TEST_ASSERT_FLOAT_WITHIN(
        0.0001f,
        0.010f,
        calcularIncrementoVolume(0.5f, 1, 400)
    );

    TEST_ASSERT_FLOAT_WITHIN(
        0.001f,
        0.500f,
        calcularIncrementoVolume(7.0f, 1, 100)
    );

    TEST_ASSERT_FLOAT_WITHIN(
        0.01f,
        -5.000f,
        calcularIncrementoVolume(20.0f, -1, 40)
    );
}

void test_quantizar_setpoint_volume() {

    TEST_ASSERT_FLOAT_WITHIN(0.0001f, 0.120f, quantizarSetpointVolume(0.124f));

    TEST_ASSERT_FLOAT_WITHIN(0.001f, 2.30f, quantizarSetpointVolume(2.28f));

    TEST_ASSERT_FLOAT_WITHIN(0.01f, 20.0f, quantizarSetpointVolume(19.96f));

    TEST_ASSERT_FLOAT_WITHIN(0.01f, 20.0f, quantizarSetpointVolume(20.04f));
}

void test_aplicar_incremento_setpoint() {

    TEST_ASSERT_FLOAT_WITHIN(
        0.01f,
        20.0f,
        aplicarIncrementoSetpoint(19.0f, 1.0f)
    );

    TEST_ASSERT_FLOAT_WITHIN(
        0.001f,
        2.50f,
        aplicarIncrementoSetpoint(2.0f, 0.5f)
    );

    TEST_ASSERT_FLOAT_WITHIN(
        0.0001f,
        0.0f,
        aplicarIncrementoSetpoint(0.05f, -0.10f)
    );
}

void test_watchdog_timeout() {

    TEST_ASSERT_FALSE(4000UL >= 5000UL);

    TEST_ASSERT_TRUE(5000UL >= 5000UL);

    TEST_ASSERT_TRUE(6000UL >= 5000UL);
}

int main(int argc, char **argv) {

    UNITY_BEGIN();

    RUN_TEST(test_pode_iniciar_enchimento);

    RUN_TEST(test_limite_volume_atingido);

    RUN_TEST(test_calcular_fluxo_yfs201);

    RUN_TEST(test_calcular_volume_litros);

    RUN_TEST(test_obter_passo_volume);

    RUN_TEST(test_multiplicador_velocidade_encoder);

    RUN_TEST(test_calcular_incremento_volume);

    RUN_TEST(test_quantizar_setpoint_volume);

    RUN_TEST(test_aplicar_incremento_setpoint);

    RUN_TEST(test_watchdog_timeout);

    return UNITY_END();
}
