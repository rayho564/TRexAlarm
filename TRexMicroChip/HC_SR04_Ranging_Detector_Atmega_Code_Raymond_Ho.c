#include <avr/io.h>
#include "usart_ATmega1284.h"

#include <util/delay.h>
#include <avr/interrupt.h>

#define USS_PORT	PORTD
#define USS_DDR		DDRD
#define Echo 		PD6
#define Trigger 	PD7
#define ECHOMSK (1<<Echo)

void USS_Trigger(void);
volatile uint16_t Pulse_Time;

int main(void)
{
	
	USS_DDR |= 1<<Trigger; // setting trigger as output
	USS_DDR &=~1<<Echo;    //  and echo as input

	
	initUSART(0);
	char DistinStr[25];
	uint16_t Distance;
	while(1)
	{

		USS_Trigger(); // triggering
		while ((PIND & ECHOMSK) == 0) { ; } //wait for rising edge of Echo
		TCCR1B = (1<<CS10);                 //start timer div1
		while ((PIND & ECHOMSK) != 0) { ; } //wait for falling edge
		TCCR1B = 0;                         //stop timer
		Pulse_Time = TCNT1; // take what's in TCNT1 timer
		TCNT1 = 0x00; //reset what's in TCNT1 timer

		Distance = Pulse_Time / 58.82; // get distance measurement
		
		sprintf(DistinStr,"%d",Distance); // conversion
		USART_SendString(DistinStr, 0);
		_delay_ms(1000);
	}
}

void USS_Trigger()
{
	USS_PORT |= 1<<Trigger; //turn on trigger
	_delay_us(10);
	USS_PORT &=~ 1<<Trigger; //turn off trigger
	
}