/*
 * luces.h
 *
 *  Created on: Oct 3, 2024
 *      Author: torre
 */

#ifndef INC_LUCES_H_
#define INC_LUCES_H_



#endif /* INC_LUCES_H_ */
#include "main.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"


//funciona principal
void SET_(uint8_t x, uint8_t y,  uint8_t z );

void SYSTEM_0FF(void);
//funciones
// Función para mostrar flecha hacia la izquierda

void Ublock(void);
void block(void);
void update_oled_left(void);

// Función para borrar flecha izquierda
void clear_oled_left(void) ;

// Función para mostrar flecha hacia la derecha
void update_oled_right(void) ;


// Función para borrar flecha derecha
void clear_oled_right(void);
//funcion parqueo
void update_oled_park(void);

void handle_key_event(char key);

void turn_signal_left(void);

void turn_signal_right(void);




//parqueo
    void park_signal(void);
