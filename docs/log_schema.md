# Schema do log de missão (HLR-SW-02)

> Documento de trabalho da equipe, em português.
> O Design Package da competição precisa desta seção **em inglês**.

## Formato

**JSON Lines**: um objeto JSON por linha, terminado em `\n`. Arquivo
`obc_mission.jsonl`, aberto em modo append.

Por que JSONL e não CSV ou binário:

- **Estruturado** — cada registro é autodescritivo, sem depender de um
  cabeçalho externo para ser lido.
- **Inspecionável** — o HLR-SW-02 é verificado por *inspeção de dados*:
  alguém vai abrir o arquivo e julgar. JSONL se lê a olho nu e ao mesmo
  tempo é parseável por qualquer linguagem.
- **Robusto a truncamento** — se a energia cair no meio da missão, perde-se
  no máximo a última linha; tudo antes continua válido. Um JSON único com
  um array grande ficaria corrompido inteiro.
- **Heterogêneo** — modos diferentes registram campos diferentes, o que um
  CSV de colunas fixas não acomoda bem.

O arquivo é gravado com `fflush()` a cada linha, então um `kill` ou queda de
energia não leva junto o que já foi registrado.

## Campos comuns

Toda linha tem:

| Campo  | Tipo    | Descrição |
|--------|---------|-----------|
| `t_ms` | inteiro | milissegundos desde o `obclog_init()`, relógio **monotônico** |
| `tipo` | string  | discrimina o registro: `boot`, `tc`, `transicao`, `telemetria` |

### Por que monotônico e não hora de parede

A Raspberry Pi Zero 2 W **não tem RTC**. Sem rede, a hora de parede após o
boot é arbitrária, e quando o NTP sincroniza ela dá um salto — que pode cair
no meio da missão e embaralhar a ordem dos registros.

O relógio monotônico nunca anda para trás nem salta. Isso torna `t_ms`
confiável para:

- **ordenar** os registros e reconstruir a sequência dos eventos (HLR-ADS-07);
- **medir latência** entre dois registros por subtração (HLR-ADS-08).

A hora de parede aparece **uma única vez**, no registro `boot`, apenas para
ancorar a missão a uma data. Se a Pi subiu sem NTP, esse campo estará errado
— `t_ms` não estará.

## Tipos de registro

### `boot` — sinal de vida

Primeiro registro do arquivo. Emitido quando a OBC sobe (o boot ocorre ao
remover o pino RBF).

```json
{"t_ms":0,"tipo":"boot","utc":"2026-08-17T17:54:22Z","periodo_ms":1000}
```

| Campo        | Tipo    | Descrição |
|--------------|---------|-----------|
| `utc`        | string  | ISO 8601 UTC; ver ressalva sobre RTC acima |
| `periodo_ms` | inteiro | período do laço principal, base do HLR-COMM-01 |

### `tc` — telecomando recebido

Emitido no ciclo em que o telecomando é lido, **antes** de a FSM agir. A
diferença de `t_ms` entre este registro e a `transicao` seguinte é a
evidência direta do tempo de resposta a TC (HLR-COMM-01).

```json
{"t_ms":263,"tipo":"tc","evento":"EV_TC_MISSION_ADSB"}
```

| Campo    | Tipo   | Descrição |
|----------|--------|-----------|
| `evento` | string | nome do enum `Event` (`fsm_event_name`) |

`EV_NONE` não é registrado: ciclos sem telecomando não geram linha.

### `transicao` — mudança de modo

Evidência de teste do HLR-SW-01: a sequência dessas linhas demonstra a
máquina de estados operando.

```json
{"t_ms":779,"tipo":"transicao","de":"PRE_TEST","para":"MISSION_ADSB"}
```

| Campo  | Tipo   | Descrição |
|--------|--------|-----------|
| `de`   | string | modo de origem (`fsm_state_name`) |
| `para` | string | modo de destino |

### `telemetria` — HLR-COMM-02

```json
{"t_ms":779,"tipo":"telemetria","bateria_v":7.412,"corrente_a":0.385,
 "temp_c":24.75,"roll_deg":-1.25,"pitch_deg":0.50,"yaw_deg":178.33,
 "adsb_msgs":1274}
```

(quebrado em linhas aqui só para caber na página; no arquivo é uma linha só)

Cobre os quatro grupos exigidos pelo HLR-COMM-02:

| Grupo exigido        | Campos |
|----------------------|--------|
| status do sistema    | `temp_c` |
| parâmetros de energia| `bateria_v`, `corrente_a` |
| informação de atitude| `roll_deg`, `pitch_deg`, `yaw_deg` |
| dados de missão      | `adsb_msgs` |

## Pendências

Nada abaixo está decidido — não preencher por conta própria.

- **Campos e unidades da telemetria** precisam ser confirmados com os
  responsáveis por EPS e ADCS. Os nomes atuais são uma proposta da OBC.
- **`temp_c` é de qual sensor?** O `legacy/healthdata.c` lê três DS18B20.
  Definir se a telemetria leva os três ou um agregado.
- **Registro de mensagem ADS-B** ainda não existe. Depende do contrato de
  interface com o payload, que também está pendente. O HLR-ADS-07 pede
  timestamp por mensagem — provavelmente um tipo `adsb` com `t_ms` e os
  campos decodificados.
- **Rotação/tamanho do arquivo** não tratados. A missão são 10 minutos, então
  não deve ser problema, mas o modo `ENV_SURVIVAL` pode rodar indefinidamente.
