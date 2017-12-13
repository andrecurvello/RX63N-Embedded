/*Lab7Final*/

/*******************************************************************************
Includes   <System Includes> , "Project Includes"
*******************************************************************************/
#include <stdint.h>
#include <machine.h>
#include "../r_bsp/mcu/rx63n/iodefine.h"
#include "r_init_clock.h"
#include "r_init_non_existent_port.h"
#include "r_init_stop_module.h"
//#include "typedefine.h"
#ifdef __cplusplus
//#include <ios>                        // Remove the comment when you use ios
//_SINT ios_base::Init::init_cnt;       // Remove the comment when you use ios
#endif

#include <stdbool.h>
#include <stdio.h>
#include <machine.h>
#include "platform.h"
#include "r_switches.h"
//#include <yrdkrx63n.h>
#include "uart.h"
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


/*******************************************************************************
DEFINES  <System Definitions> , "Defines the useful ports "
*******************************************************************************/

#define   ALL_LEDS_ON {LED4 = LED5 = LED6 = LED7 = LED8 = LED9 = LED10 = LED11 = LED12 = LED13 = LED14 = LED15 = LED_ON;}
#define   ALL_LEDS_OFF {LED4 = LED5 = LED6 = LED7 = LED8 = LED9 = LED10 = LED11 = LED12 = LED13 = LED14 = LED15 = LED_OFF;}
#define MAX_TASKS 4
#define NULL ((void *)0)
/*******************************************************************************
* Prototypes for local functions
*******************************************************************************/

void S12ADC_init (void);
void delay3(int micro);
//void S12ADC_temp (void);
//void S12ADC_voltage(void);
void rx (void);
void rtc_init(void);
void rtc_time_read(void);
void DisplayTime(void);
void inrr(void);
void interrup(void);
void led_on(); 
void led_off();
void poti();
void pwm(void);
void RTC_(void);
void command(void);
void Run_RTC_Scheduler(void);
void Init_RTC_Scheduler(void);
void Excep_TMR0_CMI0A(void);
void init_Task_Timers(void);
int Add_Task(void (*task)(void), int time, int priority);

uint32_t dly = 0;
int total = 0;
uint16_t adc_result;
float temp_vol ;
float T = 0;
float V = 0;
char *i;
int dly_ctr;
int flagset = 0;
int num=0;
bool ledstate[12]={true,true,true,true,true,true,true,true,true,true,true,true};
char new_char;
char string[50];
char buffer [30];
char Year1000, Year100, Year10, Year1, Mon10, Mon1, Day10, Day1,
 Day, Hour10, Hour1, Min10, Min1, Sec10, Sec1;
 int v=0;
 int t=0;
char temp[13];
float temperature;
char voltage[13];
float volts=0;
int finished = 0;
uint8_t  lcd_buffer[13];
int cur ;
int pre;

typedef struct {
  int initialTimerValue;
  int timer;
  int run;
  int enabled;
  void (* task)(void);
  } task_t;
task_t GBL_task_table [MAX_TASKS];
int GBL_run_scheduler=0;

void S12ADC_temp ()

{
    SYSTEM.PRCR.WORD = 0xA50B;          //enables writing to MSTPCRB 
    SYSTEM.MSTPCRB.BIT.MSTPB8 = 0;      //enables temperature sensor 
    SYSTEM.MSTPCRA.BIT.MSTPA17 = 0;     //enables ADC 
    SYSTEM.PRCR.WORD = 0xA500;          //disables writing to MSTPCRB 
    S12AD.ADEXICR.WORD = 0x0000; 
    S12AD.ADCSR.BIT.ADIE = 1;           //sets ADC to single-scan mode and enables interrupt 
    S12AD.ADEXICR.WORD = 0x0100;        //selects ADC for temperature sensor 
    S12AD.ADSSTR23.WORD = 0xFF14;       //sets sampling time of temp sensor to 255 
    TEMPS.TSCR.BIT.TSEN = 1;            //starts temperature sensor operation 
    delay3(3000);                   	//wait for 30 microseconds 
    TEMPS.TSCR.BIT.TSOE = 1;            //enables  temperature sensor 
    delay3(100);                   		 //wait for 1 microseconds 
    S12AD.ADCER.BIT.ADRFMT = 0;         // right flush 
    S12AD.ADANS0.WORD = 0x0000;         //disable adc channels 
    S12AD.ADANS1.WORD = 0x0000; 
    interrup(); 
    S12AD.ADCSR.BIT.ADST = 1;           //starting AD conversion  
}


