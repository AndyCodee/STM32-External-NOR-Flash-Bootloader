# 🔑 STM32 External Flash Bootloader

## 📘 Overview
This project implements a **custom bootloader** on **STM32F103** MCU, capable of loading multiple firmware images stored in an external **W25Q64 NOR Flash** into internal Flash memory.  
It provides a user interface on **LCD9648** with key input navigation, supporting image selection, CRC validation, and jump to application.

The bootloader is designed for **multi-firmware management**, **reliable update**, and **field upgrade capability**, which are critical features in embedded systems and IoT devices.

## 🛠️ Hardware Specification
This project is based on **STM32F103C8T6** with an external **W25Q64 NOR Flash**.  
Peripherals include **LCD9648** (menu display) and keys (PA1/PC15) for navigation.  

➡️ For detailed pin mapping and hardware notes, see [Hardware.md](./docs/hardware.md).

## 📂 Firmware Architecture
### 1. Memory Map
- **Bootloader**: `0x08000000 – 0x08003FFF`
- **Application (APP)**: starts from `0x08004000`
- **External Flash**: stores multiple firmware images + metadata
- 
### 2. Metadata Structure
Each repository stored in W25Q64 has an associated `ImageInfo_t`:
```c
typedef struct {
    uint32_t ValidFlag;   // 0xA5A5A5A5 = valid
    uint32_t Address;     // image start address in W25Q64
    uint32_t Size;        // program size in bytes
    uint32_t CRC32;       // integrity check value
} ImageInfo_t;
```
---
### 3. Workflow
1. **Bootloader Init**: initialize LCD, keys, W25Q64, and read metadata
2. **Menu Display**: LCD shows available firmware (Repo1, Repo2, …)
3. **User Input**: select desired repo with keys
4. **Image Loading**: copy image from W25Q64 → STM32 internal Flash (`0x08004000+`)
5. **CRC Validation**: hardware CRC module verifies integrity against metadata
6. **Jump to Application**: vector table remapped, MSP/PC set, execution transferred

## 🖥️ Boot Process
1. **Reset**: MCU boots from `0x08000000` (Bootloader region)  
2. **System Init**: configure clocks, peripherals (LCD, SPI, Keys)  
3. **Metadata Fetch**: read `ImageInfo_t` table from W25Q64  
4. **User Selection**: display repo menu on LCD, wait for key input  
5. **Flash Update**: erase internal APP region, program image from W25Q64  
6. **CRC Validation**: STM32 hardware CRC32 peripheral validates APP integrity  
7. **Jump Execution**:  
   - Relocate vector table (`SCB->VTOR = 0x08004000`)  
   - Load new MSP (`__set_MSP`)  
   - Branch to APP reset handler

---

## 💾 Flash Programming Details
- **Erase granularity**: STM32F103 internal Flash → 1 KB pages  
- **Write granularity**: 16-bit half-word programming  
- **Buffer strategy**: 256-byte buffer read from W25Q64 → batch write to reduce SPI overhead  
- **Alignment handling**: CRC calculation processes data in 32-bit words, with proper padding for non-aligned tails  
- **Safety**: entire APP region is erased before programming to avoid leftover data  

---

## ✅ CRC Verification
- **Polynomial**: 0x04C11DB7 (STM32 hardware CRC peripheral)  
- **Calculation unit**: 32-bit word, little-endian packing  
- **Process**:  
  1. Metadata includes reference CRC32 value  
  2. After loading image to internal Flash, hardware CRC engine recalculates checksum  
  3. Bootloader compares result against metadata before jump  
- **Rationale**: ensures protection against flash corruption, transmission errors, and power-loss anomalies  

---

## 🔍 Key Features
- **Multi-firmware support**: multiple repos selectable at boot  
- **Integrity check**: STM32 hardware CRC32 validation before execution  
- **Robust Flash management**: sector erase + 256B buffered writes  
- **User interface**: LCD menu with key navigation  
- **Clean handover**: VTOR relocation + MSP setup for safe APP execution

---

## 📊 State Diagram

```text
 ┌────────────┐
 │  Bootloader│
 └─────┬──────┘
       │ Init LCD/Key/W25Q64
       ▼
 ┌──────────────┐
 │ Repo Menu UI │
 └─────┬────────┘
       │ Key input
       ▼
 ┌──────────────┐
 │  Load Image  │  ← Copy W25Q64 → Internal Flash
 └─────┬────────┘
       │ CRC OK?
   ┌───┴───┐
   │ Yes   │  ──► Jump to Application
   │ No    │  ──► Display "CRC FAIL!"
   └───────┘
```
---

## ⚡ Reliability Considerations
- **Atomic update**: write progress flag can be extended to prevent corrupted APP after power loss  
- **Boot recovery**: invalid CRC → always stay in bootloader  
- **Metadata validation**: size, address, and valid flag checked before update  
- **Future extensibility**: versioning, timestamp, and signature fields can be added  

