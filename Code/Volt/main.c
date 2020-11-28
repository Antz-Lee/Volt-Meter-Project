#include <stdlib.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/delay.h>
#include <string.h>
#define _BV(n) (1 << n)
#define F_CPU 4000000
#define LED5x PB1
#define LED10x PB0
#define LCD_Port PORTD		//Define LCD Port (PORTA, PORTB, PORTC, PORTD)
#define LCD_DPin  DDRD		//Define 4-Bit Pins (PD4-PD7 at PORT D)
#define RSPIN PD0			//RS Pin
#define ENPIN PD1 			//E Pin

uint16_t adc_read(uint16_t adcx);         // Allows the ADC ports to be read
void adc_init();                          // Enables the ADC port
void LCD_Init (void);                     // Enables the LCD    
void LCD_Clear();                         // CLears LCD screen and puts LCD cursor at line 1
void LCD_Print (char *str);               // Prints string on LCD
void LCD_LineJmp( unsigned char cmnd );   // Allows LCD Cursor to position at line 1/2
void LED (char *command );                // LED commands


int main(void)
{
	adc_init();        // Enables the ADC port
	LED("Enable");     // Enables the LED 
	LCD_Init();        // Enables the LCD 
	
    // Setting variables
	volatile uint16_t Vread10x = 0;
	volatile uint16_t Vread5x = 0;

	volatile uint16_t ADC0;
	volatile uint16_t ADC1;
    
	// Setting I.D variables
	volatile double Vres = 5.0/1024;
	volatile double Error = .88;    //0.9253382438;
	
	// Voltage difference 
	volatile double Vmeas10x, Vmeas5x;
		
	while(1)
	{
			// Setting the ADC read out pin 
			Vread10x = adc_read(0); 
			Vread5x = adc_read(1);   
					    
			// Measuring the Voltage sum
			Vmeas5x =  (float)Vread5x * Vres * 5 /Error;            //Vdiff for Att 5x
			Vmeas10x = (float)Vread10x * Vres * 10 / Error;        //Vdiff for Att 10x
			
			//value to be displayed
			char DispVmeas5x[6];
			char DispVmeas10x[6];
			snprintf(DispVmeas5x,6,"%f", Vmeas5x);
			snprintf(DispVmeas10x,6,"%f", Vmeas10x);
			
			ADC0= Vread10x;
			ADC1= Vread5x;
			    
			//Switch 
			 if (ADC0 == 0 && ADC1 == 0)   // when connected theres no input
			  {
				  LCD_Clear();                              //clears and starts at position 1
				  LCD_Print("         Connected to GND");
				  LCD_LineJmp(0xC0);                        //Jumps to line two
				  LCD_Print("Unit (V): ");
				  LCD_Print(DispVmeas5x);                   //Displays Value
				  LED("AOF");                               // Turns off all LED
				  _delay_ms(1500);
				  LED("AON");                               // Turns on all LED
				  _delay_ms(15000);
			  }
  			 else if(ADC0 == ADC1 && ADC0 > 0 )  //WARNING CHECK Input
			  {  
				  for(int i=0; i < 3; i++)
				  {  //Flashes
				   LED("AON");                             //Turns on all LED
				   LCD_Clear();                            // clears and starts at position 1
				   LCD_Print("           !!WARNING!!");
				   LCD_LineJmp(0xC0);                      //Jumps to Line 2
				   LCD_Print("   CHECK INPUT");
				   _delay_ms(5000);
				   LCD_Clear();                            // clears and starts at position 1
				   LED("AOF");                             //Turns off all LED
			       _delay_ms(5000);
				  }
			   } 
			  else if(ADC0 > 0 && ADC1 < ADC0)   //10x Position
			  {
				  LED("Atx10");                              //Turns on 10x LED
				  LCD_Clear();                               // clears and starts at position 1
				  LCD_Print("         HighVoltage <45V");
				  LCD_LineJmp(0xC0);                         //jumps to line 2
				  LCD_Print("Unit (V): ");
				  LCD_Print(DispVmeas10x);                   //Displays Value
				  _delay_ms(15000);
			  }
			  else  //if(ADC1 > 0 && ADC1 > ADC0)       
			  {    //5x Position
				   LED("Atx5");                              // Turn on 5x LED
				   LCD_Clear();                              // clears and starts at position 1
				   LCD_Print("         Low Voltage <25V");
				   LCD_LineJmp(0xC0);                        //jumps to line two
				   LCD_Print("Unit (V): ");
				   LCD_Print(DispVmeas5x);                   // Displays Value
				   _delay_ms(15000);   
			  } 
		}
}

