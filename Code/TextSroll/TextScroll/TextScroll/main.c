
#include <stdlib.h>
#include <avr/io.h>
#include <stdint.h>
#include <avr/delay.h>
#include <string.h>
#define _BV(n) (1 << n)
#define F_CPU 4000000
#define LCD_Port PORTD			//Define LCD Port (PORTA, PORTB, PORTC, PORTD)
#define LCD_DPin  DDRD			//Define 4-Bit Pins (PD4-PD7 at PORT D)
#define RSPIN PD0			//RS Pin
#define ENPIN PD1 			//E Pin
//int runtime;			 //Timer for LCD


void LCD_Printpos (char row, char pos, char *str);
void LCD_Init (void);
void LCD_Clear();
void LCD_Print (char *str);
void LCD_LineJmp( unsigned char cmnd );

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
//Write on a specific location
void LCD_Printpos (char row, char pos, char *str)
{
	if (row == 0 && pos<16)
	LCD_LineJmp((pos & 0x0F)|0x80);
	else if (row == 1 && pos<16)
	LCD_LineJmp((pos & 0x0F)|0xC0);
	LCD_Print(str);
}

int cursor = 0;
int linenumber = 1;
int ichar;
char largetext[100] = " Understanding Scrolling text on 16x2 LCD Display.  ";

int main(void)
{
	LCD_Clear();
	Scroll();
	_delay_ms(1500);
}

void Scroll(largetext)
{
	
	
	int TextLength = strlen(largetext);   //find Length of string
	
	if(cursor == (TextLength - 1))  // Resets variable for Roll Over effect
	{
		
		LCD_LineJmp (0x80);		//Move to Position Line 1, Position 1
	}
	
	if( cursor <TextLength-16  )    //For normal 16 character
	{
		for(ichar = cursor; ichar <cursor +16;ichar++ )
		{
			LCD_Print(largetext[ichar]);  // prints message to LCD
			_delay_ms(500);
		}
	}
	else
	{
		for(ichar = cursor; ichar< (TextLength -1);ichar++)  // prints characters of current string
		{
			LCD_Print(largetext[ichar]);  // prints message to LCD
			_delay_ms(500);
		}
		for(ichar =0; ichar <= 16 -(TextLength -cursor);ichar++)
		{
			LCD_Print(largetext[ichar]);  // prints message to LCD
			_delay_ms(500);
		}
	}
	
	
}