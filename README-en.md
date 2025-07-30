# 📡 ESPNowTNC
[🇮🇹 Italian Version](README.md)

**ESPNowTNC** is a **TNC (Terminal Node Controller)** based on the **KISS** protocol that uses **ESP-NOW**.

## 📜 KISS Protocol
**KISS** (Keep It Simple, Stupid) is a minimal protocol for encapsulating AX.25 frames (or other data) over serial links (typically RS232, USB, or UART).
It is widely used in **amateur radio** contexts and allows for simple communication between a computer and a TNC (Terminal Node Controller).

[KISS - Wikipedia](https://en.wikipedia.org/wiki/KISS_%28amateur_radio_protocol%29)

## 🌐 Why a KISS TNC over ESP-NOW?
Traditional KISS TNCs are connected to a transceiver operating on amateur radio frequencies.

This project instead uses **ESP-NOW**, a wireless communication technology **based on 802.11**, integrated into **ESP32** chips, operating in the **2.4 GHz ISM band** (the same used by WiFi and Bluetooth).
It can be particularly useful during **software development and debugging** that involves communication with a TNC.

[ESPNow](https://www.espressif.com/en/solutions/low-power-solutions/esp-now)

> [!NOTE]
> This project acts **exclusively** as a KISS TNC:
> - it is **not** a MESH system; it does **not** handle forwarding, retransmission, or routing of packets.
> - it behaves in a **completely transparent** manner, just like an analog radio connected to a traditional TNC.

## 🛠️ Compilation and Upload

This project consists of a single file that can be easily compiled and uploaded using the **Arduino IDE**.

> [!NOTE]
> It has currently only been successfully compiled and tested on the NodeMCU-32S platform.
> Functionality on other boards is **not guaranteed**.

> [!WARNING]
> Debug printing via serial is **enabled by default** (`#define DEBUG true`) and will remain so until the software reaches a stable version.
> If you plan to use the device in a real-world application, it is recommended to disable it to avoid **interference with KISS data**.
> Some KISS software is **tolerant** of extra output, but this is **not guaranteed**.

## 📦 Current Features
- KISS frame encapsulation and decapsulation
- Transmission and reception of fragmented packets via ESP-NOW
- CSMA parameters support (Persistence `P`, SlotTime)

## 🔮 Planned Features
- 🔧 **Custom KISS commands** to:
    - Change the **serial port baud rate**
    - Change the **ESP-NOW radio channel**

## ⚠️ DISCLAIMER ⚠️

This project is **experimental**, in **active development**, and may contain bugs or undergo changes that break compatibility with previous versions.

It is **not suitable for critical applications** or where high reliability or security is required.

While **ESP-NOW** operates in the license-free **2.4 GHz ISM band**, it is essential to:

- **Avoid interfering** with amateur radio communications.
- **Not connect** the device to networks or equipment transmitting on amateur radio frequencies unless you are **legally authorized** (licensed amateur radio operator).

Additionally, the system **does not implement authentication**:
If you plan to use it in an amateur radio context, consider adding **access control mechanisms** to prevent abuse.

The author **is not responsible** for direct, indirect, incidental, or consequential damages resulting from the use, malfunction, or misuse of the software, nor for any regulatory violations by the user.