//Voltage  Init
void S12ADC_voltage()
{
	SYSTEM.PRCR.WORD = 0xA50B;          //enables writing to MSTPCRA 
    SYSTEM.MSTPCRA.BIT.MSTPA17 = 0;     //enables ADC 
    SYSTEM.PRCR.WORD = 0xA500;          //disables writing to MSTPCRB 
    S12AD.ADEXICR.WORD = 0x0000; 
    S12AD.ADCSR.BIT.ADIE = 1;           // ADC to single scan mode and enables interrupt 
    S12AD.ADEXICR.WORD = 0x0200;        // ADC for internal reference voltage 
    S12AD.ADANS0.WORD = 0x0000;         //disabling all ADC channels 
    S12AD.ADANS1.WORD = 0x0000; 
    S12AD.ADCSR.BIT.ADST = 1;           //starting AD conversion  
	
}

#pragma interrupt (inrr (vect =102))
void inrr(void)
{

	 if(!t) { 
        temperature = S12AD.ADTSDR & 0x0FFF; 
        IR(S12AD0, S12ADI0) = 0;  //set so that no interrupt request is currently written 
        t = 1; 
        printf("Temp\n"); 
    } 
    if(!v) 
    { 
        volts = S12AD.ADOCDR & 0x0FFF; 
        IR(S12AD0, S12ADI0) = 0;  //set so that no interrupt request is currently written 
        v = 1; 
        printf("vol\n"); 
    } 
}

void main(void)
{
	
	i = (char*)calloc(10,sizeof(char));
    /* Initialize LCD */
    lcd_initialize(); 
    /* Clear LCD */
    lcd_clear();
    /* Display Partner Names on LCD */
    lcd_display(LCD_LINE1, "   Madison   ");
    lcd_display(LCD_LINE2, "    Azhar  ");
	  
   //calling ADC function
   	S12ADC_temp ();
	rtc_init();
	
	
	/* 
        STDOUT is routed through the virtual console window tunneled through the JTAG debugger.
        Open the console window in HEW to see the output 
    */
   	printf("This is the debug console\r\n");
    
    /* The three pushbuttons on the YRDK board are tied to interrupt lines, set them up here */
    R_SWITCHES_Init();
   
   /* Initialize the SCI channel for asynchronous UART communications. */
    sci_uart_init(); 
    sci_tx_int_enable();
    sci_rx_int_enable();
	
	cur = RTC.RMINCNT.BIT.MIN1;
	pre = cur;

	init_Task_Timers();
	Add_Task(pwm,963,0);
	Add_Task(poti,20000,1);
	Add_Task(RTC_,592,2);
	Add_Task(command,296,3);
	Init_RTC_Scheduler();
	Run_RTC_Scheduler();
    /* This is the main loop.  It does nothing but call the functions and make sure the LED starts in the off state */
	
} 
/* End of function main() */

	
static uint32_t crc32_tab[] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
	0xe963a535, 0x9e6495a3,	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
	0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
	0xf3b97148, 0x84be41de,	0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,	0x14015c4f, 0x63066cd9,
	0xfa0f3d63, 0x8d080df5,	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
	0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,	0x35b5a8fa, 0x42b2986c,
	0xdbbbc9d6, 0xacbcf940,	0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
	0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
	0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,	0x76dc4190, 0x01db7106,
	0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
	0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
	0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
	0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
	0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
	0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
	0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
	0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
	0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
	0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
	0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
	0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
	0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
	0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
	0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
	0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
	0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
	0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
	0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
	0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
	0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
	0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
	0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
	0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

/*CRC32 Function*/
uint32_t crc32(uint32_t crc, const void *buf, size_t size)
{
	const uint8_t *p;

	p = buf;
	crc = crc ^ ~0U;

	while (size--)
		crc = crc32_tab[(crc ^ *p++) & 0xFF] ^ (crc >> 8);

	return crc ^ ~0U;
}

// potentiometer registers init
 void S12ADC_init (void)
  
