# Manual de Usuario – Sistema de Control de Sala (STM32L476RG)

**Curso:** Estructuras Computacionales
**Universidad Nacional de Colombia – Sede Manizales**

---

## 📟 Introducción

Este sistema implementa un **controlador básico para una sala**, que permite:

* **Controlar el brillo de una lámpara** mediante PWM (PA6).
* **Controlar una puerta** mediante un botón físico (PC13) o comandos remotos UART (PA2/PA3).
* **Monitorear eventos** vía comunicación UART a 115200 baudios.

El sistema está programado en **C puro** accediendo directamente a los registros del STM32L476RG, sin HAL ni CMSIS.

---

## 🔌 Diagrama de Conexiones (Mermaid)

```mermaid
graph TB
    subgraph "PC (Terminal Serial)"
        UART_RX["PA3 (RX)"]
        UART_TX["PA2 (TX)"]
    end

    subgraph "STM32L476RG (Nucleo)"
        B1["PC13 (Botón de Usuario)"]
        LD2["PA5 (Heartbeat LED)"]
        PWM["PA6 (PWM - Lámpara)"]
        DOOR["PA7 (GPIO - Puerta)"]
        MCU["Core STM32L476RG"]
    end

    subgraph "Salida"
        LED_PWM["Lámpara (LED Externo)"]
        LED_GPIO["Puerta (LED Externo)"]
    end

    B1 -->|EXTI13| MCU
    MCU -->|PA5| LD2
    MCU -->|PA6| PWM --> LED_PWM
    MCU -->|PA7| DOOR --> LED_GPIO
    UART_TX --> MCU
    UART_RX --> MCU

```

---

## 📡 Comandos UART

| Comando | Acción                            |
| ------: | --------------------------------- |
|   `'1'` | Encender lámpara al 100%          |
|   `'2'` | Encender lámpara al 70%           |
|   `'3'` | Encender lámpara al 50% (default) |
|   `'4'` | Encender lámpara al 20%           |
|   `'0'` | Apagar lámpara                    |
|   `'o'` | Abrir puerta remotamente          |
|   `'c'` | Cerrar puerta remotamente         |

Baudios: `115200`, Configuración: `8N1`

---

## 🔄 Lógica de Funcionamiento (Mermaid Flowchart)

```mermaid
flowchart TD
    START["Inicio del Sistema"] --> INIT["Inicializa periféricos y lógica"]
    INIT --> LOOP{"Bucle principal"}
    LOOP --> HB["Alterna LED de actividad cada 500ms"]
    LOOP --> EVT{"¿Interrupciones?"}
    EVT -->|"Botón presionado (EXTI13)"| B1_EVT["Encender LED de puerta y guardar tiempo"]
    EVT -->|"UART RX"| UART_EVT["Procesar comando UART (Lampara o puerta)"]
    EVT -->|"SysTick cada 1ms"| TICK_EVT{"¿Puerta lleva más de 3 segundos abierta?"}
    TICK_EVT -->|"Sí"| CLOSE_DOOR["Apagar LED de puerta y enviar mensaje"]
    TICK_EVT -->|"No"| LOOP
    B1_EVT --> LOOP
    UART_EVT --> LOOP
    CLOSE_DOOR --> LOOP
    HB --> LOOP

```

---

## ⚠️ Recomendaciones

* Verifica las conexiones físicas del LED de puerta (PA7) y de lámpara (PA6) con resistencias apropiadas.
* Usa una terminal serial como **PuTTY**, **CoolTerm** o la integrada en **VS Code** para enviar comandos.
* Evita presionar el botón repetidamente en menos de 50 ms (anti-rebote).
* Recuerda que la puerta se cerrará automáticamente después de 3 segundos.

---

## 🧪 Comprobación rápida

1. **Compila y carga** el firmware.
2. **Abre la terminal UART** a 115200 baudios.
3. Presiona el botón B1 o envía comandos para validar el comportamiento.

---
