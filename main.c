#include <avr/io.h>
#include <util/delay.h>


#include "ltc1257.h"
#include "uart.h"
#include "rprintf.h"

//X axis,0
#define NSAX 6	//pin nr. signal A/clk
#define NSBX 5	//pin nr. signal B/dir
#define PSAX PIND	//port signal A/clk
#define PSBX PIND	//port signal B/dir

//Y axis,1
#define NSAY 0	//pin nr. signal A/clk
#define NSBY 7	//pin nr. signal B/dir
#define PSAY PINB	//port signal A/clk
#define PSBY PIND	//port signal B/dir


#define SIGNAL_MODE 0 //1=quad 0=clk/dir


inline void signal_init(char *sig_o, char);
char read_signal(char *sig_o, char);
////////////////////////////////////////////////////////////////////	signal
inline void signal_init(char *sig_o, char axis){
	if(SIGNAL_MODE){
		switch(axis){
		case 0:
		*sig_o = ((PSAX & (1<<NSAX)) >> NSAX) | ((PSBX & (1<<NSBX)) >> (NSBX - 0));
		break;
		case 1:
		*sig_o = ((PSAY & (1<<NSAY)) >> NSAY) | ((PSBY & (1<<NSBY)) >> (NSBY - 0));
		break;
		}
	}
	else{
		switch(axis){
		case 0:
		*sig_o = PSAX & (1<<NSAX);
		break;
		case 1:
		*sig_o = PSAY & (1<<NSAY);
		break;
		}
	}
	return;
}

char read_signal(char *sig_o, char axis){
	char A,B;
	if(SIGNAL_MODE){
		switch(axis){
		case 0:
		A = PSAX & (1<<NSAX);
		B = PSBX & (1<<NSBX);
		break;
		case 1:
		A = PSAY & (1<<NSAY);
		B = PSBY & (1<<NSBY);
		break;
		}	
		switch(*sig_o){
			case 0:
				if(A){
					*sig_o = 1;
					return(1);
				}
				else if(B){
					*sig_o = 3;
					return(-1);
				}
				return(0);
			break;
			
			case 1:
				if(!A){
					*sig_o = 0;
					return(-1);
				}
				else if(B){
					*sig_o = 2;
					return(1);
				}
				return(0);		
			break;
			
			case 2:
				if(!A){
					*sig_o = 3;
					return(1);
				}
				else if(!B){
					*sig_o = 1;
					return(-1);
				}
				return(0);		
			break;
			
			case 3:
				if(A){
					*sig_o = 2;
					return(-1);
				}
				else if(!B){
					*sig_o = 0;
					return(1);
				}
				return(0);		
			break;
		}
		return(0);
	}
	else{
		switch(axis){
		case 0:
		A = PSAX & (1<<NSAX);
		B = (PSBX & (1<<NSBX)) ? 1 : -1;
		break;
		case 1:
		A = PSAY & (1<<NSAY);
		B = (PSBY & (1<<NSBY)) ? 1 : -1;
		break;
		}
		if(*sig_o && ! A){
			*sig_o = A;
			return(B);
		}
	
		*sig_o = A;
		return(0);
	}
}


/* Prototypes: */
int main(void);
inline void ioinit(void);

void delay_ms(unsigned int ms)
/* delay for a minimum of <ms> */
{
	// we use a calibrated macro. This is more
	// accurate and not so much compiler dependent
	// as self made code.
	while(ms){
		_delay_ms(0.96);
		ms--;
	}
}


int main(void)
{
	int x = 0;
	int y = 0;
	char sigx_o; // /signal/clk old
	char sigy_o; // /signal/clk old
	int tmp = 0;
	
	ioinit();
	signal_init(&sigx_o,0);
	signal_init(&sigy_o,1);
	
	uartInit();                 // initialize UART (serial port)
    uartSetBaudRate(19200);      // set UART speed to 9600 baud
    rprintfInit(uartSendByte);  // configure rprintf to use UART for output

	while(1)
	{
			x += 1*read_signal(&sigx_o,0);
			y += 1*read_signal(&sigy_o,1);
			
			if(x <= 0)
			x = 0;
			if(x >= 4095)
			x = 4095;
			
			if(y <= 0)
			y = 0;
			if(y >= 4095)
			y = 4095;
			
			if(tmp == 350){
				tmp = 0;
				//rprintf("%d\r\n",i);
				ltc1257_ll_write(4095-y,4095-x);
			}
			tmp++;
		
	}
	return 0;
}

inline void ioinit(void)
{
	/* IO setup: */
	DDRC = (1 << PC3) | (1 << PC4) | (1 << PC5);	/* LTC1257 pins (output) */
	
	/* Low-level init of DAC: */
	ltc1257_ll_init();
}
