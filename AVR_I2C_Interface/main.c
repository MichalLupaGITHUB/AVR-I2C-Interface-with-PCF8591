#include <avr\io.h>
#include <stdio.h>
#include <inttypes.h>
#include <avr\pgmspace.h>
#include <avr\delay.h>

#include "HARDdef.h"
#include "delay.h"
#include "makra.h"
#include "I2C.h"
#include "LCD.h"

#define I2C_ACK 1
#define I2C_NACK 0

int main(void)
{
	CLKPR = 1<< CLKPCE; // CLKPR - rejestr procesora odpowiedzialny za preskaler czestotliwosci taktowania
						// najpierw ustawia sie bit R/W CLKPC na 1 i w ciagu 4 cykli CPU naley ustawic ponizszy preskaler
	CLKPR = 1<< CLKPS0; // podzial przez 2
	
	// Konfiguracja portow
	PORT(LCD_DATAPORT) = 1<<I2C_SDA | 1<<I2C_SCL;
	PORT(LCD_CTRLPORT) = ~(1<<LCD_E |1<<LCD_RW | 1<< LCD_ON);
	DDR(LCD_CTRLPORT) = (1<<LCD_E | 1<<LCD_RS | 1<<LCD_RW | 1<< LCD_ON);
	DDR(LCD_DATAPORT) = (0x0F << LCD_D4 | 1 << I2C_SCL); // SCL - zegar		

	lcd_init();
	
	lcd_str_P((const char* PROGMEM)PSTR("DANE z IO:"));
	lcd_command(LCDC_DDA| 64);
	i2c_start();
	i2c_send(0x90); // przeslanie adresu - przydzielenie adresu odbiornikowu
	i2c_send(0b00000011); // bajt konfiguracyjny wysylany do odbiornika - ustawiania z ktorego kanalu ma byc odczyt
	i2c_stop();
	
	
    while(1)
    {
		lcd_command(LCDC_DDA|64);
		i2c_start();
		i2c_send(0x91); // okreslenie ze jest to odczyt
		lcd_dec(i2c_get(I2C_NACK)); // odczyt wyswietlenie z portu
		lcd_str_P((const char* PROGMEM)PSTR(" "));
		i2c_stop();
    }
	
	return 0;
}