{ 
    SYSTEM.PRCR.WORD = 0xA50B;          //enables writing to MSTPCRA 
    SYSTEM.MSTPCRA.BIT.MSTPA17 = 0;     //enables ADC 
    SYSTEM.PRCR.WORD = 0xA500;          //disables writing to MSTPCRA 
    S12AD.ADEXICR.WORD = 0x0000; 
    PORT4.PDR.BIT.B2 = 0;               //setting  to input 
    PORT4.PMR.BIT.B2 = 1;               //setting to analog 
    S12AD.ADCSR.BYTE = 0x4C;            //sets coninuous scan
    S12AD.ADANS0.WORD = 0x0004;         //poti connected 
    S12AD.ADCER.BIT.ACE = 1;            // clear values to 0 after values are used by ADC  
    S12AD.ADCER.BIT.ADRFMT = 0;         //flush right 
    S12AD.ADSTRGR.BIT.ADSTRS = 0x0;     
    S12AD.ADCSR.BIT.ADST = 1;           //start the conversion 
} 
	



// Temp registers init


void interrup(void)
{
	IPR(S12AD0,S12ADI0) = 0x01;
	IEN(S12AD0,S12ADI0)=1;
	
}
void rx (void)
		{
			char cmd[9];
			int count;
		 new_char= sci_get_char();	
 	   /* Get the char waiting in buffer. */
		i[num] = new_char;
		sprintf(string, "\r\nRECIEVED %c\r\nnum = %d", i[num], num);
		printf(string);
		num++;
	if(new_char == ';')
	{
		i[num] = ';';
		sprintf(string, "\r\nnum = %d", num);
		printf(string);
		for(count=0;count<(num);count++)
		{
			if(i[count] == ',' | i[count] == ';')
			{
				cmd[0] = i[count - 8];
				cmd[1] = i[count - 7];
				cmd[2] = i[count - 6];
				cmd[3] = i[count - 5];
				cmd[4] = i[count - 4];
				cmd[5] = i[count - 3];
				cmd[6] = i[count - 2];
				cmd[7] = i[count - 1];
				cmd[8] = '\0';
				//Parse Commands for LED
					if(strcmp(cmd,"LED04=01") == 0)
					{
						ledstate[0]= true;
					}else if(strcmp(cmd,"LED04=00")== 0)
					{
						ledstate[0] = false;
					}
						if(strcmp(cmd,"LED05=01") == 0)
					{
						ledstate[1]= true;
					}else if(strcmp(cmd,"LED05=00") == 0)
					{
						ledstate[1] = false;
					}
					if(strcmp(cmd,"LED06=01") == 0)
					{
						ledstate[2]= true;
					}else if(strcmp(cmd,"LED06=00") == 0)
					{
						ledstate[2] = false;
					}
					if(strcmp(cmd,"LED07=01") == 0)
					{
						ledstate[3]= true;
					}else if(strcmp(cmd,"LED07=00") == 0)
					{
						ledstate[3] = false;
					}
					if(strcmp(cmd,"LED08=01") == 0)
					{
						ledstate[4]= true;
					}else if(strcmp(cmd,"LED08=00") == 0)
					{
						ledstate[4] = false;
					}		
					if(strcmp(cmd,"LED09=01") == 0)
					{
						ledstate[5]= true;
					}else if(strcmp(cmd,"LED09=00") == 0)
					{
						ledstate[5] = false;
					}
					if(strcmp(cmd,"LED10=01") == 0)
					{
						ledstate[6]= true;
					}else if(strcmp(cmd,"LED10=00") == 0)
					{
						ledstate[6] = false;
					}			
					if(strcmp(cmd,"LED11=01") == 0)
					{
						ledstate[7]= true;
					}else if(strcmp(cmd,"LED11=00") == 0)
					{
						ledstate[7] = false;
					}	
					if(strcmp(cmd,"LED12=01") == 0)
					{
						ledstate[8]= true;
					}else if(strcmp(cmd,"LED12=00") == 0)
					{
						ledstate[8] = false;
					}	
					if(strcmp(cmd,"LED13=01") == 0)
					{
						ledstate[9]= true;
					}else if(strcmp(cmd,"LED13=00") == 0)
					{
						ledstate[9] = false;
					}					
					if(strcmp(cmd,"LED14=01") == 0)
					{
						ledstate[10]= true;
					}else if(strcmp(cmd,"LED14=00") == 0)
					{
						ledstate[10] = false;
					}	
					if(strcmp(cmd,"LED15=01") == 0)
					{
						ledstate[11]= true;
					}else if(strcmp(cmd,"LED15=00") == 0)
					{
						ledstate[11] = false;
					}	
			
				//Parse Commands for PWM			
					if(strcmp(cmd,"PWM01=00") == 0)
					{
						dly = 00;
					}else if(strcmp(cmd,"PWM01=01" ) == 0)
					{
						dly= 10;
					}else if(strcmp(cmd,"PWM01=02") == 0)
					{
						dly= 20;
					}else if(strcmp(cmd,"PWM01=03") == 0)
					{
						dly= 30;
					}else if(strcmp(cmd,"PWM01=04") == 0)
					{
						dly= 40;
					}else if(strcmp(cmd,"PWM01=05") == 0)
					{
						dly= 50;
					}else if(strcmp(cmd,"PWM01=06") == 0)
					{
						dly= 60;
					}else if(strcmp(cmd,"PWM01=07") == 0)
					{
						dly= 70;
					}else if(strcmp(cmd,"PWM01=08") == 0)
					{
						dly= 80;
					}else if(strcmp(cmd,"PWM01=09") == 0)
					{
						dly= 90;
					}else if(strcmp(cmd,"PWM01=10") == 0)
					{
						dly= 100;
					}
			}
		}
		free(i);//frees i to be reallocated
		num=0;//resets count	
		i = (char*)calloc(10,sizeof(char));//reallocates memory	
	}				
					
	if(num%10 ==0)
	{
		i= realloc(i,10+num);// when the counter reaches the end of the buffer reallocate more memory
	}			
        sci_put_char(new_char);   /* Transmit the same character back. */

		   
	}  	 
	 

