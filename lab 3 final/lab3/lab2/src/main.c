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

#define   ALL_LEDS_ON {LED4 = LED5 = LED6 = LED7 = LED8 = LED9 = LED10 = LED11 = LED12 = LED13 = LED14 = LED15 = LED_ON;}
#define   ALL_LEDS_OFF {LED4 = LED5 = LED6 = LED7 = LED8 = LED9 = LED10 = LED11 = LED12 = LED13 = LED14 = LED15 = LED_OFF;}
/*******************************************************************************
* Prototypes for local functions
*******************************************************************************/
void delay(void); 
void delay2(void);
void disp(void);
void S12ADC_init (void);
void S12ADC_temp (void);
void delay3(int micro);
void voltage(void);


uint32_t dly = 0;
uint8_t total = 0;
uint16_t adc_result;
float temp_vol ;


void main(void)
{
    /* Initialize LCD */
    lcd_initialize(); 
    /* Clear LCD */
    lcd_clear();
    /* Display Partner Names on LCD */
    lcd_display(LCD_LINE1, "   Madison   ");
    lcd_display(LCD_LINE2, "    Azhar  ");

   //calling ADC fun
  
 	S12ADC_temp ();
    voltage();
    S12ADC_init();
	
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
		
		  adc_result = S12AD.ADDR2/455;	
   	      total = dly + adc_result;
	      if(total > 100)
		  total = 100;
			
		disp();			 					
	if(total == 0)
			{
				ALL_LEDS_OFF;// sets all LED's off when the total is 0
				delay3(1850);
			}
			else
			{	
				delay();
      			delay2();    
			}				
  	}
} 
/* End of function main() */

// potentiometer registers init
 void S12ADC_init (void)
{
	MSTP(S12AD) = 0;
 	SYSTEM.PRCR.WORD = 0xA50B; // protection on
 	PORT4.PDR.BIT.B2  = 0;    // Set I/O pin direction to input. 
 	PORT4.PMR.BIT.B2  = 0;    //First set I/O pin mode register to GPIO mode.   
 	SYSTEM.MSTPCRA.BIT.MSTPA17 = 0;

 	S12AD.ADCSR.BYTE = 0x4C;  // for continuous scan mode and lowest conversion clock
 	S12AD.ADANS0.WORD = 0x0004; // Read AN002, which is connected to the potentiometer 
 	MPC.P42PFS.BYTE = 0x80;   // Set port function register to analog input, no interrupt. 
 	S12AD.ADCER.WORD = 0x0020;   // Right align data, automatic clearing off. and select formatting
 	S12AD.ADSTRGR.BYTE = 0x00; 
 	S12AD.ADCSR.BIT.ADST = 1; // start conv
 	SYSTEM.PRCR.WORD = 0xA500; // // protection off

}

// Temp registers init
void S12ADC_temp (void)

{
	char temp[13];
	float temperature;
	
	SYSTEM.PRCR.WORD = 0xA50B;		            //protection on	
	SYSTEM.MSTPCRB.BIT.MSTPB8 = 0;				//Enable Module Stop Control
	SYSTEM.MSTPCRA.BIT.MSTPA17 = 0;				//Enable ADC
	delay3(30);	
	S12AD.ADCSR.BYTE = 0x0C;					//continuous scan mode and lowest conversion clock 
	S12AD.ADANS0.WORD = 0x00;		        	//	using A/D channel select register 0 
	S12AD.ADEXICR.BIT.TSS = 1;					//temperature sensor using A/D-Converted Extended Input Control Register
	S12AD.ADSSTR23.BIT.SST2 = 0xFF;				//Set the sampling time of the temperature sensor to the 255 states
	TEMPS.TSCR.BIT.TSEN = 1;					//Enable sensor
	delay3(30);
	TEMPS.TSCR.BIT.TSOE = 1;					//Enable output to ADC
	delay3(30);
	S12AD.ADANS0.WORD = 0x00;					//Disable all channels
	S12AD.ADANS1.WORD = 0x00;					//Disable all channels
	S12AD.ADCSR.BIT.ADST = 1;					//Start Conversion
	
	while(S12AD.ADCSR.BIT.ADST);				//take readings
	
	temperature = S12AD.ADTSDR;					//take temperature reading
	delay3(1);
	temperature = ((temperature*3.3)/(4095));		//temperature voltage formula
	temperature = (temperature-1.26)/(.0041)+25;	//temperature conversion formula
	
	
    sprintf(temp, "T Value=%4.2f", temperature);
	lcd_display(LCD_LINE4, (uint8_t*)temp);			//display temperature
	
	TEMPS.TSCR.BIT.TSOE=0;						//disable output to ADC
	TEMPS.TSCR.BIT.TSEN=0;						//disable temperature sensor
	S12AD.ADEXICR.BIT.TSS=0;					//disable ADC
	SYSTEM.PRCR.WORD = 0xA500;					//protection off
}


