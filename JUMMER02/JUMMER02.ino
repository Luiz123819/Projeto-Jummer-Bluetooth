#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <RF24.h>
#include <ezButton.h>
#include "esp_wifi.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// =========================================================================
// 🔌 CONFIGURAÇÃO DE PINOS RECONHECIDOS (Compartilhando o barramento VSPI)
// =========================================================================
// Ambos usam: SCK (18), MISO (19), MOSI (23) para dados.
// Pinos de controle individuais:
#define CE_PIN_1   4   // CE Rádio 1
#define CSN_PIN_1  5   // CSN Rádio 1

#define CE_PIN_2   26  // CE Rádio 2
#define CSN_PIN_2  27  // CSN Rádio 2

RF24 radio1(CE_PIN_1, CSN_PIN_1);  // Rádio 1
RF24 radio2(CE_PIN_2, CSN_PIN_2);  // Rádio 2

ezButton toggleSwitch(33); 

// --- ARTE EM BITMAP: CAVEIRA (32x32 pixels) ---
const unsigned char PROGMEM skull_bmp [] = {
  0x00, 0x07, 0xe0, 0x00, 0x00, 0x1f, 0xf8, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x7f, 0xfe, 0x00,
  0x00, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0x80, 0x03, 0xff, 0xff, 0xc0, 0x03, 0xf3, 0xcf, 0xc0,
  0x07, 0xc1, 0x83, 0xe0, 0x07, 0x00, 0x00, 0xe0, 0x0f, 0x18, 0x18, 0xf0, 0x0e, 0x3c, 0x3c, 0x70,
  0x0e, 0x3c, 0x3c, 0x70, 0x0e, 0x00, 0x00, 0x70, 0x0f, 0x03, 0xc0, 0xf0, 0x07, 0x07, 0xe0, 0xe0,
  0x07, 0x8f, 0xf1, 0xe0, 0x03, 0xdf, 0xfb, 0xc0, 0x01, 0xff, 0xff, 0x80, 0x00, 0xff, 0xff, 0x00,
  0x00, 0x7d, 0xbe, 0x00, 0x00, 0x3c, 0x3c, 0x00, 0x00, 0x3d, 0xbc, 0x00, 0x00, 0x3d, 0xbc, 0x00,
  0x00, 0x3d, 0xbc, 0x00, 0x00, 0x1d, 0xb8, 0x00, 0x00, 0x1d, 0xb8, 0x00, 0x00, 0x0d, 0xb0, 0x00,
  0x00, 0x0d, 0xb0, 0x00, 0x00, 0x05, 0xa0, 0x00, 0x00, 0x07, 0xe0, 0x00, 0x00, 0x03, 0xc0, 0x00
};

// --- ESTRUTURA PARA MÁQUINA DE ESTADOS DOS MODOS ---
class ModoTransmissao {
public:
    virtual void executar() = 0; 
    virtual String obterNome() = 0;
    virtual String obterCanalInfo() = 0;
};

// MODO 1: Varredura Sequencial Rápida (0 a 78) nos dois rádios
class VarreduraSequencial : public ModoTransmissao {
public:
    void executar() override {
        for(int ch = 0; ch < 79; ch++){
            radio1.setChannel(ch);
            radio2.setChannel(ch);
        }
    }
    String obterNome() override { return "SWEEP COMPLETO"; }
    String obterCanalInfo() override { return "1 & 2: 0->78 (FAST)"; }
};

// MODO 2: Varredura Alternada Ping-Pong (2 em 2 canais)
class VarreduraPingPong : public ModoTransmissao {
private:
    byte canalAtual = 45;
    bool subindo = true;
public:
    void executar() override {
        if (subindo) {
            canalAtual += 2;
        } else {
            canalAtual -= 2;
        }

        if (canalAtual > 79 && subindo) {
            subindo = false;
        } else if (canalAtual < 2 && !subindo) {
            subindo = true;
        }
        
        radio1.setChannel(canalAtual);
        radio2.setChannel(canalAtual);
    }
    String obterNome() override { return "VARREDURA ALTERNADA"; }
    String obterCanalInfo() override { return "CH ATUAL: " + String(canalAtual); }
};

VarreduraSequencial modoSequencial;
VarreduraPingPong modoPingPong;
ModoTransmissao* modoAtivo = &modoSequencial;

