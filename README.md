# 🔑 STM32 External Flash Bootloader

## 📘 Overview
This project implements a **custom bootloader** on **STM32F103** MCU, capable of loading multiple firmware images stored in an external **W25Q64 NOR Flash** into internal Flash memory.  
It provides a user interface on **LCD9648** with key input navigation, supporting image selection, CRC validation, and jump to application.

The bootloader is designed for **multi-firmware management**, **reliable update**, and **field upgrade capability**, which are critical features in embedded systems and IoT devices.

---

## 🛠️ Hardware Overview
- MCU: **STM32F103C8T6**  
- External Flash: **W25Q64 NOR Flash (SPI)**  
- Display: **LCD9648** (menu display)  
- Keys: **PA1** (menu switch), **PC15** (confirm)  

➡️ Detailed pin mapping and electrical configuration are documented in [Hardware.md](./docs/hardware.md).

---

## 📂 Firmware Architecture

### 1. Internal/External Memory Map
```text
 ┌──────────────────────────┐
 │   Internal Flash 64 KB   │
 │                          │
 │  ┌────────────────────┐  │
 │  │   Bootloader       │  │  0x0800_0000 - 0x0800_3FFF (16 KB)
 │  └────────────────────┘  │
 │  │   Application (APP)│  │  0x0800_4000 - 0x0800_FFFF (~48 KB)
 │  └────────────────────┘  │
 └──────────────────────────┘

 ┌──────────────────────────┐
 │ External Flash W25Q64    │  8 MByte (0x000000 - 0x7FFFFF)
 │                          │
 │  ┌────────────────────┐  │
 │  │   Metadata Table   │  │  0x000000 - 0x000FFF (ImageInfo_t array)
 │  └────────────────────┘  │
 │  │   Firmware Images  │  │  0x001000 - 0x7FFFFF (Repo1, Repo2, ...)
 │  └────────────────────┘  │
 └──────────────────────────┘
```

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

## 🖥️ Boot Process
1. **Reset**: MCU boots from `0x08000000` (Bootloader region).  
2. **System Init**: configure clocks, LCD, SPI, and keys.  
3. **Metadata Fetch**: read `ImageInfo_t` table from W25Q64.  
4. **Menu UI**: LCD displays available repositories, user navigates with keys.  
5. **Flash Update**: selected repo copied from W25Q64 → internal Flash (`0x08004000`).  
6. **CRC Validation**: hardware CRC32 verifies image integrity.  
7. **Jump Execution**:  
   - Relocate vector table (`SCB->VTOR = 0x08004000`).  
   - Load new MSP (`__set_MSP`).  
   - Branch to APP reset handler.

---

## 💾 Flash Programming
- **Erase granularity**: STM32F103 internal Flash → 1 KB pages.  
- **Write granularity**: 16-bit half-word programming.  
- **Buffer strategy**: 256-byte buffered write (W25Q64 → STM32) for efficiency.  
- **Alignment handling**: CRC processes data in 32-bit words, padding if needed.  
- **Safety**: entire APP region erased before programming to prevent residue data.  

---

## ✅ CRC Verification
- **Polynomial**: 0x04C11DB7 (STM32 hardware CRC).  
- **Calculation unit**: 32-bit word, little-endian packing.  
- **Process**:  
  1. Reference CRC32 stored in metadata.  
  2. After programming, Bootloader recalculates CRC from internal Flash.  
  3. Jump allowed only if CRC matches metadata.  
- **Purpose**: prevents corrupted firmware execution due to flash errors or power loss.  

---

## 📊 Workflow State Diagram

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

## 🔍 Debug Guide (ST-LINK Utility)
1. **Inspect Internal Flash**:  
   - `0x08000000` (Bootloader), `0x08004000` (APP).  
2. **Verify APP Load**:  
   - Export memory, confirm content matches W25Q64 image.  
3. **Breakpoints**:  
   - `LoadRepoFromW25Q64()`, `VerifyAppCRC()`, `JumpToApplication()`.  

---

## 🚀 DEMO Video (YouTube) 
[![IMAGE ALT TEXT HERE](https://img.youtube.com/vi/ZV1sHwWrmP4/0.jpg)](https://www.youtube.com/watch?v=ZV1sHwWrmP4)

---



