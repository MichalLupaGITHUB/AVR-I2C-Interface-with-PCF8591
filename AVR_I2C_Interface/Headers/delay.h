#ifndef DELAY_H_INCLUDED
#define DELAY_H_INCLUDED

// opoznienie - 250 ns (przy taktowaniu 4MHz)
#define delay250ns() {asm volatile("nop"::);}
	
// skok "rjmp" do nastepnej instrukcji zawiera 2 cykle procesora
#define delay500ns() {asm volatile("rjmp exit%=\n\t" "exit%=\n\t"::);}

#define delayus8(t)\
{asm volatile( \
	"delayus8_loop%=: \n\t"\
	"nop \n\t"\
	"dec %[ticks] \n\t"\
	"brne delayus8_loop%= \n\t"\
: :[ticks]"r"(t) );}
// nop - 1 cykl, brne - 2 cykle, dec - 1 cykl wiec sumarycznie 4 cykle -> 4 cykle przy taktowaniu 4MHz to 4*250ns = 1000 ns = 1us

void delay100us8(uint8_t t); // 1us -> z funkcji * (t= 100) = 100us, maksymalne opoznienienie wynika z wartosci uint8_t -> 256 * 1 us

#endif /* DELAY_H_INCLUDED */