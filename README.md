# Controle de Volume Automático - ESP07

Firmware para controle automático de enchimento de fermentadores utilizando ESP07 (ESP8266).

O equipamento controla uma válvula solenóide através da medição contínua do fluxo de líquido, permitindo enchimento preciso por volume.

---

## Principais recursos

* Controle automático por volume
* Medição de vazão em tempo real (L/min)
* Controle de válvula: relé ou retenção PWM (MOSFET)
* Display LCD 16x2 I2C
* Encoder rotativo
* EEPROM versionada para configurações
* Interface Web + WebSocket (2 Hz)
* WiFiManager
* Backend local (calibração, watchdogs, reset)
* Watchdogs de segurança configuráveis
* Timer de enchimento
* Modo manual (botão GPIO0)
* Atualização OTA (ElegantOTA)

---

## Hardware

| Periférico | GPIO | Observação |
|------------|------|------------|
| Sensor de fluxo | 2 | Interrupção FALLING |
| Relé / MOSFET válvula | 13 | HIGH = aberto (relé) |
| Optoacoplador | 15 | Pulso ao completar enchimento |
| Encoder CLK | 12 | Deve estar HIGH no boot |
| Encoder DT | 14 | — |
| Encoder botão | 16 | Sem pull-up interno — usar resistor externo |
| Botão manual | 0 | Boot strap — pull-up externo recomendado |
| LCD SDA / SCL | 4 / 5 | Endereço I2C 0x27 |

---

## Builds PlatformIO

| Ambiente | Define | Uso |
|----------|--------|-----|
| `esp07_rele` (padrão) | `USAR_RELE` | Válvula via relé + opto |
| `esp07_mosfet` | `USAR_MOSFET` | Válvula via PWM com pico e retenção adaptativa |

```bash
pio run -e esp07_rele
pio run -e esp07_mosfet
pio test -e native
```

---

## Calibração (sensor YF-S201)

```
F(Hz) = 7.5 × Q(L/min)
Q = pulsos_por_segundo / fator_calibracao
volume (L) = pulsos_por_segundo / fator_calibracao / 60
```

Fator padrão: **7.5** (ajustável no backend, item CALIB SENSOR).

---

## Operação — Encoder

| Gesto | Ação |
|-------|------|
| Rotação | Ajusta setpoint de volume (STANDBY/ENCHENDO) ou navega backend |
| Clique | Inicia / pausa / retoma enchimento (ou ação no backend/timer) |
| Duplo clique | Entra modo zerar (setpoint pisca); girar alterna set/total; clique zera e sai; duplo clique cancela |
| Long press (2 s) encoder | Entra no backend (STANDBY/ERRO) ou sai do backend (long press no menu) |
| Manual + encoder 2 s | Entra no modo timer (STANDBY); long press no timer sai para STANDBY |

---

## Máquina de estados

```
STANDBY → ENCHENDO → PAUSADO → STANDBY
         ↓ watchdog
        ERRO → STANDBY (clique)
```

Estados auxiliares: `TIMER_OFF`, `TIMER_ON`, `TIMER_PAUSA`, `BACKEND_MENU`.

Diagrama completo em [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).

---

## Troubleshooting

| Sintoma | Causa provável |
|---------|----------------|
| ERRO SEM FLUXO | Válvula aberta sem líquido por > timeout configurado |
| TIMEOUT | Enchimento excedeu tempo máximo |
| Não inicia enchimento | Setpoint = 0 L |
| WiFi não conecta | Controlador funciona normalmente; web/OTA só após conectar. Portal manual: botão manual + encoder no boot, ou RESET WIFI no backend (AP: `controle_volume`) |

---

## Documentação

* [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) — arquitetura e módulos
* [docs/CHANGELOG.md](docs/CHANGELOG.md) — histórico de alterações

---

## Licença

Projeto desenvolvido por MAZZA Handmade.
