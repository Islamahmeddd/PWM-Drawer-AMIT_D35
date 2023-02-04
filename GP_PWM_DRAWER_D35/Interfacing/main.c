#include "LSTD_TYPES.h"
#include "LBIT_MATH.h"
#include "HLCD_interface.h"
#include "util/delay.h"
#include"MDIO_interface.h"
#include "avr/interrupt.h"
#include "avr/io.h"
#include"MTIMER_interface.h"
#include"MTIMER_private.h"
void Display_parameters_on_LCD(void);
void Display_Pulses_on_LCD(void);
void Display_NEW_parameters_on_LCD(void);
u16_t RISE=0,FALL=0, RISE2=0;
int main(void)
{
    hlcd_init(); //LCD Initialization
    mdio_setPinStatus(PORTD,PIN6,INPUT_FLOAT); //Make (ICP)Input capture To input
    u16_t Input_Capture_Register = ((MTIMER1_ICR1H <<8) | (MTIMER1_ICR1L));
    u16_t Timer_Counter= ((MTIMER1_TCNT1H<<8) | (MTIMER1_TCNT1L));
    SET_BIT(MTIMER_TIFR, 5);	//Clear the input Capture Flag
    Display_parameters_on_LCD();        
      while(1)
	  {
        Timer_Counter = 0;
        MTIMER1_TCCR1A = 0;
        MTIMER1_TCCR1B = 0x41;	// Capture Rising Edge
        while(!(CHECK_BIT(MTIMER_TIFR, 5)));//Wait for The Flag
        RISE = Input_Capture_Register;		// Store Value
        SET_BIT(MTIMER_TIFR, 5);       // Clear the input capture flag

        MTIMER1_TCCR1B = 0x01;	// // Capture Falling Edge
		while(!(CHECK_BIT(MTIMER_TIFR, 5)));//Wait for The Flag
		FALL = Input_Capture_Register;		// Store Value
    	SET_BIT(MTIMER_TIFR, 5);// Clear the input capture flag

		MTIMER1_TCCR1B = 0x41;	// Capture Rising Edge
		while(!(CHECK_BIT(MTIMER_TIFR, 5)));
    	RISE2 = Input_Capture_Register;		// Store Value
		SET_BIT(MTIMER_TIFR, 5);
		MTIMER1_TCCR1B = 0;  // Stop The Timer
		Display_NEW_parameters_on_LCD(); //Display new Freq,Time and Duty Cycle
		Display_Pulses_on_LCD(); //Display PWM  Waves on LCD
	  }
    return 0; 
}

void Display_parameters_on_LCD(void)
{
	hlcd_displayPosition(0, 0);		// Start From Row 0 Col 0
	hlcd_displayString((u8_t*)"F="); //Write Frequency
	hlcd_displayPosition(0, 6);//start from row 0 and col 6
	hlcd_displayString((u8_t*)"T=");//Write Time
	hlcd_displayPosition(0, 12);//Start From Row 0 col 12
	hlcd_displayString((u8_t*)"D="); //Write Duty Cycle
}

void Display_NEW_parameters_on_LCD(void)
{
	int time = RISE2-RISE;
	int frequency = (16000000/time)/1000;			// Calculate frequency
	int dutycycle = (u16_t)((FALL-RISE)*100)/time;	// Calculate Duty Cycle
	hlcd_displayPosition(0, 2);		// Start From Row 0 Col 0
	Lcd_DisplayNum(frequency); //Display Frequency Data
	hlcd_displayPosition(0, 8);//start from row 0 and col 6
	Lcd_DisplayNum(time);//Display Time Data
	hlcd_displayPosition(0, 14);//Start From Row 0 col 12
	Lcd_Display2Num(dutycycle);//Display the Duty Cycle
}

void Display_Pulses_on_LCD(void)
{
	int duty = (((FALL-RISE)*100)/(RISE2-RISE));
	int Rise_Pulse = (duty*8)/100;
	int Counter =0;
	int Fall_Pulse = 8-Rise_Pulse;
	hlcd_displayPosition(1, 0);
	while(Counter<4){
		for(int Risep=1; Risep<Rise_Pulse; Risep++)
			hlcd_displayString((u8_t*)"-"); // Rising  Line
		hlcd_displayCharacter(2);
		for(int FallP=1; FallP<Fall_Pulse; FallP++)
			hlcd_displayString((u8_t*)"_"); //Falling Line
		Counter++;
	}
}