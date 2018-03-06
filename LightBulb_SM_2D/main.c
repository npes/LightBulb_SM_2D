/*
 * LightBulb_SM_2D.cpp
 *
 * Created: 01-03-2016 22:53:00
 * Author : rust
 *
 *     ___ E_LIGHT_ON                                            ___
 *    /   \ ____________       E_LIGHT_OFF        _____________ /   \
 *   |     /            \ ---------------------> /             \     | E_LIGHT_OFF
 *   |     | S_LIGHT_ON |      E_LIGHT_ON        | S_LIGHT_OFF |     |
 *    \--> \____________/ <--------------------  \_____________/ <--/                  
 *
 *   +----------------------------------------------------+
 *   | STATE\EVENT     | E_LIGHT_ON     | E_LIGHT_OFF     |
 *   |-----------------+----------------+-----------------|
 *   | STATE_LIGHT_ON  |    --------    | STATE_LIGHT_OFF |
 *   |-----------------+----------------+-----------------|
 *   | STATE_LIGHT_OFF | STATE_LIGHT_ON |    --------     |
 *   +----------------------------------------------------+
 */ 

#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdio.h>
#include "STDIO_UART.h"
#include <util/delay.h>

/* The two states in the enum */
enum {
	S_LIGHT_ON,
	S_LIGHT_OFF
};

/* The Two event assigned to the same enum */
enum {
	E_BUTTON_ON,
	E_BUTTON_OFF,
	NO_EVENT	// Used for testing the buttons for button press
};

unsigned int transistionTable[2][2] = {
	{S_LIGHT_ON, S_LIGHT_OFF},			// State S_LIGHT_ON
	{S_LIGHT_ON, S_LIGHT_OFF}			// state S_LIGHT_OFF
};

/* Function Forward decelerations */
int get_button_press();
void handle_action(unsigned int state, unsigned int event);
void print2DArray ();

int main(void)
{
	ioinit();
	unsigned int state;
	unsigned int event;
	
	/* Setup PC.4 - used to simulate light bulb on/off
	 * PC.4 high -> Light ON  - (LED ON)
	 * PC.4 low  -> Light OFF - (LED OFF)
	 */
	DDRC |= (1 << DDC5);    // Set PC.5 to output direction
	PORTC &= ~(1 << DDC5);  // Initially turn LED off

    state = S_LIGHT_OFF;	// Set state variable to initial state.

	
	/* Setup PD.4 and PD.5 as on/off buttons. 
	 * PD.4 (high->low transistion) Light ON
	 * PD.5 (high->low transistion) Light OFF
	 */ 
	PORTD |= (1 << DDD4) | (1 << DDD3);   // Enable pull-up resistor on PD.3 and PD.4.
	DDRD &= ~((1 << DDD4) | (1 << DDD3)); // Set pin PD.3 and PD.4 to input direction
	
	/* The buttons can now be setup in interrupt mode, or pulling mode. 
	 * The buttons in this example are implemented as pulling mode in a separate function
	 */
	
	/* Enter the loop testing for events and running the state machine */
    for (;;)
    {
		event = get_button_press();
		printf("going from state == %d \n", state);
		if (transistionTable[state][event] != state)
		{
			handle_action(state, event);
			state = transistionTable[state][event];
			printf("to state == %d \n", state);
			print2DArray();
			/* output each array element's value */
			
		}
		// else do nothing
	}
}

/************************************************************************/
/* Do the action before entering the state                              */
/************************************************************************/
void handle_action(unsigned int state, unsigned int event)
{
	switch (state)
	{
		case S_LIGHT_ON:
		{
			switch (event)
			{
				case E_BUTTON_OFF:
					PORTC &= ~(1 << DDC5);	// Turn on the LED;
					break;
					
				case E_BUTTON_ON: // Lightbulb already on - do nothing
				break;
			}
		}
		break;
		
		case S_LIGHT_OFF:
		{
			switch (event)
			{
				case E_BUTTON_ON:
					PORTC |= (1 << DDC5);
				break;
				
				case E_BUTTON_OFF: // Lightbulb already off - do nothing
				break;
			}
		}
	}
}

/************************************************************************/
/* Test PD.4 and PD.5 for a button press. Return the corresponding event */
/************************************************************************/
int get_button_press()
{
	int event = NO_EVENT;
	while (event == NO_EVENT)
	{
		if ((PIND & (1 << DDB4)) == 0)
		{
			_delay_ms(100);
			event = E_BUTTON_ON; 
		}
		else if ((PIND & (1 << DDB3)) == 0)
		{
			event = E_BUTTON_OFF;
			_delay_ms(100);
		}
		else
		{
			/* None of the buttons have been pressed - do nothing */ 			
		}
	}
	return event;
}

void print2DArray ()
{
	printf("2darray is : \n");
	for ( int row=0; row < 2; row++ ) {

		for ( int column = 0; column < 2; column++ ) {
			printf("%d ", transistionTable[row][column]);
		}
		printf("\n\n");
	}
}