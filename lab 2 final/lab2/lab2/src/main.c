/*******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <machine.h>
#include "platform.h"
#include "r_switches.h"

/*******************************************************************************
DEFINES  <System Definitions> , "Defines the useful ports "
*******************************************************************************/
#define   led_on 0
#define   led_off 1
#define   SW1 PORT4.PIDR.BIT.B0
#define   SW2 PORT4.PIDR.BIT.B1
#define   SW3 PORT4.PIDR.BIT.B4
#define   ALL_LEDS_ON {LED4 = LED5 = LED6 = LED7 = LED8 = LED9 = LED10 = LED11 = LED12 = LED13 = LED14 = LED15 = led_on;}
#define   ALL_LEDS_OFF {LED4 = LED5 = LED6 = LED7 = LED8 = LED9 = LED10 = LED11 = LED12 = LED13 = LED14 = LED15 = led_off;}

/*******************************************************************************
* Prototypes for local functions
*******************************************************************************/
void delay(void); 
void delay2(void);
void disp(void);
uint32_t dly = 0;

void main(void)
{
    /* Initialize LCD */
    lcd_initialize(); 
    /* Clear LCD */
    lcd_clear();
    /* Display message on LCD */
    lcd_display(LCD_LINE1, "   Madison   ");
    lcd_display(LCD_LINE2, "    Azhar  ");


    /* 
        STDOUT is routed through the virtual console window tunneled through the JTAG debugger.
        Open the console window in HEW to see the output 
    */
    printf("This is the debug console\r\n");
    
    /* The three pushbuttons on the YRDK board are tied to interrupt lines, set them up here */
    R_SWITCHES_Init();
    
    /* This is the main loop.  It does nothing but call the functions and make sure the LED starts in the off state */
	while (1)
	{
			if(dly ==0)
			{
				ALL_LEDS_OFF;
			}
		else
		{	
				delay();
      			delay2();
    
		}
	}
} /* End of function main() */


//Global Delay counter variable
int dly_ctr;

//delay increment
void delay(void) 
{ 
 for (dly_ctr = 0; dly_ctr < (850*dly); dly_ctr++)
 {
	 ALL_LEDS_ON;			
 } 
} 

//delay deccrement
void delay2(void) 
{ 	
 for (dly_ctr = 0; dly_ctr < (850*(10-dly)); dly_ctr++)
 {
	 	 ALL_LEDS_OFF;			
 }
} 


void disp(void)
{
	uint8_t  lcd_buffer[13];
	if (dly <10)
	{
		sprintf((char *)lcd_buffer, " B Value= 0%d", dly);
		lcd_display(LCD_LINE3, lcd_buffer);
	}
	else
		{
		sprintf((char *)lcd_buffer, " B Value= %d", dly);
		lcd_display(LCD_LINE3, lcd_buffer);
		}
}	



/******************************************************************************
* Function name: sw1_callback
* Description  : Callback function that is executed when SW1 is pressed.
*                Called by sw1_isr in r_switches.c
* Arguments    : none
* Return value : none
******************************************************************************/
void sw1_callback(void)
{
  dly++;
 	if(dly >=10)
	{
		dly = 10;
	}
	if(dly <=0)
	{
		dly = 0;
	}
  disp();
 
  /* Add your code here. Note: this is executing inside an ISR. */
} /* End of function sw1_callback() */


/******************************************************************************
* Function name: sw2_callback
* Description  : Callback function that is executed when SW2 is pressed.
*                Called by sw2_isr in r_switches.c
* Arguments    : none
* Return value : none
******************************************************************************/
void sw2_callback(void)
{
   	if(dly ==0)
	{
		dly = 0;
	}
	
	else 
	dly--;
   

   disp();

} /* End of function sw2_callback() */


/******************************************************************************
* Function name: sw3_callback
* Description  : Callback function that is executed when SW3 is pressed.
*                Called by sw3_isr in r_switches.c
* Arguments    : none
* Return value : none
******************************************************************************/
void sw3_callback(void)
{
    nop(); /* Add your code here. Note: this is executing inside an ISR. */
} /* End of function sw3_callback() */