void LED (char *command )
{
	if (command == "Enable")
	{
		DDRB |= _BV(LED10x);
		DDRB |= _BV(LED5x);
	}
	else if(command == "Atx10")
	{
		 PORTB &= ~(1<<LED5x);       // Turns off Low Voltage Light
		 PORTB |= _BV(LED10x);      //turns on High Voltage LED
	}
	else if (command == "Atx5")
	{
		 PORTB &= ~(1<<LED10x);    //turns off High Voltage LED
		 PORTB |= _BV(LED5x);     // Turns on Low Voltage Light
	}
	else if(command == "AOF")
	{
		PORTB &= ~(1<<LED10x);    //turns off High Voltage LED
		PORTB &= ~(1<<LED5x);    // Turns off Low Voltage Light
	}
	else if(command == "AON")
	{
		PORTB |= _BV(LED10x);     //turns on High Voltage LED
		PORTB |= _BV(LED5x);     //turns on Low Voltage LED
	}
	else if(command == "Blink")
	{
		PORTB &= ~(1<<LED10x);    //turns off High Voltage LED
		PORTB &= ~(1<<LED5x);    // Turns off Low Voltage Light
		_delay_ms(1500);
		PORTB |= _BV(LED10x);     //turns on High Voltage LED
		PORTB |= _BV(LED5x);     //turns on Low Voltage LED
	}
	else
	{
		return 0;
	}
}

void LCD_Init (void)
{
	LCD_DPin = 0xFF;		//Control LCD Pins (D4-D7)
	_delay_ms(15);		//Wait before LCD activation
	LCD_LineJmp(0x02);	//4-Bit Control
	LCD_LineJmp(0x28);       //Control Matrix @ 4-Bit
	LCD_LineJmp(0x0c);       //Disable Cursor
	LCD_LineJmp(0x06);       //Move Cursor
	LCD_LineJmp(0x01);       //Clean LCD
	_delay_ms(2);
}

void LCD_LineJmp( unsigned char cmnd )
{
	LCD_Port = (LCD_Port & 0x0F) | (cmnd & 0xF0);
	LCD_Port &= ~ (1<<RSPIN);
	LCD_Port |= (1<<ENPIN);
	_delay_us(1);
	LCD_Port &= ~ (1<<ENPIN);
	_delay_us(200);
	LCD_Port = (LCD_Port & 0x0F) | (cmnd << 4);
	LCD_Port |= (1<<ENPIN);
	_delay_us(1);
	LCD_Port &= ~ (1<<ENPIN);
	_delay_ms(2);
}

void LCD_Clear()
{
	LCD_LineJmp (0x01);		//Clear LCD
	_delay_ms(2);			//Wait to clean LCD
	LCD_LineJmp (0x80);		//Move to Position Line 1, Position 1
}


void LCD_Print (char *str)
{
	int i;
	for(i=0; str[i]!=0; i++  )
	{
		LCD_Port = (LCD_Port & 0x0F) | (str[i] & 0xF0);
		LCD_Port |= (1<<RSPIN);
		LCD_Port|= (1<<ENPIN);
		_delay_us(1);
		LCD_Port &= ~ (1<<ENPIN);
		_delay_us(200);
		LCD_Port = (LCD_Port & 0x0F) | (str[i] << 4);
		LCD_Port |= (1<<ENPIN);
		_delay_us(1);
		LCD_Port &= ~ (1<<ENPIN);
		_delay_ms(2);
	}
}

// Enables 
void adc_init()
{
	ADMUX = (1<<REFS0) | (0<<REFS1);
	// ADC Enable and prescaler of 128
	// 8000000/128 = 62500
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

// reads the ADC port
uint16_t adc_read(uint16_t adcx)
{
	ADMUX &= 0xf0;
	ADMUX |= adcx;
	
	ADCSRA |= _BV(ADSC);
	while( ADCSRA & _BV(ADSC)  );
	
	return (double)ADC;
}

/*//Write on a specific location
void LCD_Printpos (char row, char pos, char *str)
{
	if (row == 0 && pos<16)
	LCD_LineJmp((pos & 0x0F)|0x80);
	else if (row == 1 && pos<16)
	LCD_LineJmp((pos & 0x0F)|0xC0);
	LCD_Print(str);
}    */


