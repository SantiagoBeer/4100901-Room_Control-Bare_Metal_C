/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Sam C
 * @brief          : Main program body (Bare-Metal)
 ******************************************************************************
 */
#include "gpio.h"
#include "systick.h"
#include "nvic.h"
#include "uart.h"
#include "tim.h"
#include "room_control.h"

/**
 * @brief Función principal del programa.
 *        Configura los periféricos y entra en un bucle infinito.
*/
int main(void)
{
    // Inicialización de SysTick
    systick_init_1ms(); // Utiliza SYSCLK_FREQ_HZ (ej. 4MHz) de rcc.h

    // LED Heartbeat
    gpio_setup_pin(GPIOA, HEARTBEAT_LED_PIN, GPIO_MODE_OUTPUT, 0);

    // LED Externo ON/OFF
    gpio_setup_pin(GPIOA, EXTERNAL_LED_ONOFF_PIN, GPIO_MODE_OUTPUT, 0);

    // Botón B1 (PC13)
    gpio_setup_pin(GPIOC, 13, GPIO_MODE_INPUT, 0);
    nvic_exti_pc13_button_enable();

    // USART2
    uart2_init(115200);
    nvic_usart2_irq_enable();

    // TIM3 Canal 1 para PWM
    tim3_ch1_pwm_init(1000); // ej. 1000 Hz
    tim3_ch1_pwm_set_duty_cycle(70); // ej. 70%

    // Inicialización de la Lógica de la Aplicación (room_control)
    room_control_app_init();

    // Mensaje de bienvenida
    uart2_send_string("\r\nSistema Inicializado. Esperando eventos...\r\n");

    while (1) {
        room_control_main_tasks(); // Aquí se gestiona la lógica del heartbeat, botón y LED externo
    }
}
