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
}
