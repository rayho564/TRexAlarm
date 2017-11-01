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

void initUSART(unsigned char usartNum);
// Empties the UDR register of the desired USART, this will cause USART_HasReceived to return false.
void USART_Flush(unsigned char usartNum);

// Returns a non-zero number if the desired USART is ready to send data.
// Returns 0 if the desired USART is NOT ready to send data.
unsigned char USART_IsSendReady(unsigned char usartNum);

// Returns a non-zero number if the desired USART has finished sending data.
// Returns 0 if the desired USART is NOT finished sending data.
unsigned char USART_HasTransmitted(unsigned char usartNum);

// Returns a non-zero number if the desired USART has received a byte of data.
// Returns 0 if the desired USART has NOT received a byte of data.
unsigned char USART_HasReceived(unsigned char usartNum);

// Writes a byte of data to the desired USARTs UDR register.
// The data is then sent serially over the TXD pin of the desired USART.
// Call this function after USART_IsSendReady returns 1.
void USART_Send(unsigned char data, unsigned char usartNum);

// Returns the data received on RXD pin of the desired USART.
// Call this function after USART_HasReceived returns 1.
unsigned char USART_Receive(unsigned char usartNum);

//--------Shared Variables----------------------------------------------------
	char Data_in;

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
		/*else
		{
			state = SM1_neither;
		}*/
		break;

		SM1_press:
			Data_in = 0;
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
			/*if( USART_IsSendReady(0) != 0 )
			{
				USART_Send( "WTF are you typing", 0);
			}*/
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
	/*MCUCR = (1<<JTD);
	MCUCR = (1<<JTD);
	DDRD = 0xF0; PORTC = 0x0F;
	DDRA = 0xFF; PORTA = 0x00; // LCD data lines
	DDRC = 0xFF; PORTC = 0x00; // LCD control lines
	*/
	

	DDRB = 0xFF; PORTB = 0;		/* make PORT as output port */


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

	initUSART(0);


	unsigned short i; // Scheduler for-loop iterator
	
    /*while (1) 
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

	}*/
	unsigned char duh = 'A';

	while(1){
		//USART_Send(duh, 0);
		//USART_Send('b', 0);


		if( USART_HasReceived(0) != 0 )
		{
			
			Data_in = USART_Receive(0);
			USART_Flush(0);

			if(Data_in == '1')
			{
				USART_Send(duh, 0);
				PORTB = 0x01;
			}
		}
	}
}