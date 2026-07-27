# 📡 Transmissor & Analisador de Espectro RF Dual nRF24L01 (ESP32)

Projeto pessoal desenvolvido **do zero (do hardware ao código)** para estudos de radiofrequência, testes de resiliência e análise de propagação de sinais na faixa de 2.4 GHz (ISM Band).
O sistema é baseado em um microcontrolador **ESP32** operando dois transceptores **nRF24L01** através de um barramento SPI compartilhado, com circuito autônomo de gerenciamento de energia via bateria 18650, chave geral física e interface visual em display OLED.
---
## 📷 Galeria do Projeto

<p align="center">
  <img height="180" alt="Módulo TP4056" src="https://github.com/user-attachments/assets/8f0166ff-d15b-4c22-ba03-4c69d1deabec" />
  <img height="180" alt="Regulador AMS1117" src="https://github.com/user-attachments/assets/d80dff77-f732-42ac-afc5-ef134ccb9a1b" />
  <img height="180" alt="Esquema Geral ESP32" src="https://github.com/user-attachments/assets/cf13959c-3587-4ea7-a50b-21737f4c72de" />
  <img height="180" alt="Chave e Alimentação" src="https://github.com/user-attachments/assets/3f87c344-a698-4053-b288-49b7b1d86bc5" />
</p>

---
## 📊 Arquitetura e Funcionalidades

- **Dual-Transmitter Control:** Sincronização e controle de dois módulos nRF24L01 atuando em emissão contínua (*startConstCarrier*).
- **Gerenciamento de Barramento SPI:** Controle preciso das linhas `CSN` via reset lógico de hardware para evitar colisão de barramento no barramento `VSPI`.
- **Máquina de Estados de Transmissão:**
  - **Modo 1 (Sweep Completo):** Varredura sequencial ultrarrápida pelos canais 0 a 78.
  - **Modo 2 (Varredura Alternada):** Alternância dinâmica de canais em salto duplo (Ping-Pong).
- **Interface Visual:** Display OLED I2C com status de inicialização do hardware e informações do canal ativo.
- **Power Optimization:** Desativação completa da pilha Wi-Fi do ESP32 para reduzir ruído elétrico e consumo de corrente.

---

## 🛠️ Especificações de Hardware e Pinagem

| Componente | Sinal / Função | Pino no ESP32 | Notas |
| :--- | :--- | :--- | :--- |
| **Barramento SPI** | SCK / MISO / MOSI | GPIO 18 / 19 / 23 | Barramento VSPI compartilhado |
| **Rádio 1 (nRF24)** | CE / CSN | GPIO 4 / GPIO 5 | Controle Rádio A |
| **Rádio 2 (nRF24)** | CE / CSN | GPIO 26 / GPIO 27 | Controle Rádio B |
| **Display OLED** | SDA / SCL | GPIO 21 / GPIO 22 | Interface I2C (`0x3C`) |
| **Chave Seletora** | Digital IN | GPIO 33 | Seleção de Modo |

> **Nota de Hardware:** Recomenda-se o uso de capacitores de desacoplamento de $10\,\mu\text{F}$ a $100\,\mu\text{F}$ entre as linhas `VCC` e `GND` de cada módulo nRF24L01 para absorver os picos de corrente na inicialização do rádio.

---

## ⚙️ Bibliotecas Requeridas

* [RF24 (TMRh20)](https://github.com/nRF24/RF24)
* [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
* [ezButton](https://github.com/ArduinoGetStarted/button)

---

## 🚀 Como Compilar e Gravar

1. Clone o repositório:
   ```bash
   git clone [https://github.com/SEU-USUARIO/esp32-rf-auditor.git](https://github.com/SEU-USUARIO/esp32-rf-auditor.git)
   ```
2. Abra o projeto na **Arduino IDE** ou **VS Code (PlatformIO)**.
3. Certifique-se de que as bibliotecas acima estão instaladas.
4. Selecione a placa **ESP32 Dev Module**.
5. Compile e faça o upload para a sua placa.

---

## ⚖️ Isenção de Responsabilidade (Disclaimer)

*Este projeto foi desenvolvido exclusivamente para fins educacionais, testes de resiliência em ambientes de laboratório controlados e auditoria de hardware em frequências ISM. O uso de transmissores de radiofrequência deve obedecer às regulamentações locais de telecomunicações.*
