/*
 * Low level access routines to the LTC1257 DAC device.
 */

/*
 * Port, where LTC1257 is connected to:
 * PORTC/PINC
 */

#define DACOUT		PORTC
#define DACIN		PINC

/*
 * Port configuration:
 *
 * Port pin	LTC1257
 * --------------------
 * PC3		LOAD
 * PC4		DATA
 * PC5		CLK
 *
 */

#define DACCLK(LEVEL)	DACOUT = (LEVEL) ? (DACIN & ~_BV(PC5)):(DACIN | _BV(PC5))
#define DACDATA(LEVEL)	DACOUT = (LEVEL) ? (DACIN & ~_BV(PC4)):(DACIN | _BV(PC4))
#define DACLOAD(LEVEL)	DACOUT = (LEVEL) ? (DACIN & ~_BV(PC3)):(DACIN | _BV(PC3))

#define LATCHTIMING	0x01				/* spend some time to the latch	*/
 
#define LEV_LOW		0
#define LEV_HIGH	1

inline void ltc1257_ll_init(void);
inline void ltc1257_ll_write(unsigned int, unsigned int);

/*
 * Low level initialisation routine for LTC1257.
 */	

inline void
ltc1257_ll_init(void)
{
	/* Initial port/pin state */
	DACCLK(LEV_LOW);				/* clock pin low -> idle	*/
	DACLOAD(LEV_HIGH);				/* load pin high -> idle	*/
}

/*
 * Low level write routine for LTC1257.
 * Awaits data in unsigned integer format
 * 12 bits masked (0x800 masks the data's MSB)
 */

inline void
ltc1257_ll_write(unsigned int data1,unsigned int data2)
{
	volatile unsigned char bitctr = 0;
	
	for(bitctr = 0; bitctr < 0x0C; bitctr++)
	{
		DACDATA(data1 & 0x800);			/* output MSB (bits [11..0]!)		*/
		data1 <<= 1;				/* shift next bit to MSB		*/
		DACCLK(LEV_HIGH);			/* rising edge -> load bit		*/
		DACCLK(LEV_LOW);			/* -> await rising edge			*/
	}
	
	for(bitctr = 0; bitctr < 0x0C; bitctr++)
	{
		DACDATA(data2 & 0x800);			/* output MSB (bits [11..0]!)		*/
		data2 <<= 1;				/* shift next bit to MSB		*/
		DACCLK(LEV_HIGH);			/* rising edge -> load bit		*/
		DACCLK(LEV_LOW);			/* -> await rising edge			*/
	}
	
	DACCLK(LEV_LOW);				/* clock pin low -> idle		*/

	DACLOAD(LEV_LOW);				/* load pulled low -> output		*/
	
	for (bitctr = 0; bitctr < LATCHTIMING; bitctr++)
		;
	
	DACLOAD(LEV_HIGH);				/* load pulled high -> idle		*/
}