//delay increment
void led_on() 
{ 

	 /*CONTROLS THE LEDS to turn on or off depending on value of boolean*/
	 if (ledstate[0]==true) //led4
	 	{
		 LED4=0;// led4 on
	 	}
	 else
		{
			LED4=1;
		}
	 if (ledstate[1]==true) //led5
	 	{
		 	LED5=0;// led5 on
	 	}
	 else
		{
			LED5=1;
		}
	 if (ledstate[2]==true) //led6
	 	{
		 	LED6=0;// led6 on
	 	}
	 else
		{
			LED6=1;
		}	
	 if (ledstate[3]==true) //led7
	 	{
		 	LED7=0;// led7 on
	 	}
	 else
		{
			LED7=1;
		}	
	 if (ledstate[4]==true) //led8
	 	{
		 	LED8=0;// led8 on
	 	}
	 else
		{
			LED8=1;
		}
	 if (ledstate[5]==true) //led9
	 	{
		 	LED9=0;// led9 on
	 	}
	 else
		{
			LED9=1;
		}
	 if (ledstate[6]==true) //led10
	 	{
		 	LED10=0;// led10 on
	 	}
	 else
		{
			LED10=1;
		}
	 if (ledstate[7]==true) //led11
	 	{
		 	LED11=0;// led11 on
	 	}
	 else
		{
			LED11=1;
		}
	 if (ledstate[8]==true) //led12
	 	{
		 	LED12=0;// led12 on
	 	}
	 else
		{
			LED12=1;
		}
	 if (ledstate[9]==true) //led13
	 	{
		 	LED13=0;// led13 on
	 	}
	 else
		{
			LED13=1;
		}
	 if (ledstate[10]==true) //led14
	 	{
		 	LED14=0;// led14 on
	 	}
	 else
		{
			LED14=1;
		}
	 if (ledstate[11]==true) //led15
	 	{
		 	LED15=0;// led15 on
	 	}
	 else
		{
			LED15=1;
		}

} 

//delay decrement
void led_off() 
{		
	  ALL_LEDS_OFF; 
} 


void delay3(int micro)
{
	for( int i = 0; i<(micro*1000); i++)
	{
	}
	
}
	

