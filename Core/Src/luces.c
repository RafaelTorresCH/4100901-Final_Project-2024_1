/*
 * ligt.c
 *
 *  Created on: Oct 3, 2024
 *      Author: torre
 */

#include "luces.h"
//definiciones de variables utilizadas
uint32_t left_toggles = 0;
uint32_t left_last_press_tick = 0;


uint8_t left_signal_state = 0;

uint32_t right_toggles = 0;
uint32_t right_last_press_tick = 0;
uint8_t right_signal_state = 0;

uint32_t park_toggles = 0;
uint32_t park_last_press_tick = 0;
uint8_t park_signal_state = 0;


// Función para pintar en pantalla
void block(void) {
	ssd1306_Fill(Black);
	ssd1306_SetCursor(25, 10);  // Baja el cursor 10 píxeles más abajo para la segunda línea
	ssd1306_WriteString("   LOCKED", Font_7x10, White);  // Escribe el segundo mensaje

	ssd1306_SetCursor(25, 30);  // Establece el cursor en la posición (25, 30)
	ssd1306_WriteString("WELCOME", Font_11x18, White);

	ssd1306_UpdateScreen();             // Actualiza la pantalla
}
void Ublock(void) {
	ssd1306_Fill(Black);
	ssd1306_SetCursor(25, 10);  // Baja el cursor 10 píxeles más abajo para la segunda línea
	ssd1306_WriteString("  UNLOCKED", Font_7x10, White);  // Escribe el segundo mensaje

	ssd1306_SetCursor(25, 30);  // Establece el cursor en la posición (25, 30)
	ssd1306_WriteString("WELCOME", Font_11x18, White);
	ssd1306_SetCursor(25, 50);  // Baja el cursor 10 píxeles más abajo para la segunda línea
	ssd1306_WriteString(" ", Font_7x10, White);  // Escribe el segundo mensaje
	ssd1306_UpdateScreen();             // Actualiza la pantalla
}


void SYSTEM_0FF(void) {
    ssd1306_Fill(Black);                 // Limpia la pantalla
    ssd1306_SetCursor(10, 20);           // Posiciona la flecha
   	ssd1306_WriteString("SYSTEM OFF", Font_11x18, White);  // Dibuja flecha derecha
	ssd1306_SetCursor(25, 10);  // Baja el cursor 10 píxeles más abajo para la segunda línea
	ssd1306_WriteString("   UNLOCKED", Font_7x10, White);  // Escribe el segundo mensaje

   	ssd1306_UpdateScreen();            // Actualiza la pantalla
}

void update_oled_left(void) {
    ssd1306_Fill(Black);                 // Limpia la pantalla
    ssd1306_SetCursor(10, 10);           // Posiciona la flecha
    ssd1306_WriteString("<= left", Font_11x18, White);  // Dibuja flecha izquierda
    ssd1306_UpdateScreen();              // Actualiza la pantalla
}

// Función para borrar flecha izquierda
void clear_oled_left(void) {
    ssd1306_Fill(Black);                 // Limpia la pantalla
    ssd1306_UpdateScreen();              // Actualiza la pantalla
}

// Función para mostrar flecha hacia la derecha
void update_oled_right(void) {
    ssd1306_Fill(Black);                 // Limpia la pantalla
    ssd1306_SetCursor(10, 10);           // Posiciona la flecha
    ssd1306_WriteString("right =>", Font_11x18, White);  // Dibuja flecha derecha
    ssd1306_UpdateScreen();              // Actualiza la pantalla
}

// Función para borrar flecha derecha
void clear_oled_right(void) {
    ssd1306_Fill(Black);                 // Limpia la pantalla
    ssd1306_UpdateScreen();              // Actualiza la pantalla
}
//funcion parqueo
void update_oled_park(void) {
    ssd1306_Fill(Black);                 // Limpia la pantalla
    ssd1306_SetCursor(10, 20);           // Posiciona la flecha
    ssd1306_WriteString(" --PARK--", Font_11x18, White);  // Dibuja flecha derecha
    ssd1306_UpdateScreen();              // Actualiza la pantalla

}

