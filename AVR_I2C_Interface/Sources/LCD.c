#include <avr\io.h>
#include <inttypes.h>
#include <avr\pgmspace.h>
#include <stdlib.h>

#include "HARDdef.h"
#include "makra.h"
#include "delay.h"
#include "LCD.h"

#define LCD_EPULSE()\
{PORT(LCD_CTRLPORT) |= 1<<LCD_E;\
	delay250ns();\
	PORT(LCD_CTRLPORT) &= ~(1<<LCD_E);}

void lcd_sendhalf(uint8_t data)
{
	data = (data & 0x0F) << LCD_D4;
	PORT(LCD_DATAPORT) = (PORT(LCD_DATAPORT) & ~(0x0F<<LCD_D4)) | data;
	LCD_EPULSE();
}
	
void lcd_send(uint8_t data)
{
	lcd_sendhalf(data>>4);
	lcd_sendhalf(data);
	delayus8(120);
}

void lcd_command(uint8_t command)
{
	PORT(LCD_CTRLPORT) &= ~(1<<LCD_RS);
	lcd_send(command);
}

void lcd_data(uint8_t data)
{
	PORT(LCD_CTRLPORT) |= 1<<LCD_RS;
	lcd_send(data);
}

void lcd_cls(void)
{
	lcd_command(LCDC_CLS);
	delay100us8(48);
}

void lcd_home(void)
{
	lcd_command(LCDC_HOME);
	delay100us8(48);
}

void lcd_init(void)
{
	delay100us8(150);
	PORT(LCD_CTRLPORT) &= ~(1<<LCD_RS);
	lcd_sendhalf(LCDC_FUNC|LCDC_FUNC8b);
	delay100us8(41);
	lcd_sendhalf(LCDC_FUNC|LCDC_FUNC8b);
	delay100us8(2);
	lcd_sendhalf(LCDC_FUNC|LCDC_FUNC4b);
	delay100us8(2);
	lcd_command(LCDC_FUNC|LCDC_FUNC4b|LCDC_FUNC2L|LCDC_FUNC5x7);
	lcd_command(LCDC_ON);
	lcd_cls();
	lcd_command(LCDC_MODE|LCDC_MODER);
	lcd_command(LCDC_ON|LCDC_ONDISPLAY);
}

void lcd_str(char* str)
{
	int n = 0;
	
	while(str[n] != 0)
	{
		lcd_data(str[n]);
		n++;
	}
}

void lcd_str_P(const char* str PROGMEM)
{
	char znak;
	while((znak = (pgm_read_byte(str++))) != 0) lcd_data(znak);
}

void lcd_dec(int val)
{
	char bufor[7];
	lcd_str(itoa(val,bufor,10));	
}