// --- PROTÓTIPOS ---
void exibirAberturaCaveira();
void inicializarSistemas();
void configurarRadioParametros(RF24 &rad, byte canalInicial);
void atualizarTela();

void setup() {
  // Desliga o Wi-Fi do ESP32 para economizar energia e focar no rádio
  esp_wifi_stop();
  esp_wifi_deinit(); 

  Serial.begin(115200);
  toggleSwitch.setDebounceTime(50);

  // Inicializa o display OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println("Erro ao inicializar SSD1306");
    for(;;);
  }

  display.setRotation(2); // Ajusta orientação da tela se necessário

  exibirAberturaCaveira();


  inicializarSistemas();
}

void loop() {
  toggleSwitch.loop();
  int state = toggleSwitch.getState();

  // Alterna o modo de transmissão com base no botão físico
  if(state == HIGH) {
    modoAtivo = &modoPingPong;
  } else {
    modoAtivo = &modoSequencial;
  }

  modoAtivo->executar();
  atualizarTela();
}

void exibirAberturaCaveira() {
  display.clearDisplay();
  display.drawBitmap(48, 10, skull_bmp, 32, 32, SSD1306_WHITE);
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(18, 50);
  display.print("SYSTEM STARTING");
  display.display();
  delay(2000); 
}

void inicializarSistemas() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("INICIANDO...");
  display.display();

  // 1. FORÇA RESET LÓGICO NOS PINOS DE SELEÇÃO DE CHIP (CSN)
  pinMode(CSN_PIN_1, OUTPUT);
  pinMode(CSN_PIN_2, OUTPUT);
  digitalWrite(CSN_PIN_1, HIGH);
  digitalWrite(CSN_PIN_2, HIGH);  
  delay(300);                     

  display.clearDisplay();
  display.setCursor(0, 5);
  display.println("STATUS DO HARDWARE:");
  display.println("---------------------");
  display.display();

  // 2. TENTA INICIAR O RÁDIO 1 (Até 3 tentativas)
  display.print("RADIO 1 (4/5):   ");
  display.display();
  
  bool statusA = false;
  for (int i = 0; i < 3; i++) {
    if (radio1.begin()) {
      configurarRadioParametros(radio1, 45);
      statusA = true;
      break;
    }
    delay(200);
  }
  display.println(statusA ? "OK" : "FALHA");
  display.display();

  // 3. TENTA INICIAR O RÁDIO 2 (Até 3 tentativas)
  display.print("RADIO 2 (26/27): ");
  display.display();
  
  bool statusB = false;
  for (int i = 0; i < 3; i++) {
    if (radio2.begin()) {
      configurarRadioParametros(radio2, 45);
      statusB = true;
      break;
    }
    delay(200);
  }
  display.println(statusB ? "OK" : "FALHA");
  display.display();

  delay(2500); 

  // Se ambos falharem, trava por segurança
  if(!statusA && !statusB) {
    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("ERRO CRITICO:");
    display.println("Nenhum nRF24 detectado");
    display.println("Reinicie a placa...");
    display.display();
    while(1);
  }
}

// Configura os parâmetros de sinal constante (portadora pura) para teste de RF
void configurarRadioParametros(RF24 &rad, byte canalInicial) {
  rad.setAutoAck(false);
  rad.stopListening();
  rad.setRetries(0, 0);
  rad.setPayloadSize(5);
  rad.setAddressWidth(3);
  rad.setPALevel(RF24_PA_MAX, true);
  rad.setDataRate(RF24_2MBPS);
  rad.setCRCLength(RF24_CRC_DISABLED);
  rad.startConstCarrier(RF24_PA_MAX, canalInicial); 
}

void atualizarTela() {
  static unsigned long ultimaAtualizacao = 0;
  if (millis() - ultimaAtualizacao > 200) { 
    display.clearDisplay();
    
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("SISTEMA DUAL-nRF24");
    display.drawFastHLine(0, 9, 128, SSD1306_WHITE);

    display.setCursor(0, 16);
    display.print("MODO: ");
    display.setCursor(0, 26);
    display.println(modoAtivo->obterNome());

    display.setCursor(0, 44);
    display.print("SINALIZANDO EM:");
    display.setCursor(0, 54);
    display.println(modoAtivo->obterCanalInfo());

    display.display();
    ultimaAtualizacao = millis();
  }
}