// voltage registers init
void voltage(void)
{
	char voltage[13];
	float volts=0;
	
	SYSTEM.PRCR.WORD = 0xA50B;					//protection on
	SYSTEM.MSTPCRA.BIT.MSTPA17 = 0;				//Enable ADC
	S12AD.ADCSR.BYTE = 0x0C;					//continuous scan mode and lowest conversion clock 
	S12AD.ADANS0.WORD = 0x00;					//Disable all channels
	S12AD.ADANS1.WORD = 0x00;					//Disable all channels
	S12AD.ADSTRGR.BIT.ADSTRS = 0x0;             //These bits select the A/D conversion startup source from the on-chip peripheral modules
	S12AD.ADEXICR.BIT.OCS = 1;					//Selects ADC for Internal voltage
	S12AD.ADCSR.BIT.ADST = 1;
	
	while(S12AD.ADCSR.BIT.ADST);	
	volts = S12AD.ADOCDR;
	volts = ((volts*3.3)/(4095.0));
	
	sprintf(voltage, "V Value=%4.2f", volts);
	lcd_display(LCD_LINE5, (uint8_t *)voltage);
	
	S12AD.ADEXICR.BIT.OCS = 0;                   //A/D conversion of A/D internal reference voltage is not performed
	SYSTEM.PRCR.WORD = 0xA500;					//protection off	
}

	
	
//Global Delay counter variable
int dly_ctr;

//delay increment
void delay(void) 
{ 

 for (dly_ctr = 0; dly_ctr < (60*total); dly_ctr++)
 {	
	  ALL_LEDS_ON; 		
 }
} 

//delay decrement
void delay2(void) 
{
	
 for (dly_ctr = 0; dly_ctr < (60*(100-total)); dly_ctr++)
 {	
	  ALL_LEDS_OFF; 		
 }
} 


void delay3(int micro)
{
	for( int i = 0; i<(micro*1000); i++)
	{
	}
	
}
void disp(void)
{


	uint8_t  lcd_buffer[13];
	if (dly <= 100)
		{
			sprintf((char *)lcd_buffer, "B Value= %03d", total);
		
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

  dly = dly +10;
 	if(dly >=100)
	{
		dly = 100;
	}
	if(dly <=0)
	{
		dly = 0;
	}

  /* Add your code here. Note: this is executing inside an ISR. */
} /* End of function sw1_callback() *******************************************/


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
		dly = 0;
	else 
		dly = dly - 10;
		
} 
/* End of function sw2_callback() ********************************************/


/******************************************************************************
* Function name: sw3_callback
* Description  : Callback function that is executed when SW3 is pressed.
*                Called by sw3_isr in r_switches.c
* Arguments    : none
* Return value : none
******************************************************************************/
void sw3_callback(void)
{
    nop(); 
	/* Add your code here. Note: this is executing inside an ISR. */
} 
/* End of function sw3_callback() ********************************************/


