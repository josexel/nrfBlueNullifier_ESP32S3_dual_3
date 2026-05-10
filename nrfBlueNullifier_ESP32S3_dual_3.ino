/*
 * nrfBlueNullifier - ESP32-S3 Dual NRF24L01+PA+LNA
 * Adaptado para ESP32-S3 con dos módulos NRF24L01+PA+LNA
 * Basado en el proyecto original de WireBits
 *
 * CONEXIONES DE PINES:
 * ─────────────────────────────────────────────────────────
 *  NRF24L01 #A  →  ESP32-S3         NRF24L01 #B  →  ESP32-S3
 *  VCC          →  3.3V              VCC          →  3.3V
 *  GND          →  GND               GND          →  GND
 *  CE           →  GPIO 4            CE           →  GPIO 12
 *  CSN          →  GPIO 5            CSN          →  GPIO 13
 *  SCK          →  GPIO 6            SCK          →  GPIO 14
 *  MOSI         →  GPIO 7            MOSI         →  GPIO 21
 *  MISO         →  GPIO 8            MISO         →  GPIO 47
 *  IRQ          →  No conectar       IRQ          →  No conectar
 *
 * NOTA IMPORTANTE: Los módulos PA+LNA consumen hasta 115mA en TX.
 * Usa un capacitor de 100µF entre VCC y GND de CADA módulo.
 * Alimentar desde fuente estable 3.3V con suficiente corriente.
 * ─────────────────────────────────────────────────────────
 */

#include <RF24.h>
#include <SPI.h>
#include <esp_wifi.h>
#include <esp_bt.h>

// ── Pines SPI Bus A (FSPI - módulo A) ──
#define CE_A    4
#define CSN_A   5
#define SCK_A   6
#define MOSI_A  7
#define MISO_A  8

// ── Pines SPI Bus B (HSPI - módulo B) ──
#define CE_B    12
#define CSN_B   13
#define SCK_B   14
#define MOSI_B  21
#define MISO_B  47

// El ESP32-S3 usa FSPI y HSPI (no tiene VSPI como el ESP32 clásico)
SPIClass *spA = nullptr;
SPIClass *spB = nullptr;

RF24 radioA(CE_A, CSN_A);
RF24 radioB(CE_B, CSN_B);

int channelA = 45;
int channelB = 25;
unsigned int flagA = 0;
unsigned int flagB = 0;

// ── Sweeper con offsets distintos para mayor cobertura ──
void channelSweeper() {
  if (flagA == 0) channelA += 4; else channelA -= 4;
  if (flagB == 0) channelB += 2; else channelB -= 2;

  if ((channelA > 79) && (flagA == 0)) flagA = 1;
  else if ((channelA < 2) && (flagA == 1)) flagA = 0;

  if ((channelB > 79) && (flagB == 0)) flagB = 1;
  else if ((channelB < 2) && (flagB == 1)) flagB = 0;

  radioA.setChannel(channelA);
  radioB.setChannel(channelB);
}

void randomChannel() {
  radioA.setChannel(random(80));
  radioB.setChannel(random(80));
  delayMicroseconds(random(60));
}

// ── Inicializa módulo A en FSPI ──
void initRadioA() {
  spA = new SPIClass(FSPI);
  spA->begin(SCK_A, MISO_A, MOSI_A, CSN_A);

  if (radioA.begin(spA)) {
    radioA.setAutoAck(false);
    radioA.stopListening();
    radioA.setRetries(0, 0);
    radioA.setPayloadSize(32);
    radioA.setAddressWidth(5);
    radioA.setPALevel(RF24_PA_MAX, true);
    radioA.setDataRate(RF24_2MBPS);
    radioA.setCRCLength(RF24_CRC_DISABLED);
    radioA.startConstCarrier(RF24_PA_MAX, channelA);
  }
}

// ── Inicializa módulo B en HSPI ──
void initRadioB() {
  spB = new SPIClass(HSPI);
  spB->begin(SCK_B, MISO_B, MOSI_B, CSN_B);

  if (radioB.begin(spB)) {
    radioB.setAutoAck(false);
    radioB.stopListening();
    radioB.setRetries(0, 0);
    radioB.setPayloadSize(32);
    radioB.setAddressWidth(5);
    radioB.setPALevel(RF24_PA_MAX, true);
    radioB.setDataRate(RF24_2MBPS);
    radioB.setCRCLength(RF24_CRC_DISABLED);
    radioB.startConstCarrier(RF24_PA_MAX, channelB);
  }
}

void setup() {
  // Deshabilitar WiFi interno
  esp_wifi_stop();
  esp_wifi_deinit();

  // Deshabilitar Bluetooth interno (ESP32-S3)
  esp_bt_controller_disable();
  esp_bt_controller_deinit();

  initRadioA();
  initRadioB();
}

void loop() {
  channelSweeper();
  randomChannel();
}
