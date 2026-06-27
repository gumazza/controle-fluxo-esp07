# Changelog

## [Revisão 2026-06] — Refatoração completa

### Corrigido

- Válvula/relé não acionava hardware (`releLigado` sem efeito em GPIO)
- Limite de volume não parava enchimento automaticamente
- Backend menu inoperante (`atualizarBackendMenu` ausente do loop)
- Watchdogs ignoravam `timeoutSemFluxo` / `timeoutEnchimento` do backend
- `volume_total` não resetava ao iniciar novo enchimento
- OTA loop (`atualizarOTA`) não era chamado
- Cálculo de volume corrigido (L/min → L/s na integração)
- Default de calibração unificado em 7.5 (YF-S201)

### Adicionado

- Módulo unificado `atualizarAtuadores()` em `valve_control`
- EEPROM versionada com magic e version
- Persistência de timeouts e calibração com auto-save
- Throttle WebSocket (500 ms)
- Funções puras testáveis em `fill_logic.h`
- Testes Unity no ambiente `native`
- Documentação: pinout, calibração, troubleshooting

### Removido

- `relay_control.*` (migrado para `valve_control`)
- `logs.*` (estatísticas integradas em `valve_control`)
- LittleFS do build (não utilizado)
- `String` no hot path do LCD

### Alterado

- `platformio.ini`: versões fixadas das bibliotecas
- WiFiManager com timeouts de conexão e portal
- Encoder library inicializada lazy no backend
