# Controle de Volume Automático - ESP07

Firmware desenvolvido para controle automático de enchimento de fermentadores utilizando ESP07 (ESP8266).

O equipamento controla uma válvula solenóide através da medição contínua do fluxo de líquido, permitindo enchimento preciso por volume.

---

# Principais recursos

* Controle automático por volume
* Medição de vazão em tempo real
* Controle de válvula com retenção PWM
* Display LCD 16x2 I2C
* Encoder rotativo
* EEPROM para armazenamento de configurações
* Interface Web
* WebSocket para atualização em tempo real
* WiFiManager
* Backend local
* Watchdogs de segurança
* Timer de enchimento
* Modo Manual
* Atualização OTA (em desenvolvimento)

---

# Hardware

Microcontrolador

ESP07 (ESP8266)

Periféricos

* LCD1602 I2C
* Encoder Rotativo
* Sensor de Fluxo
* Relé / Driver da válvula
* Optoacoplador
* Botão Manual

---

# Estrutura do firmware

Core

* main.cpp
* config.h
* globals.*

Hardware

* flow_sensor.*
* encoder_control.*
* valve_control.*
* lcd_display.*

Serviços

* storage.*
* watchdogs.*

Interface

* backend_menu.*
* web_interface.*

---

# Máquina de Estados

STANDBY

↓

ENCHENDO

↓

PAUSADO

↓

STANDBY

Estados auxiliares

* TIMER_OFF
* TIMER_ON
* TIMER_PAUSA
* BACKEND_MENU
* ERRO

---

# Compilação

PlatformIO

Plataforma: ESP8266

Framework: Arduino

---

# Licença

Projeto desenvolvido por MAZZA Handmade.
