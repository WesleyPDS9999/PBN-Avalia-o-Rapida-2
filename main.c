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
#include <stdio.h>
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

#define TIMER_CLK		F_CPU / 64
// Freq. de interrupção: 10 Hz
#define IRQ_FREQ		10

//tratamento do INT0
ISR(INT0_vect)
{
    PORTB ^= (1 << PB1);
}

//tratamento do INT1
ISR(INT1_vect)
{
    PORTB ^= (1 << PB2);
}

ISR(TIMER2_COMPA_vect){
    if(milisegundos >= 99){
            milisegundos = 0;
            segundos +=1;
        }

        if(segundos >= 60){
            segundos = 0;
            minutos += 1;
        }
    milisegundos += 1;
}

void atualiza_lcd();

int main(void)
{
    cli();                                  // desabilita interrupções
    int pausa = 1;
    int reset = 0;
    DDRD &= ~(1 << PD7);                    // seta PD7 como entrada
    DDRD &= ~(1 << PD6);                    // seta PD6 como entrada
    DDRB |= (1 << PB1);                     // seta PB1 como saida
    DDRB |= (1 << PB2);                     // seta PB2 como saída
    nokia_lcd_init();

    PORTB &= ~(1 << PB0);                   // desabilita pull-up de PB0
    PORTD &= ~(1 << PD7);                   // desabilita pull-up de PD7
    

    EICRA = (1 << ISC01) | (1 << ISC00);
    EICRA |= (1 << ISC11) | (0 << ISC10);
    EIMSK |= (1 << INT1) | (1 << INT0); 


    sei();                                  //habilita interrupções

    while (1) {
        atualiza_lcd();

        if((PIND &(1 << PD6)) != 0){        //se o botão de start foi pressionado
        _delay_ms(100);
            if(pausa == 0){
                pausa = 1;
            }else{                          //o cronometro deve continuar
                OCR2A = (TIMER_CLK / IRQ_FREQ) - 1;// seta o registrador output compare
                // liga modo CTC
                TCCR2A |= (1 << WGM21);
                // seta CS10 e CS12 para prescaler 1024
                TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20);
                TIMSK2 |= (1 << OCIE2A);    // habilita máscara do timer2
                pausa = 0;
            }
        }else{
            if(pausa == 0){                 //não esta pausado
                atualiza_lcd();
            }else{                          //esta pausado
                TIMSK2 &= ~(1 << OCIE2A);   //pausa o cronometro
                atualiza_lcd();
                if((PIND &(1 << PD7)) != 0){//se o botão de reset foi pressionado
                    milisegundos = 0;
                    segundos = 0;
                    minutos = 0;
                    atualiza_lcd();
                }
            }
        }
    }
}

void atualiza_lcd() 
{
    nokia_lcd_clear();

    char tempoTela[9];

    sprintf(tempoTela, "%02d:%02d:%02d", minutos, segundos, milisegundos);

    nokia_lcd_set_cursor(12, 12);
    nokia_lcd_write_string(tempoTela, 1);
    nokia_lcd_render();
}
