/**
 ******************************************************************************
 * @file           : room_control.h
 * @author         : Sam C
 * @brief          : Header file for Room Control Application
 ******************************************************************************
 */
#ifndef ROOM_CONTROL_H
#define ROOM_CONTROL_H

#include <stdint.h>

/**
 * @brief Función a ser llamada por EXTI15_10_IRQHandler cuando se detecta
 *        la pulsación del botón B1.
 */
void room_control_on_button_press(void);

/**
 * @brief Función a ser llamada por USART2_IRQHandler cuando se recibe un carácter.
 * @param received_char El carácter recibido por UART.
 */
void room_control_on_uart_receive(char received_char);

/**
 * @brief Función a ser llamada periódicamente por SysTick_Handler para ejecutar
 *        la lógica de control de la sala (ej. apagar luz de puerta después de un tiempo).
 */
void room_control_tick(void);

/**
 * @brief (Opcional) Función para realizar inicializaciones específicas de la lógica
 *        de room_control, si las hubiera (ej. inicializar variables de estado).
 *        Las inicializaciones de periféricos se harán en main().
 */
void room_control_app_init(void);

#endif // ROOM_CONTROL_H