void rtc_init(void)
{
    uint8_t i;
    volatile uint8_t dummy;

    /* ==== Counters and prescaler are stopped ==== */
    RTC.RCR2.BYTE &= 0x7E;
    while (0 != RTC.RCR2.BIT.START)
    {
        /* Confirm that the written value can be read correctly. */
    }

    /* ==== Set the time ==== */
    RTC.RSECCNT.BYTE = 0x00;            /* Set the BCD-coded value: 00 second */
    RTC.RMINCNT.BYTE = 0x01;            /* Set the BCD-coded value: 00 minute */
    RTC.RHRCNT.BYTE = 0x16;             /* Set the BCD-coded value: 00 hour */
    RTC.RWKCNT.BYTE = 0x00;             /* Set the day of the week: Tuesday */
    RTC.RDAYCNT.BYTE = 0x20;            /* Set the BCD-coded value: 01 day */
    RTC.RMONCNT.BYTE = 0x04;            /* Set the BCD-coded value: 01 month */
    RTC.RYRCNT.WORD = 0x0014;           /* Set the BCD-coded value: 13 year */

    RTC.RCR2.BIT.HR24 = 1;              /* Hours Mode: 24-hour mode */
    for (i = 0; i < 3; i++)             /* Dummy read three times */
    {
         dummy = RTC.RCR2.BYTE;
    }

    /* ==== Set the interrupt ==== */
    RTC.RCR1.BYTE = 0x00;               /* Disable RTC interrupt requests */
    while (0x00 != RTC.RCR1.BYTE)
    {
        /* Confirm that the written value can be read correctly. */
    }

    //IR(RTC,CUP) = 0;                    /* Clear RTC.CUP interrupt request */
//    IR(RTC,ALM) = 0;                    /* Clear RTC.ALM interrupt request */
 //   IR(RTC,PRD) = 0;                    /* Clear RTC.PRD interrupt request */

    RTC.RCR1.BYTE = 0xD2;               /* A carry interrupt request is enabled */
                                        /* Periodic interrupt is generated every 1/2 second */
    while (0xD2 != RTC.RCR1.BYTE)
    {
        /* Confirm that the written value can be read correctly. */
    }

    /* ==== Counters and prescaler operate normally ==== */
    RTC.RCR2.BIT.START = 1;
    while (1 != RTC.RCR2.BIT.START)
    {
        /* Confirm that the written value can be read correctly. */
    }
	 IEN(RTC,COUNTUP) = 0; //Disable carry interrupt in ICU
 RTC.RCR1.BIT.CIE = 1; //Enable carry interrupt in the RTC
}

