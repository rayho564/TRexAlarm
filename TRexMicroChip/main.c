/*
 * Slave.c
 *
 * Created: 10/4/2017 2:44:26 PM
 * Author : LovePoki
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

//--------Shared Variables----------------------------------------------------


//--------End Shared Variables------------------------------------------------

//--------User defined FSMs---------------------------------------------------
enum SM1_States { SM1_wait, SM1_press, SM1_release };
//Enumeration of states.
int SMTick1(int state) {
	// Local Variables
	
	switch (state) {
		case SM1_wait:
		
		break;

		SM1_press:
		
		break;

		case SM1_release:
		
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
		
			default: break; 
		}
		
		break;

		case SM1_release:
		
		
		
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
	MCUCR = (1<<JTD);
	MCUCR = (1<<JTD);
	DDRD = 0xF0; PORTC = 0x0F;
	DDRA = 0xFF; PORTA = 0x00; // LCD data lines
	DDRC = 0xFF; PORTC = 0x00; // LCD control lines


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
	task *tasks[] = { &task1 };
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