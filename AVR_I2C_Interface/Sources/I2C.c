#include <avr\io.h>
#include <inttypes.h>

#include "HARDdef.h"
#include "makra.h"
#include "I2C.h"

#define I2C_nhalf (F_CPU/I2C_SPEED/2)


#if I2C_nhalf < 3
// nic
#elif I2C_nhalf < 8
	static void i2c_xdelay(void)
	{
	NOP();
	}
#else
	#define I2C_delayloops (1+(I2C_nhalf - 8)/3)
	#if I2C_delayloops > 255
		#error ZA WOLNO
	#endif
	static void i2c_xdelay(void)
	{
		asm volatile(\
		"delayus8_loop%=: \n\t"\
		"dec %[ticks] \n\t"\
		"brne delayus8_loop%= \n\t"\
		::[ticks]"r"(I2C_delayloops));
	}
#endif

// I2C - opoznienia
static inline void i2c_hdelay(void)
{
#if I2C_nhalf < 1
	return;
#elif I2C_nhalf < 2
NOP();
#elif I2C_nhalf < 3
asm volatile(
	"rjmp_exit%= \n\t"
	"exit%=:\n\t"::);
#else
	i2c_xdelay();
#endif
}

// Ustawienie wyjscia
static inline void i2c_sdaset(void)
{
	// ustawienie portu i pinu na podciaganie - odpowiednia konfiguracja DDR i PORT
	DDR(I2C_SDAPORT) &= ~(1<<I2C_SDA); 
	PORT(I2C_SDAPORT) |= (1<<I2C_SDA);
}

// Zerowanie wyjscia
static inline void i2c_sdaclear(void)
{
	PORT(I2C_SDAPORT) &= ~(1<<I2C_SDA);
	DDR(I2C_SDAPORT) |= (1<<I2C_SDA);
}

// Pobieranie danej
static inline uint8_t i2c_sdaget(void)
{
	return (PIN(I2C_SDAPORT) & (1<<I2C_SDA));
}

// Ustawienie zegara
static inline void i2c_sclset(void)
{
	PORT(I2C_SCLPORT) |= (1<<I2C_SCL);
}

// zerowanie zegara
static inline void i2c_sclclear(void)
{
	PORT(I2C_SCLPORT) &= ~(1<<I2C_SCL);
}

void i2c_start(void)
{
	// Start bez stopu
	i2c_sdaset(); // SDA - 1 
	i2c_hdelay(); // opoznienie
	i2c_sclset(); // SCL - 1
	i2c_hdelay(); // opoznienie
	
	//Normalny start
	i2c_sdaclear(); // SDA - 0
	i2c_hdelay(); // opoznienie
	i2c_sclclear(); // SCL - 0
}

void i2c_stop(void)
{
	i2c_sdaclear(); // SDA - 0
	i2c_hdelay(); // opoznienie
	i2c_sclset(); // SCL - 1
	i2c_hdelay(); // opoznienie
	i2c_sdaset(); // SDA - 1
	i2c_hdelay(); // opznienie
}

// Wysylanie wartosci
uint8_t i2c_send(uint8_t data)
{
	uint8_t n;
	
	for (n=8; n>0; --n)
	{
		if (data & 0x80)
		{
			i2c_sdaset();
		}
		else
		{
			i2c_sdaclear();
		}
	
		data <<= 1; 
	
		// impuls zegara
		i2c_hdelay();
		i2c_sclset();
		i2c_hdelay();
		i2c_sclclear();
	}
	
	// Potwierdzenie
	i2c_sdaset();
	i2c_hdelay();
	i2c_sclset();
	i2c_hdelay();
	
	n = i2c_sdaget();; // odebranie wartosci na port B od odbiornika - sprawdzenie czy jest 0 czy 1
	i2c_sclclear();
	
	return n;
}

uint8_t i2c_get(uint8_t ack)
{
	uint8_t n;
	uint8_t temp = 0;
	
	i2c_sdaset(); // utrzymywanie podciagania portu
	
	for (n=8; n>0; --n)
	{
		i2c_hdelay();
		i2c_sclset();
		i2c_hdelay();
		temp <<= 1; // przesuwanie bajtu w celu zapisu kolejnej jedynki lub 0 w petli if
		
		if (i2c_sdaget()) // jezeli na porcie jest 1 to temp = 1, a jezlei 0 to temp = 0 
		{
			temp++;
		}
		
		i2c_sclclear();
	}
	
	// ack
	if (ack == I2C_ACK)
	{
		i2c_sdaclear();
	}
	else
	{
		i2c_sdaset();
	}
	
	i2c_hdelay();
	i2c_sclset();
	i2c_hdelay();
	i2c_sclclear();
	
	return temp;
}