/*******************************************************************************
* Outline      : RTC time data reading
* Header       : none
* Function Name: rtc_time_read
* Description  : Stores to global variables reading the time information.
* Arguments    : none
* Return Value : none
*******************************************************************************/
void rtc_time_read(void)
{
 do {
 IR(RTC,COUNTUP) = 0; //Clear the Carry Flag
 Year10 = RTC.RYRCNT.BIT.YR10;
 Year1 = RTC.RYRCNT.BIT.YR1;
 Mon10 = RTC.RMONCNT.BIT.MON10;
 Mon1 = RTC.RMONCNT.BIT.MON1;
 Day10 = RTC.RDAYCNT.BIT.DATE10;
 Day1 = RTC.RDAYCNT.BIT.DATE1;
 Day = RTC.RWKCNT.BYTE;
 Hour10 = RTC.RHRCNT.BIT.HR10;
  Hour1 = RTC.RHRCNT.BIT.HR1;
 Min10 = RTC.RMINCNT.BIT.MIN10;
 Min1 = RTC.RMINCNT.BIT.MIN1;
 Sec10 = RTC.RSECCNT.BIT.SEC10;
 Sec1 = RTC.RSECCNT.BIT.SEC1;
	 } while(IR(RTC,COUNTUP) == 1);
 }

 void DisplayTime(void)
 {	 
// char buffer [30];
 // char DayNames[7]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
rtc_time_read();
 //lcd_display(LCD_LINE1, "RTC Example");
 sprintf((char *)buffer, "20%d%d %d%d / %d%d", Year10,Year1,Mon10,Mon1,Day10,Day1);
 //lcd_display(LCD_LINE2, buffer);
 //lcd_display(LCD_LINE3, DayNames[Day]);
 sprintf((char *)buffer, "%d%d:%d%d:%d%d",
 Hour10,
 Hour1,
 Min10,
 Min1,
 Sec10,
 Sec1);
// lcd_display(LCD_LINE4, buffer);
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
	
		sprintf((char *)lcd_buffer, "B Value= %03d", dly);
		
			lcd_display(LCD_LINE3, lcd_buffer);
		
	//disp();

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
	//	disp();
		sprintf((char *)lcd_buffer, "B Value= %03d", dly);
		
		lcd_display(LCD_LINE3, lcd_buffer);
		
		
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
char s5[13];
	char s6[13];
	char s7[13];
	char s8[50];
	char tottString[60]="";
	char crc_string[70];
	
	char s2[13];
	char s3[13];
	char s4[13];
	char bString[20]= "B Value=" ;
	char tString[20] = "T Value=";
	char vString[20]= "V Value=";
	char totString[60]= "";
	DisplayTime();
	
	sprintf(s5, "20%d%d", Year10,Year1);
	sprintf(s6,"%d%d",Mon10,Mon1);
	sprintf(s7,"%d%d",Day10,Day1);
	sprintf(s8, "%d%d:%d%d:%d%d",Hour10,Hour1,Min10,Min1,Sec10,Sec1);
	
	strcat(tottString,s5);
	strcat(tottString,"-");
	strcat(tottString,s6);
	strcat(tottString,"-");
	strcat(tottString,s7);
	strcat(tottString,"T");
	strcat(tottString,s8);
	strcat(tottString,":");
	
	
	sprintf(s2,"%0.3d", total);
	sprintf(s3,"%4.1f",T );
	sprintf(s4,"%4.2f",volts);

	strcat(bString,s2);
	strcat(tString,s3);
	strcat(vString,s4);
		    
					 
	strcat(totString,bString);
	strcat(totString, ";");
    strcat(totString,tString);
	strcat(totString, ";");
	strcat(totString,vString);

	
	strcat(tottString,totString);
	sci_put_string( "\r\n");
	sci_put_string((uint8_t *)tottString);
	sci_put_string( "\r\n");
	
	/*CRC String Output*/				
	sprintf(crc_string, "CRC-32:%8.8X\r\n", crc32(0x00000000,tottString,strlen(tottString)));			
    sci_put_string((uint8_t *)crc_string); 		 
    
} 
/* End of function sw3_callback() ********************************************/

void poti(void)
{
	adc_result = S12AD.ADDR2/455;	
   	      total = dly  + adc_result;
	      if(total >= 100)
		  total = 100;
		  
		 	if (dly <= 100)
		{
			sprintf((char *)lcd_buffer, "B Value= %03d", total);
		
			lcd_display(LCD_LINE3, lcd_buffer);
		}
}


void pwm(void)

{
	
	 if(total == 0)
	{
			ALL_LEDS_OFF;// sets all LED's off when the total is 0
		
	}
	 else
	{		led_on();
		delay3(total);
	 	
			led_off();
		delay3(100-total); 
			
	 }	
		
		
		 
	
     int tim; 
    tim = GBL_task_table[0].timer; 
    printf("\nTimer = %d",tim); 			
}

void RTC_(void)
{
	
		 cur = RTC.RMINCNT.BIT.MIN1;
		 if (cur != pre)
		{
			sw3_callback();
			pre = cur;
		}
		
}

