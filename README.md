# 🌦 Estação Meteorológica IoT — ESP8266

Este projeto apresenta o desenvolvimento de uma estação meteorológica IoT de baixo custo, capaz de capturar dados ambientais e enviá-los para plataformas na nuvem como ThingSpeak, MQTT (HiveMQ) e Node-RED.  
Trabalho final da disciplina **Internet das Coisas — UNIVALI**, elaborado por **Daiane Wan-Dall Splitter da Silva**.

---

## 📌 Funcionalidades

| Variável Lida | Sensor | Status |
|---|---:|---|
| 🌡 Temperatura | DHT11 | ✅ |
| 💧 Umidade | DHT11 | ✅ |
| 🌪 Pressão Atmosférica | BMP180 | ⚠ Implementado |
| 🏭 Qualidade do Ar (ppm) | MQ-135 | ⚠ Requer calibração |
| 🌧 Presença de Chuva | Sensor de Chuva | ⚠ Implementado |

> Para apresentação final foram utilizados apenas **temperatura e umidade**, conforme orientação docente.

---

## 🧠 Objetivo

Desenvolver uma estação meteorológica conectada, capaz de capturar dados ambientais e transmiti-los automaticamente via **HTTP (ThingSpeak)** e **MQTT (HiveMQ)**, permitindo monitoramento em dashboards online via **Node-RED**.

---

## 🛰 Tecnologias Utilizadas

| Tecnologia | Função |
|---|---|
| ESP8266 (NodeMCU) | Microcontrolador com Wi‑Fi |
| Arduino IDE | Desenvolvimento do firmware |
| ThingSpeak | Armazenamento e visualização dos dados |
| HiveMQ (broker público) | Broker MQTT para telemetria |
| Node-RED | Dashboard interativo com gauges e gráficos |

---

## 🧩 Componentes utilizados

- NodeMCU (ESP8266)
- DHT11 (temperatura e umidade)
- BMP180 (pressão atmosférica)
- MQ-135 (qualidade do ar)
- Sensor de chuva (digital)
- Protoboard, jumpers, fonte USB

---

## 🔌 Esquemático de Ligação

> Diagrama em Fritzing será adicionado no repositório como `docs/diagrama.fzz` (opcional).

### DHT11 → ESP8266  
- VCC → 3.3V  
- GND → GND  
- DATA → D4

### BMP180 → ESP8266  
- VCC → 3.3V  
- GND → GND  
- SCL → D1  
- SDA → D2

### MQ-135 → ESP8266  
- VCC → VIN (5V)  
- GND → GND  
- AOUT → A0

### Sensor de chuva → ESP8266  
- Sinal → D5

---

## 📡 Envio MQTT

Broker utilizado:

```
broker.hivemq.com
Porta: 1883
Sem autenticação
```

Tópicos publicados (exemplo):

```
mestrado/iot/aluno/silva/temperatura
mestrado/iot/aluno/silva/umidade
```

---

## 📄 Envio HTTP (ThingSpeak)

Servidor: `api.thingspeak.com`  
Porta: `80`  
Protocolo: `HTTP POST`  

Campos enviados:
- `field1` → Umidade  
- `field2` → Temperatura  
- `field3` → Pressão relativa  
- `field4` → Pressão absoluta  
- `field5` → ppm MQ-135  
- `field6` → Chuva

---

## 📂 Estrutura sugerida do repositório

```
.
├── README.md
├── src
│   └── main.ino
├── docs
│   ├── diagrama.fzz
│   └── dashboard_screenshots.png
└── LICENSE
```

---

## 📄 Código principal (trecho)

O código completo está em `src/main.ino` (anexo). Abaixo um trecho de exemplo mostrando a publicação MQTT e envio ao ThingSpeak:

```cpp
// MQTT (HiveMQ)
#include <PubSubClient.h>
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// envio MQTT (exemplo)
mqttClient.publish("mestrado/iot/aluno/silva/temperatura", String(t).c_str());
mqttClient.publish("mestrado/iot/aluno/silva/umidade", String(u).c_str());

// envio ThingSpeak (exemplo)
if (client.connect(server, 80)) {
  String postStr = apiKey;
  postStr += "&field1=" + String(u);
  postStr += "&field2=" + String(t);
  // ...
  client.print(postStr);
}
```

---

## 📊 Integração com Node-RED

Fluxo mínimo utilizado:

- `mqtt in` (topic: `mestrado/iot/aluno/silva/temperatura`) → `ui_gauge` + `ui_chart`  
- `mqtt in` (topic: `mestrado/iot/aluno/silva/umidade`) → `ui_gauge` + `ui_chart`

URL do dashboard (local): `http://localhost:1880/ui`

---

## ✅ Resultados

- ThingSpeak: dados armazenados e gráficos gerados com sucesso.  
- HiveMQ: validação em tempo real via WebSocket Client (print em `docs/`).  
- Node-RED: dashboard com gauges e gráfico em tempo real.

Exemplo de telemetria:

| Data/Hora | Tópico | Valor |
|---|---|---|
| 2025-11-30 14:08:52 | `mestrado/iot/aluno/silva/umidade` | 60.00 % |
| 2025-11-30 14:08:52 | `mestrado/iot/aluno/silva/temperatura` | 29.30 °C |

---

## 🚀 Melhorias Futuras

- Publicar pressão, chuva e ppm via MQTT  
- Integração com InfluxDB + Grafana  
- Alertas por Telegram/Email quando limites forem excedidos  
- Versão com BME280 para maior precisão

---

## 📝 Autora

Daiane Wan-Dall Splitter da Silva  
Universidade do Vale do Itajaí — UNIVALI  
Disciplina: Internet das Coisas  
Professor: Jordan P. Sausen

