/* Nokia 5110 LCD AVR Library example
 *
 * Copyright (C) 2015 Sergey Denisov.
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version 3
 * of the Licence, or (at your option) any later version.
 *
 * Original library written by SkewPL, http://skew.tk
 * Custom char code by Marcelo Cohen - 2021
 */

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdint.h>
#include <avr/interrupt.h>
#include "nokia5110.h"

uint8_t glyph[] = { 0b00010000, 0b00100100, 0b11100000, 0b00100100, 0b00010000 };
uint8_t minutos = 0;
uint8_t segundos = 0;
uint8_t milisegundos = 0;

void atualizaTempo();

void update_lcd() 
{
    nokia_lcd_clear();
    //nokia_lcd_custom(1, glyph);

    char tempoTela[9];
 
    sprintf(tempoTela, "%02d:%02d:%02d", minutos, segundos, milisegundos);
  
    nokia_lcd_set_cursor(12, 12);
    nokia_lcd_write_string(tempoTela, 1);
    nokia_lcd_render();
}
int main(void)
{
    nokia_lcd_init();

    while (1) {
        atualizaTempo();
        update_lcd();
        milisegundos += 1;
    }
}

void atualizaTempo(){
    if(milisegundos >= 100){
            milisegundos = 0;
            segundos +=1;
        }

        if(segundos >= 60){
            segundos = 0;
            minutos += 1;
        }
    milisegundos += 1;
}
