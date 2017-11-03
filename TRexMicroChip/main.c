/*
 * TRex_main.c
 *
 * Created: 10/31/2017 1:20:26 PM
 * Author : Raymond Ho
 */ 

#include <avr/io.h>
#include "usart_ATmega1284.h"

#include "timer.h"
#include "scheduler.h"
//#include "bit.h"
//#include "keypad.h"
//#include "lcd.h"
#include <util/delay.h>
#include <avr/interrupt.h>


#define USS_PORT	PORTA
#define USS_DDR		DDRA
#define Echo 		PA0
#define Trigger 	PA1
#define ECHOMSK (1<<Echo)

void USS_Trigger(void);
volatile uint16_t Pulse_Time;

//--------Shared Variables----------------------------------------------------
	char Data_in;
	char DistinStr[25];
	uint16_t Distance = 0;
//--------End Shared Variables------------------------------------------------

//--------User defined FSMs---------------------------------------------------
enum SM1_States { SM1_wait, SM1_press, SM1_release, SM1_neither };
//Enumeration of states.
int SMTick1(int state) {
	// Local Variables
	
	//transitions
	switch (state) {
		case SM1_wait:
		// receive data from Bluetooth device
		if( USART_HasReceived(0) != 0 )
		{
		
			Data_in = USART_Receive(0);
			USART_Flush(0);
			
		}
		if(Data_in =='1')
		{
			state = SM1_press;
		}
		else if(Data_in == '2')
		{
			state = SM1_release;
		}
		//else
		//{
			//state = SM1_neither;
		//}
		break;

		SM1_press:
			state = SM1_wait;
		break;

		case SM1_release:
			Data_in = 0;
			state = SM1_wait;
		break;

		case SM1_neither:
			Data_in = 0;
			state = SM1_wait;
		break;

		default:
		state = SM1_wait; // default: Initial state
		break;
	}

	//State machine actions
	switch(state) {
		case SM1_wait:


		break;

		case SM1_press:
			//send status of LED i.e. LED ON
			if( USART_IsSendReady(0) != 0 )
			{
				USART_SendString( "LED_ON", 0);
				//USART_Send('A', 0);
				PORTB = 0x01;
			}
			Data_in = 0;

			USS_Trigger(); // triggering
			while ((PINA & ECHOMSK) == 0) { ; } //wait for rising edge of Echo
			TCCR1B = (1<<CS10);                 //start timer div1
			while ((PINA & ECHOMSK) != 0) { ; } //wait for falling edge
			TCCR1B = 0;                         //stop timer
			Pulse_Time = TCNT1; // take what's in TCNT1 timer
			TCNT1 = 0x00; //reset what's in TCNT1 timer
			
			Distance = Pulse_Time / 588.2; // get distance measurement
			
			sprintf(DistinStr,"%d",Distance); // conversion
			USART_SendString(DistinStr, 0);
			//_delay_ms(50);
			TimerOn();

		break;

		case SM1_release:

			// send status of LED i.e. LED OFF Sending B for off
			if( USART_IsSendReady(0) != 0 )
			{
				USART_SendString( "LED_OFF", 0);
				PORTB = 0x00;
			}
		
		
		break;

		case SM1_neither:
			//if( USART_IsSendReady(0) != 0 )
			//{
				//USART_Send( "WTF are you typing", 0);
			//}
		break;

		default:
		state = SM1_wait;
		break;
	}
	
	return state;
	}
// --------END User defined FSMs-----------------------------------------------

int main(void)
{
	//MCUCR = (1<<JTD);
	//MCUCR = (1<<JTD);
	//DDRD = 0xF0; PORTC = 0x0F;
	//DDRA = 0xFF; PORTA = 0x00; // LCD data lines
	//DDRC = 0xFF; PORTC = 0x00; // LCD control lines
	
	DDRB = 0xFF; PORTB = 0x00;
	USS_DDR |= 1<<Trigger; // setting trigger as output
	USS_DDR &=~1<<Echo;    //  and echo as input

	// Period for the tasks
	unsigned long int SMTick1_calc = 150;
	unsigned long int SMTick2_calc = 150;
	unsigned long int SMTick3_calc = 150;

	//Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(SMTick1_calc, SMTick1_calc);
	//tmpGCD = findGCD(tmpGCD, SMTick3_calc);

	//Greatest common divisor for all tasks or smallest time unit for tasks.
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = SMTick1_calc/GCD;
	unsigned long int SMTick2_period = SMTick2_calc/GCD;
	unsigned long int SMTick3_period = SMTick3_calc/GCD;
		
	//Declare an array of tasks
	static task task1;
	task *tasks[] = { &task1 }; // remember to add tasks back in for multiple
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	// Task 1
	task1.state = SM1_wait;//Task initial state.
	task1.period = SMTick1_period;//Task Period.
	task1.elapsedTime = SMTick1_period;//Task current elapsed time.
	task1.TickFct = &SMTick1;//Function pointer for the tick.

	// Set the timer and turn it on
	TimerSet(GCD);
	TimerOn();

	//LCD_init();
	//LCD_ClearScreen(); 

	//LCD_DisplayString(1, "1");

	initUSART(0);

	unsigned short i; // Scheduler for-loop iterator
	
    while (1) 
    {
		for ( i = 0; i < numTasks; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;

	}
	
}

void USS_Trigger()
{
	USS_PORT |= 1<<Trigger; //turn on trigger
	_delay_us(10);
	USS_PORT &=~ 1<<Trigger; //turn off trigger
	
}