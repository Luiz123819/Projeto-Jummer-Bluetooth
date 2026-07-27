# 📡 Dispositivo de Auditoria RF & Jammer de 2.4 GHz (ESP32)

Ferramenta de hardware de segurança desenvolvida para testes de intrusão, validação de contramedidas e estresse eletromagnético na banda ISM (2.4 GHz). Baseado em um microcontrolador ESP32 e dois módulos nRF24L01 atuando de forma dual, o dispositivo alterna dinamicamente entre a análise de espectro e a inibição ativa de comunicações (RF Jamming) por meio de injeção de pacotes e ruído de portadora contínua. O hardware é portátil e autônomo, equipado com display OLED para controle de frequências, alimentação por célula 18650 e chave liga/desliga física.

## 📷 Galeria do Projeto

<p align="center">
  <img height="180" alt="Módulo TP4056" src="https://github.com/user-attachments/assets/8f0166ff-d15b-4c22-ba03-4c69d1deabec" />
  <img height="180" alt="Regulador AMS1117" src="https://github.com/user-attachments/assets/d80dff77-f732-42ac-afc5-ef134ccb9a1b" />
  <img height="180" alt="Esquema Geral ESP32" src="https://github.com/user-attachments/assets/cf13959c-3587-4ea7-a50b-21737f4c72de" />
  <img height="180" alt="Chave e Alimentação" src="https://github.com/user-attachments/assets/3f87c344-a698-4053-b288-49b7b1d86bc5" />
</p>

## 📊 Arquitetura e Funcionalidades

*   **Controle de Transmissão Dual (Jamming Contínuo):** Sincronização de dois módulos nRF24L01 configurados em modo de emissão de portadora contínua (`startConstCarrier`), gerando ruído de rádio constante para saturação da banda de 2.4 GHz.
*   **Gerenciamento do Barramento SPI:** Controle preciso das linhas CSN via reset lógico de hardware para alternância ultra-rápida entre os transceptores, evitando colisões no barramento VSPI durante o ataque.
*   **Algoritmos de Bloqueio (Máquina de Estados de Ataque):**
    *   **Modo 1 (Barragem / Sweep Completo):** Varredura sequencial e inundação de ruído pelos canais 0 a 78, comprometendo a faixa útil do protocolo nRF24.
    *   **Modo 2 (Salto Dinâmico / Ping-Pong):** Alternância em salto duplo para perseguir dispositivos com hopping de frequência ou derrubar canais específicos de forma intermitente.
*   **Interface Visual:** Display OLED I2C com status do hardware e exibição em tempo real do canal ou faixa sob bloqueio ativo.
*   **Otimização de Hardware:** Desativação completa do Wi-Fi/Bluetooth do ESP32 para mitigar auto-interferência na antena e direcionar a corrente máxima da bateria 18650 para a potência dos transmissores.

## 🛠️ Especificações de Hardware e Pinagem

| Componente | Sinal / Função | Pino no ESP32 | Notas |
| :--- | :--- | :--- | :--- |
| **Barramento SPI** | SCK / MISO / MOSI | GPIO 18 / 19 / 23 | Barramento VSPI compartilhado |
| **Rádio 1 (nRF24)** | CE / CSN | GPIO 4 / GPIO 5 | Controle do Transmissor A |
| **Rádio 2 (nRF24)** | CE / CSN | GPIO 26 / GPIO 27 | Controle do Transmissor B |
| **Display OLED** | SDA / SCL | GPIO 21 / GPIO 22 | Interface I2C (Endereço 0x3C) |
| **Chave Seletora** | Digital IN | GPIO 33 | Seleção do Modo de Ataque |

> ⚠️ **Nota de Hardware:** Recomenda-se o uso de capacitores de desacoplamento de **10 µF a 100 µF** em paralelo com capacitores cerâmicos de **100 nF** entre as linhas VCC e GND de cada módulo nRF24L01. Isso é crítico para absorver os picos de corrente gerados durante a emissão de portadora contínua de alta potência.

## ⚙️ Bibliotecas Requeridas

*   `RF24` (por TMRh20)
*   `Adafruit SSD1306` & `Adafruit GFX Library`
*   `ezButton`

## 🚀 Como Compilar e Gravar

1. Clone o repositório:
   ```bash
   git clone https://github.com/SEU-USUARIO/esp32-rf-auditor.git
   ```
2. Abra o projeto na **Arduino IDE** ou **VS Code (PlatformIO)**.
3. Certifique-se de que as bibliotecas listadas acima estão instaladas no seu ambiente.
4. Selecione a placa **ESP32 Dev Module**.
5. Compile e faça o upload do firmware para a sua placa.

## ⚖️ Isenção de Responsabilidade (Disclaimer)

Este projeto foi desenvolvido exclusivamente para fins educacionais, testes de resiliência em ambientes de laboratório controlados e auditoria de hardware em frequências ISM. O uso de transmissores de radiofrequência para inibição ou bloqueio intencional de sinais (Jamming) fora de ambientes de teste autorizados pode violar as regulamentações locais de telecomunicações (como as normas da ANATEL no Brasil). O autor não se responsabiliza pelo uso indevido deste equipamento.


*Este projeto foi desenvolvido exclusivamente para fins educacionais, testes de resiliência em ambientes de laboratório controlados e auditoria de hardware em frequências ISM. O uso de transmissores de radiofrequência deve obedecer às regulamentações locais de telecomunicações.*