//esta es la funcion que se encarga de gestionar todas las direccionales
void SET_(uint8_t x, uint8_t y,  uint8_t z ){
	park_toggles = y;
	left_toggles = x;
	right_toggles = z;
}
//selecciona cual de las funciones de pantalla utilizar
void handle_key_event(char key) {
    switch (key) {
        case '1':  // Tecla 1 presionada: Flecha izquierda encendida7

        	update_oled_left();
            break;
        case '2':  // Tecla 2 presionada: Flecha izquierda apagada
            clear_oled_left();   // Limpia la pantalla
            break;
        case '3':  // Tecla 3 presionada: Flecha derecha encendida
            update_oled_right(); // Muestra flecha derecha en la pantalla
            break;
        case '4':  // Tecla 4 presionada: Flecha derecha apagada
            clear_oled_right();  // Limpia la pantalla
            break;
        case '5':
        	update_oled_park();
        	break;
        default:
            // Manejo de otras teclas si es necesario
            break;
    }
}
//direccional izquierda
void turn_signal_left(void)
    {

    	static uint32_t turn_toggle_tick = 0;
    	if (turn_toggle_tick < HAL_GetTick()) {
    		if (left_toggles > 0) {
    			turn_toggle_tick = HAL_GetTick() + 500;
    			HAL_GPIO_TogglePin(izquierdo_GPIO_Port, izquierdo_Pin);

    			left_signal_state = HAL_GPIO_ReadPin(izquierdo_GPIO_Port, izquierdo_Pin) == GPIO_PIN_SET ? 1 : 0;
    			if(left_signal_state == 1){
    				handle_key_event('1');
    				printf("izq\r\n");
    			}else{
    				handle_key_event('2');
    			}

    			left_toggles--;
    			if(left_toggles <= 0){

    				SYSTEM_0FF();
    			}
    		} else {
    			HAL_GPIO_WritePin(izquierdo_GPIO_Port, izquierdo_Pin, 0);
    			//handle_key_event('2');

    		}

    	}
    }


//direccional derecha
    void turn_signal_right(void){

        static uint32_t turn_toggle_tick2 = 0;
        if (turn_toggle_tick2 < HAL_GetTick()) {
            if (right_toggles > 0) {
                // Cambiar el estado del pin cada 500 ms
                turn_toggle_tick2 = HAL_GetTick() + 500;
                HAL_GPIO_TogglePin(derecho_GPIO_Port, derecho_Pin);

                // Leer el estado actual del pin y guardarlo (1 encendido, 0 apagado)
                right_signal_state = HAL_GPIO_ReadPin(derecho_GPIO_Port, derecho_Pin) == GPIO_PIN_SET ? 1 : 0;

                // Llamar a la función handle_key_event según el estado del pin
                if (right_signal_state == 1) {
                    handle_key_event('3');  // Enviar un evento si el pin derecho está encendido
                    printf("der\r\n");
                } else {
                    handle_key_event('4');  // Enviar un evento si el pin derecho está apagado
                }

                // Reducir el contador de toggles
                right_toggles--;
                if(right_toggles <= 0){

                	SYSTEM_0FF();
                    			}
            } else {
                // Apagar el pin derecho
                HAL_GPIO_WritePin(derecho_GPIO_Port, derecho_Pin, 0);

                // Enviar un evento indicando que el pin derecho está apagado
                //handle_key_event('4');
            }
        }
    }



//parqueo
    void park_signal(void)
    {



    	static uint32_t turn_toggle_tick3 = 0;
    	if (turn_toggle_tick3 < HAL_GetTick()) {
    		if (park_toggles > 0) {

    			turn_toggle_tick3 = HAL_GetTick() + 500;
    			HAL_GPIO_TogglePin(izquierdo_GPIO_Port, izquierdo_Pin);
    			HAL_GPIO_TogglePin(derecho_GPIO_Port, derecho_Pin);
    			park_signal_state = HAL_GPIO_ReadPin(derecho_GPIO_Port, derecho_Pin) == GPIO_PIN_SET ? 1 : 0;

    			                // Llamar a la función handle_key_event según el estado del pin
    			                if (park_signal_state == 1) {
    			                    handle_key_event('5');  // Enviar un evento si el pin derecho está encendido
    			                    printf("park\r\n");
    			                } else {
    			                    handle_key_event('2');  // Enviar un evento si el pin derecho está apagado
    			                }

    			park_toggles--;
    		}else{
    			HAL_GPIO_WritePin(izquierdo_GPIO_Port, izquierdo_Pin, 0);
    			HAL_GPIO_WritePin(derecho_GPIO_Port, derecho_Pin, 0);
    		}

    	}

    }
