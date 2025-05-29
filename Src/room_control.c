/**
 ******************************************************************************
 * @file           : room_control.c
 * @author         : Sam C
 * @brief          : Room control driver for STM32L476RGTx (Bare-Metal)
 ******************************************************************************
 */
#include "room_control.h"
#include <stdio.h>
#include <string.h>

#include "gpio.h"    // Para controlar LEDs y leer el botón
#include "systick.h" // Para obtener ticks y manejar retardos/tiempos
#include "uart.h"    // Para enviar mensajes
#include "tim.h"     // Para controlar el PWM

<<<<<<< HEAD

static volatile uint32_t g_door_open_tick = 0;
static volatile uint8_t g_door_open = 0;
static volatile uint32_t g_last_button_tick = 0;

void room_control_app_init(void)
{
    gpio_write_pin(EXTERNAL_LED_ONOFF_PORT, EXTERNAL_LED_ONOFF_PIN, GPIO_PIN_RESET);
    g_door_open = 0;
    g_door_open_tick = 0;

    tim3_ch1_pwm_set_duty_cycle(20); // Lámpara al 20%

    uart2_send_string("Controlador de Sala v1.0\r\n");
    uart2_send_string("Desarrollador: [German Santiago Bernal Hoyos]\r\n");
    uart2_send_string("Estado inicial:\r\n");
    uart2_send_string(" - Lámpara: 20%\r\n");
    uart2_send_string(" - Puerta: Cerrada\r\n");
=======
#define BUTTON_DEBOUNCE_MS 200  // Tiempo de debounce configurable (ms)

static char tx_buffer[50];

// --- Variables de estado ---
static volatile uint8_t heartbeat_state = 0;
static volatile uint32_t led_ext_off_time = 0;

// --- Inicialización principal ---
void room_control_app_init(void)
{
    // Inicializa PWM, UART, SysTick, etc. si no lo hace main()
    tim3_ch1_pwm_set_duty_cycle(0); // LED PWM apagado al inicio
    gpio_write_ld2(0);              // LD2 apagado
    gpio_write_led_ext(0);          // LED externo apagado
>>>>>>> 50c5022abba163564e6894b583464d05d1616075
}

// --- Heartbeat: Parpadeo de LD2 ---
void room_control_heartbeat_task(void)
{
    static uint32_t last_toggle = 0;
    if (systick_get_tick() - last_toggle >= 500) { // 500 ms
        heartbeat_state = !heartbeat_state;
        gpio_write_ld2(heartbeat_state);
        last_toggle = systick_get_tick();
    }
}

// --- Botón: Interrupción EXTI ---
void room_control_on_button_press(void)
{
<<<<<<< HEAD
    uint32_t now = systick_get_tick();
    if (now - g_last_button_tick < 50) return;  // Anti-rebote de 50 ms
    g_last_button_tick = now;

    uart2_send_string("Evento: Botón presionado - Abriendo puerta.\r\n");

    gpio_write_pin(EXTERNAL_LED_ONOFF_PORT, EXTERNAL_LED_ONOFF_PIN, GPIO_PIN_SET);
    g_door_open_tick = now;
    g_door_open = 1;
}

void room_control_on_uart_receive(char cmd)
{
    switch (cmd) {
        case '1':
            tim3_ch1_pwm_set_duty_cycle(100);
            uart2_send_string("Lámpara: brillo al 100%.\r\n");
            break;

        case '2':
            tim3_ch1_pwm_set_duty_cycle(70);
            uart2_send_string("Lámpara: brillo al 70%.\r\n");
            break;

        case '3':
            tim3_ch1_pwm_set_duty_cycle(50);
            uart2_send_string("Lámpara: brillo al 50%.\r\n");
            break;

        case '4':
            tim3_ch1_pwm_set_duty_cycle(20);
            uart2_send_string("Lámpara: brillo al 20%.\r\n");
            break;

        case '0':
            tim3_ch1_pwm_set_duty_cycle(0);
            uart2_send_string("Lámpara apagada.\r\n");
            break;

        case 'o':
        case 'O':
            gpio_write_pin(EXTERNAL_LED_ONOFF_PORT, EXTERNAL_LED_ONOFF_PIN, GPIO_PIN_SET);
            g_door_open_tick = systick_get_tick();
            g_door_open = 1;
            uart2_send_string("Puerta abierta remotamente.\r\n");
            break;

        case 'c':
        case 'C':
            gpio_write_pin(EXTERNAL_LED_ONOFF_PORT, EXTERNAL_LED_ONOFF_PIN, GPIO_PIN_RESET);
            g_door_open = 0;
            uart2_send_string("Puerta cerrada remotamente.\r\n");
            break;

        case '?': 
            uart2_send_string("Comandos disponibles:\r\n");
            uart2_send_string("1: Lámpara al 100%\r\n");
            uart2_send_string("2: Lámpara al 70%\r\n");
            uart2_send_string("3: Lámpara al 50%\r\n");
            uart2_send_string("4: Lámpara al 20%\r\n");
            uart2_send_string("0: Lámpara apagada\r\n");
            uart2_send_string("o: Abrir puerta \r\n");
            uart2_send_string("c: Cerrar puerta \r\n");
            uart2_send_string("s: Estado del sistema\r\n");
            uart2_send_string("?: Ayuda \r\n");
        
        case 'g':
        case 'G': 

        default:
            uart2_send_string("Comando desconocido.\r\n");
            break;
    }
=======
    // Anti-rebote configurable
    static uint32_t last_press = 0;
    if (systick_get_tick() - last_press < BUTTON_DEBOUNCE_MS) return; // Usar macro
    last_press = systick_get_tick();

    gpio_write_led_ext(1); // Enciende LED externo
    gpio_write_ld2(1);     // Enciende LD2
    led_ext_off_time = systick_get_tick() + 3000; // 3 segundos
    uart2_send_string("Boton B1: Presionado..\r\n");
}

// --- Tarea para apagar LED externo tras timeout ---
void room_control_led_ext_task(void)
{
    if (led_ext_off_time != 0 && systick_get_tick() >= led_ext_off_time)
    {
        gpio_write_led_ext(0); // Apaga LED externo
        gpio_write_ld2(0);     // Apaga LD2
        uart2_send_string("LED externo apagado.\r\n");
        led_ext_off_time = 0;
    }
}

// --- Procesamiento de comandos UART ---
void room_control_on_uart_receive(char received_char)
{
    switch (received_char) {
        case 'h':
        case 'H':
            tim3_ch1_pwm_set_duty_cycle(100); // LED PWM al 100%
            uart2_send_string("PWM LED: 100%\r\n");
            break;
        case 'l':
        case 'L':
            tim3_ch1_pwm_set_duty_cycle(0);   // LED PWM apagado
            uart2_send_string("PWM LED: 0%\r\n");
            break;
        case 't':
        case 'T': {
            static uint8_t pwm_state = 0;
            pwm_state = !pwm_state;
            tim3_ch1_pwm_set_duty_cycle(pwm_state ? 100 : 0);
            uart2_send_string("PWM LED: Toggle\r\n");
            break;
        }
        case '5':
            tim3_ch1_pwm_set_duty_cycle(50);  // 50% intensidad
            uart2_send_string("PWM LED: 50%\r\n");
            break;
        default:
            uart2_send_string("Comando no reconocido\r\n");
            break;
    }
}

// --- Llamar estas funciones en el main loop ---
void room_control_main_tasks(void)
{
    room_control_heartbeat_task();
    room_control_led_ext_task();
>>>>>>> 50c5022abba163564e6894b583464d05d1616075
}

void room_control_tick(void)
{
    if (g_door_open && (systick_get_tick() - g_door_open_tick >= 3000)) {
        gpio_write_pin(EXTERNAL_LED_ONOFF_PORT, EXTERNAL_LED_ONOFF_PIN, GPIO_PIN_RESET);
        uart2_send_string("Puerta cerrada automáticamente tras 3 segundos.\r\n");
        g_door_open = 0;
    }
}

