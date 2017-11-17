/*
 * TRex_main.c
 *
 * Created: 10/31/2017 1:20:26 PM
 * Author : Raymond Ho
 *
 * There is an issue where I have conflicting Timer's. Timer.h for scheduler and Timer for our Ranging detector causes a delay to read the value in.
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
void BT_rename( char *sendMe);
void BT_changePin( char *sendMe);
void get_dist();
volatile uint16_t Pulse_Time;

//--------Shared Variables----------------------------------------------------
	char Data_in = 1;
	char DistinStr[25];
	uint16_t Distance = 0;
	int def_dist = 0;
	int dist_diff = 3; //3 cm diff will cause alarm
	char name[30] = "Default_name";
	char pin[30] = "1234";
	char* nameStr = "name";
	char* pinStr = "pass";
	char* discStr = "disconnect";
//--------End Shared Variables------------------------------------------------

//--------User defined FSMs---------------------------------------------------
enum SM1_States { SM1_wait, SM1_on, SM1_polling, SM1_rename, SM1_changePin };
//Enumeration of states.
int SMTick1(int state) {
	// Local Variables
	
	// '0' = default value for wait
	// '1' = on
	// '2' = off
	// '3' = set new default
	// '4' = set name
	// '5' = change pass

	//transitions
	switch (state) {
		case SM1_wait:
		// receive data from Bluetooth device
		if( USART_HasReceived(0) != 0 )
		{
		
			Data_in = USART_Receive(0);
			USART_Flush(0);
			
		}
		//If 1, start pulsing to set default distance
		if(Data_in =='1')
		{
			state = SM1_on;
		}
		
		break;

		SM1_on:
			state = SM1_polling;
		break;

		case SM1_polling:
			if( USART_HasReceived(0) != 0 )
			{
				
				Data_in = USART_Receive(0);
				USART_Flush(0);
				
			}
			if(Data_in == '2')
			{
				PORTB = 0x00;
				state = SM1_wait;
			}
			else if(Data_in == '3')
			{
				state = SM1_on;
			}
			else if(Data_in == '4')
			{
				state = SM1_rename;
			}
			else if(Data_in == '5')
			{
				state = SM1_changePin;
			}
			else
			{
				state = SM1_polling;
			}
		break;

		case SM1_rename:
			Data_in = 0;
			
				
			state = SM1_wait;
		break;
		case SM1_changePin:
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
		//USART_SendString( "statusOFF", 1, 0);


		break;

		case SM1_on:
			//send status of PULSE i.e. PULSE ON
			if( USART_IsSendReady(0) != 0 )
			{
				USART_SendString( "statusON", 1, 0);

				//USART_Send('A', 0);
				PORTB = 0x01;

			}
			Data_in = 0;
			//get first distance to set default
			//get_send_dist();
			get_dist();
			def_dist = Distance;

			state = SM1_polling;

		break;

		case SM1_polling:

			//Constantly polling to see if different from default
			//debugging purposes
			/*if( USART_IsSendReady(0) != 0 )
			{
				USART_SendString( "Polling", 1, 0);
				//PORTB = 0x00;
			}*/
			//state = SM1_wait;
			//get_send_dist();
			Data_in = 0;
			get_dist();

			if((Distance < (def_dist-dist_diff)) || (Distance > (def_dist+dist_diff)))
			{
				//Send Alarm to alert if Distance is +-2 compared to default
				USART_SendString("Alarm", 1, 0);
				_delay_ms(500);

				
				//char str[80];
				//strcpy(str, nameStr);
				//strcat(str, name);

				//USART_SendString(str, 1, 0);

				//if alarmed go back to wait for acknowledgement
				state = SM1_wait;
			}
					
		break;

		case SM1_rename:
			//wait until full string is sent
			USART_SendString("statusRenaming", 1, 0);

			//while( USART_HasReceived(0) == 0 ){;}
			//strcpy(name, USART_GetString(0));
			USART_GetString(name, 0);
			//USART_SendString(name, 1, 0);
			
			BT_rename(name);
		break;
		case SM1_changePin:
		//wait until full string is sent
		USART_SendString("statusChanging Pin", 1, 0);

		USART_GetString(pin, 0);
		
		BT_changePin(pin);
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
	unsigned long int SMTick1_calc = 50;
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

void get_send_dist(){
	USS_Trigger(); // triggering
	while ((PINA & ECHOMSK) == 0) { ; } //wait for rising edge of Echo
	TCCR1B = (1<<CS10);                 //start timer div1
	while ((PINA & ECHOMSK) != 0) { ; } //wait for falling edge
	TCCR1B = 0;                         //stop timer
	Pulse_Time = TCNT1; // take what's in TCNT1 timer
	TCNT1 = 0x00; //reset what's in TCNT1 timer
	
	Distance = Pulse_Time / 588.2; // get distance measurement
	
	sprintf(DistinStr,"%d",Distance); // conversion
	USART_SendString(DistinStr, 1, 0);
	_delay_ms(500);


	//_delay_ms(50);
	TimerOn();
}
void get_dist(){
	USS_Trigger(); // triggering
	while ((PINA & ECHOMSK) == 0) { ; } //wait for rising edge of Echo
	TCCR1B = (1<<CS10);                 //start timer div1
	while ((PINA & ECHOMSK) != 0) { ; } //wait for falling edge
	TCCR1B = 0;                         //stop timer
	Pulse_Time = TCNT1; // take what's in TCNT1 timer
	TCNT1 = 0x00; //reset what's in TCNT1 timer
	
	Distance = Pulse_Time / 588.2; // get distance measurement

	//_delay_ms(50);
	TimerOn();

}

void BT_rename( char *sendMe ) {
		_delay_ms(500);

		USART_SendString(discStr, 2, 0);

		//give some time to dc
		_delay_ms(2000);
		
		USART_SendString("AT+NAME", 0, 0);
		USART_SendString(name, 0, 0);
		USART_Send(0x0d, 0);
		USART_Send(0x0a, 0);


		//USART_SendString(reccStr, 2, 0);

}
void BT_changePin( char *sendMe ) {
	_delay_ms(500);

	USART_SendString(discStr, 2, 0);

	//give some time to dc
	_delay_ms(2000);
	
	USART_SendString("AT+PIN", 0, 0);
	USART_SendString(pin, 0, 0);
	USART_Send(0x0d, 0);
	USART_Send(0x0a, 0);


	//USART_SendString(reccStr, 2, 0);

}