void command()
{
	while((sci_read_count_get())>0) // when there are characters in the buffer
 		{
			rx();
		}
}

  
void init_Task_Timers(void) 
{ 
    int i; 
    /* Initialize all tasks */
    for(i = 0; i < MAX_TASKS; i++) 
    { 
        GBL_task_table[i].initialTimerValue = 0; 
        GBL_task_table[i].run = 0; 
        GBL_task_table[i].timer = 0; 
        GBL_task_table[i].enabled = 0; 
        GBL_task_table[i].task = NULL; 
    } 
 } 
  
  
 int Add_Task(void (*task)(void), int time, int priority) 
 { 
     /* Check for valid priority */
     if(priority >= MAX_TASKS || priority < 0) 
        return 0; 
     /* Check to see if we are overwriting an already scheduled task */
     if(GBL_task_table [priority ].task != NULL)//420 EMBEDDED SYSTEMS USING THERENESASRX63N MICROCONTROLLER 
        return 0; 
     /* Schedule the task */
     GBL_task_table[priority].task = task; 
     GBL_task_table[priority].run = 0; 
     GBL_task_table[priority].timer = time; 
     GBL_task_table[priority].enabled = 1; 
     GBL_task_table[priority].initialTimerValue = time; 
     return 1; 
} 
  void Init_RTC_Scheduler(void) 
 { 
      
    #ifdef PLATFORM_BOARD_RDKRX63N 
    SYSTEM.PRCR.WORD = 0xA50B; /* Protect off */
    #endif 
  
    /* Power up the TMR unit using the appropriate Module Stop Control Register */
    MSTP(TMR0) = 0; 
          
    #ifdef PLATFORM_BOARD_RDKRX63N 
    SYSTEM.PRCR.WORD = 0xA500; /* Protect on  */
    #endif    
  
    TMR0.TCR.BYTE = 0x42;   
    TMR1.TCR.BYTE = 0x00; 
  
    TMR0.TCCR.BYTE = 0x08; 
      
    TMR0.TCSR.BYTE = 0xE0; 
    TMR0.TCORA= 240; /* delay length in timer counts. 5us */ 
         
    TMR01.TCNT = 0; 
    
    IR (TMR0, CMIA0) = 0; /* interrupt reset */
    IPR(TMR0, CMIA0) = 4; /* interrupt priority set */
    IEN(TMR0, CMIA0) = 1; /* interrupt enable */
      
} /* End of function tmr_start() */
       
   
   
   
   
 #pragma interrupt (Excep_TMR0_CMI0A(vect = VECT(TMR0, CMIA0)))     
  void Excep_TMR0_CMI0A(void) 
 { 
static char i;    
//PORTD.PODR.BYTE = 00; 
        for (i=0 ; i<MAX_TASKS ;i++)  
        { 
                                                       
            if (GBL_task_table[i].task != NULL)  
            { 
            if (GBL_task_table[i].enabled == 1)  
            { 
        //  printf( "1"); 
            if (GBL_task_table[i].timer > 0)  
            { 
            //printf("\n3"); 
            if (--(GBL_task_table[i].timer) == 0) 
            { 
        //printf("\n4"); 
  
        GBL_task_table[i].run = 1; 
  
    
        GBL_task_table[i].timer = GBL_task_table[i].initialTimerValue; 
  
            } 
            }  
            }  
            } 
        }  
    } 
    
 void Run_RTC_Scheduler(void) 
 { 
    int i; 
    /* Loop forever */
    while(1) 
    {    
       if(!finished && t) 
        { 
		
			temperature = S12AD.ADTSDR;					//take temperature reading
			temperature = ((temperature*3.3)/(4095));		//temperature voltage formula
			temperature = (temperature-1.26)/(.0041)+25;	//temperature conversion formula
			T = temperature; 
		    sprintf(temp, "T Value=%4.2f", temperature);
			lcd_display(LCD_LINE4, (uint8_t*)temp);			//display temperature

            TEMPS.TSCR.BIT.TSOE = 0;    //disables output from the temperature sensor 
            TEMPS.TSCR.BIT.TSEN = 0;    //stops temperature sensor operation   
         
			S12ADC_voltage();
		    printf("t\n"); 
        } 
        if(!finished && v) 
        { 
			volts = S12AD.ADOCDR & 0x0FFF;
			volts = ((volts*3.3)/(4095.0));
			V = volts;
			sprintf(voltage, "V Value=%4.2f", volts);
			lcd_display(LCD_LINE5, (uint8_t *)voltage);

            finished = 1; 
            t = v = 0; 
            printf("v"); 
            S12ADC_init ();
        } 
		 
	
                   
     /* Check each task */
     for(i = 0; i < MAX_TASKS; i++) 
     { 
         /* check if valid task */
         if(GBL_task_table[i].task != NULL) 
         { 
                     /* check if enabled */
             if(GBL_task_table[i].enabled == 1) 
             { 
                 /* check if ready to run */
                 if(GBL_task_table[i].run == 1) 
                 { 
                     /* Reset the run ?ag */
                     GBL_task_table[i].run = 0; 
                     /* Run the task */
                     GBL_task_table[i].task(); 
                     /* break out of loop to start at entry 0 */
                     break; 
                 } 
             } 
          } 
        } 
     } 
 }