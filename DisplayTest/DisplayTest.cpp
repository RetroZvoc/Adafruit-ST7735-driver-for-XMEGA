/*
 * DisplayTest.cpp
 *
 * Created: 21.1.2016. 21:17:01
 *  Author: Zvoc47
 */ 

#include <avr/io.h>
#include "Includes.h"
#include "ST7735xmDriver.h"
#include <avr/pgmspace.h>

ST7735xmDriver tft(&SPIC,&PORTE,0b01000000,&PORTE,0b10000000,&PORTR,0b00000001,&PORTC,0b10110000, 0b00010000,128,160);

static inline void sysclk_init(void)
{
	// enable RC32M and TOSC and wait for ready
	OSC.XOSCCTRL = OSC_XOSCSEL_32KHz_gc;
	OSC.CTRL |= OSC_RC32MEN_bm | OSC_XOSCEN_bm;
	while (~OSC.STATUS & (OSC_RC32MRDY_bm | OSC_XOSCRDY_bm));

	// select DFLL32 reference clock and enable DFLL32
	OSC.DFLLCTRL = OSC_RC32MCREF_XOSC32K_gc;
	DFLLRC32M.CTRL = DFLL_ENABLE_bm;

	// switch to RC32M as system clock source
	CCP = CCP_IOREG_gc;
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;

	// turn off RC2M
	OSC.CTRL &= ~OSC_RC2MEN_bm;

	// select TOSC as RTC clock source
	CLK.RTCCTRL = CLK_RTCSRC_TOSC_gc;
}

#define TermUART USARTE0

static inline void termuart_init()
{
	PORTE_OUTSET=1<<3;
	PORTE_DIRSET=1<<3;
	TermUART.BAUDCTRLA=17;
	TermUART.BAUDCTRLB=0;
	TermUART.CTRLC=USART_CMODE_ASYNCHRONOUS_gc|USART_PMODE_DISABLED_gc|USART_CHSIZE_8BIT_gc;
	TermUART.CTRLB=USART_RXEN_bm|USART_TXEN_bm;
}

int main(void)
{
	sysclk_init();
	termuart_init();
	PORTQ_DIRSET=0b00001000;
	// Use this initializer if you're using a 1.8" TFT
	tft.initR(INITR_BLACKTAB);   // initialize a ST7735S chip, black tab
	for(int y=0;y<160/8;y++)
	{
		for(int x=0;x<128/8;x++)
		{
			tft.setAddrWindow(x*8,y*8,x*8+8,y*8+8);
			for(int i=0;i<64;i++)
			{
				tft.pushColor(i*64);
			}
		}
	}
    while(1)
    {
		PORTQ_OUTTGL=0b00001000;
		_delay_ms(500);
		TermUART.DATA='A';
		tft.invertDisplay(1);
    }
}