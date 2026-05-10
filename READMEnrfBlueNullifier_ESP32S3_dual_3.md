# nrfBlueNullifier — ESP32-S3 Dual NRF24L01+PA+LNA

> Barredor de canales Bluetooth 2.4 GHz con dos módulos NRF24L01+PA+LNA sobre ESP32-S3.  
> Basado en el proyecto original **WireBits / nrfBlueNullifier**.

---

## Descripción

Este proyecto implementa un barredor de canales RF en la banda de 2.4 GHz usando dos módulos **NRF24L01+PA+LNA** conectados a un microcontrolador **ESP32-S3** mediante dos buses SPI independientes (FSPI y HSPI). Cada módulo barre el espectro con un offset de canal distinto, maximizando la cobertura del rango Bluetooth (2402–2480 MHz).

---

## Hardware necesario

| Componente | Cantidad |
|---|---|
| ESP32-S3 Dev Module | 1 |
| NRF24L01+PA+LNA | 2 |
| Capacitor electrolítico 100µF / 16V | 2 |
| Capacitor cerámico 100nF | 2 |
| Cables jumper | — |
| Fuente 3.3V estable (≥ 300mA) | 1 |

---

## Conexiones de pines

### Módulo A → Bus FSPI (SPI2)

| NRF24L01 Pin | ESP32-S3 GPIO |
|---|---|
| VCC | 3.3V |
| GND | GND |
| CE | GPIO 4 |
| CSN | GPIO 5 |
| SCK | GPIO 6 |
| MOSI | GPIO 7 |
| MISO | GPIO 8 |
| IRQ | No conectar |

### Módulo B → Bus HSPI (SPI3)

| NRF24L01 Pin | ESP32-S3 GPIO |
|---|---|
| VCC | 3.3V |
| GND | GND |
| CE | GPIO 12 |
| CSN | GPIO 13 |
| SCK | GPIO 14 |
| MOSI | GPIO 21 |
| MISO | GPIO 47 |
| IRQ | No conectar |

---

## Desacoplamiento de alimentación (crítico)

Los módulos **+PA+LNA** consumen hasta **115 mA** durante transmisión. Sin capacitores de desacoplamiento el ESP32-S3 se reinicia o la comunicación SPI falla.

Colocar en **paralelo entre VCC y GND** de **cada módulo**, a menos de 5 mm del pin VCC:

- `100µF` electrolítico — pata larga (+) a VCC, pata corta (−) a GND
- `100nF` cerámico — sin polaridad

---

## Dependencias (librerías Arduino)

| Librería | Instalación |
|---|---|
| RF24 | Arduino IDE → Gestor de librerías → `RF24` by TMRh20 |
| SPI | Incluida en el core ESP32 |
| esp_wifi.h | Incluida en el core ESP32 |
| esp_bt.h | Incluida en el core ESP32 |

---

## Configuración del Arduino IDE

1. Abrir `Archivo → Preferencias` y agregar esta URL en *Gestor de URLs adicionales*:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
2. Ir a `Herramientas → Placa → Gestor de tarjetas` e instalar **esp32 by Espressif Systems**.
3. Seleccionar placa: `ESP32S3 Dev Module`.
4. Instalar la librería **RF24** by TMRh20 desde el Gestor de librerías.

---

## Lógica del firmware

### `channelSweeper()`
Barre los canales de ambos módulos con offsets distintos (+4 / +2 por ciclo). Cuando alcanza el límite superior (canal 79) invierte la dirección. Los módulos A y B nunca barren al mismo ritmo, cubriendo distintas zonas del espectro en paralelo.

### `randomChannel()`
Después del sweep, asigna canales aleatorios a ambos módulos con un retardo aleatorio de hasta 60 µs entre cambios. Esto añade componente aleatoria al patrón de barrido.

### `setup()`
- Desactiva el WiFi y BT internos del ESP32-S3 con `esp_wifi_stop()` y `esp_bt_controller_disable()` para eliminar interferencia interna.
- Inicializa los dos buses SPI de forma independiente.
- Configura ambos radios en modo **portadora continua** (`startConstCarrier`) a potencia máxima (`RF24_PA_MAX`), velocidad 2 Mbps, sin CRC, sin ACK.

---

## Parámetros de configuración

| Parámetro | Módulo A | Módulo B |
|---|---|---|
| Canal inicial | 45 | 25 |
| Paso de sweep | +4 / −4 | +2 / −2 |
| Potencia TX | RF24_PA_MAX | RF24_PA_MAX |
| Velocidad | 2 Mbps | 2 Mbps |
| CRC | Desactivado | Desactivado |
| AutoACK | Desactivado | Desactivado |

---

## Advertencia legal

> Este proyecto genera interferencia de radiofrecuencia en la banda de 2.4 GHz.  
> Su uso puede estar **regulado o prohibido** por la autoridad de telecomunicaciones de tu país.  
> Úsalo únicamente en entornos controlados, con los permisos correspondientes y bajo tu exclusiva responsabilidad.

---

## Créditos

- Proyecto original: **WireBits / nrfBlueNullifier**
- Adaptación ESP32-S3 dual SPI: documentación generada con Claude (Anthropic)
