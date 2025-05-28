/**
 ******************************************************************************
 * @file           : room_control.c
 * @author         : Sam C
 * @brief          : Room control driver for STM32L476RGTx
 ******************************************************************************
 */
#include "room_control.h"

#include "gpio.h"    // Para controlar LEDs y leer el botón (aunque el botón es por EXTI)
#include "systick.h" // Para obtener ticks y manejar retardos/tiempos
#include "uart.h"    // Para enviar mensajes
#include "tim.h"     // Para controlar el PWM


static volatile uint32_t g_door_open_tick = 0;
static volatile uint8_t g_door_open = 0;
static volatile uint32_t g_last_button_tick = 0;

void room_control_app_init(void)
{
    gpio_write_pin(EXTERNAL_LED_ONOFF_PORT, EXTERNAL_LED_ONOFF_PIN, GPIO_PIN_RESET);
    g_door_open = 0;
    g_door_open_tick = 0;

    tim3_ch1_pwm_set_duty_cycle(20); // Lámpara al 20%
}

void room_control_on_button_press(void)
{
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

        default:
            uart2_send_string("Comando desconocido.\r\n");
            break;
    }
}

void room_control_tick(void)
{
    if (g_door_open && (systick_get_tick() - g_door_open_tick >= 3000)) {
        gpio_write_pin(EXTERNAL_LED_ONOFF_PORT, EXTERNAL_LED_ONOFF_PIN, GPIO_PIN_RESET);
        uart2_send_string("Puerta cerrada automáticamente tras 3 segundos.\r\n");
        g_door_open = 0;
    }
}
