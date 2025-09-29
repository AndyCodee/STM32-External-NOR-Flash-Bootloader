# ⚙️ Hardware Specification – STM32 External Flash Bootloader

## 🔌 Pin Mapping
| Peripheral        | MCU Pin (STM32F103C8T6) | GPIO Mode            | Function / Note |
|-------------------|-------------------------|----------------------|-----------------|
| **SPI1 – W25Q64 NOR Flash** | PA4 (NSS/CS) | Output Push-Pull     | Chip Select (software controlled) |
|                   | PA5 (SCK)  | AF Push-Pull Output | SPI Clock (Mode 0, idle low, sample on rising edge) |
|                   | PA6 (MISO) | Input Pull-Up       | Master In, data from W25Q64 |
|                   | PA7 (MOSI) | AF Push-Pull Output | Master Out, data to W25Q64 |
| **LCD9648**       | PB10 (SCL) | Push-Pull Output     | Bit-bang serial clock |
|                   | PB11 (SDA) | Push-Pull Output     | Bit-bang serial data |
|                   | PB12 (RST) | Push-Pull Output     | Reset pin |
|                   | PB13 (CS)  | Push-Pull Output     | Chip Select |
|                   | PB14 (RS)  | Push-Pull Output     | Register Select (Data/Command) |
| **Keys**          | PA1        | Input Pull-Up        | Menu switch |
|                   | PC15       | Input Pull-Up        | Confirm selection |
| **OLED (Soft I2C)** | PB8 (SCL) | Open-Drain Output    | I2C Clock (software bit-bang) |
|                   | PB9 (SDA) | Open-Drain Output    | I2C Data (software bit-bang) |
| **Debug (SWD)**   | PA13 (SWDIO), PA14 (SWCLK) | Dedicated | Debug & Flash with ST-LINK |

---

## 🖥️ SPI Configuration
- **Mode**: Master  
- **Direction**: Full Duplex  
- **Data size**: 8-bit  
- **Clock polarity / phase**: CPOL = 0, CPHA = 1 (Mode 0)  
- **Bit order**: MSB first  
- **Baud rate**: PCLK2 / 128 ≈ 562.5 kHz (safe, adjustable up to 18 MHz for W25Q64)  
- **NSS management**: Software controlled (PA4 manual CS)  

---

## 🧾 Notes
- Always erase full APP region (1 KB/page) before programming.  
