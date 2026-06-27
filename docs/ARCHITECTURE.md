# Arquitetura — VolControler

## Visão geral

Firmware monolítico Arduino para ESP8266 (ESP-07), organizado em módulos por responsabilidade. O loop principal em `main.cpp` orquestra leitura de sensores, UI, rede e atuadores.

## Fluxo do loop

```mermaid
flowchart LR
    A[atualizarFluxo] --> B[atualizarEncoder]
    B --> C[controlarBotaoEncoder]
    C --> D[atualizarBackendMenu]
    D --> E[atualizarTimer]
    E --> F[verificarWatchdogs]
    F --> G[atualizarAtuadores]
    G --> H[atualizarLCD]
    H --> I[atualizarWebSocket]
    I --> J[atualizarOTA]
    J --> K[verificarSalvarConfiguracoes]
    K --> L[yield]
```

## Módulos

| Módulo | Arquivos | Responsabilidade |
|--------|----------|------------------|
| Core | `main.cpp`, `globals.h`, `config.h`, `system_state.h` | Globals, setup, loop, constantes |
| Lógica pura | `fill_logic.h` | Cálculos testáveis (volume, fluxo, limites) |
| Fluxo | `flow_sensor.*` | ISR de pulsos, integração de volume |
| Atuadores | `valve_control.*` | Válvula, opto, botão manual, limite de volume |
| Encoder | `encoder_control.*` | Polling do encoder, gestos de botão |
| Timer | `timer_control.*` | Contagem regressiva para enchimento |
| Watchdogs | `watchdogs.*` | Sem fluxo e timeout de enchimento |
| Storage | `storage.*` | EEPROM versionada (magic + version) |
| LCD | `lcd_display.*` | UI 16×2 I2C |
| Backend | `backend_menu.*`, `backend.*` | Menu de configuração e portal WiFi |
| Rede | `web_interface.*`, `ota_update.*` | HTTP, WebSocket, OTA |

## Estado global

- `estadoSistema` — máquina de estados principal
- `releLigado` — intenção de válvula aberta (sincronizada em `atualizarAtuadores`)
- `volume_total` / `volume_limite` — medição e setpoint

## Controle de válvula

`valve_control` unifica o antigo `relay_control`:

1. `controlarBotaoManual()` — modo manual via GPIO0
2. Verificação de limite → STANDBY + pulso opto + estatísticas
3. `sincronizarValvula()` — `releLigado` → GPIO13
4. PWM adaptativo (build MOSFET)

## EEPROM

Struct `Configuracoes` com `EEPROM_MAGIC` (0x56430F4C) e `EEPROM_VERSION` (1). Config inválida restaura defaults e regrava.

Campos persistidos: volume, calibração, timer, ciclos, litros, PWM retenção, timeouts de watchdog.

## Builds condicionais

| Define | Efeito |
|--------|--------|
| `USAR_RELE` | `digitalWrite` HIGH/LOW em GPIO13 |
| `USAR_MOSFET` | PWM pico + retenção adaptativa |

## Dependências externas

WiFiManager, LiquidCrystal_I2C, AiEsp32RotaryEncoder (backend), ArduinoJson, ESPAsyncWebServer/TCP, ElegantOTA.

## Testes

Ambiente `native` com Unity testa `fill_logic.h` (sem hardware).

```bash
pio test -e native
```
