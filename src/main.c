/**
  ******************************************************************************
  * @file    main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    19-September-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usbd_hid_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_adc.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_exti.h"
#include "string.h"

//Keyboard_defines
# define ilosc_probek 50
#define ilosc_wejsc 6
double prog = 0.85; //im wiecej tym mniejsza szansa na 1
#define UP_ARROW '5'
#define DOWN_ARROW '6'
#define LEFT_ARROW '7'
#define RIGHT_ARROW '8'
#define SPACEBAR '9'
double prog_pusc = 0.6;
int licznik=0;
//end
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment = 4
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;


/* Private function prototypes -----------------------------------------------*/
static uint32_t Demo_USBConfig(void);
static void TIM3_Config(void);
//struktury
typedef struct {
	int  pomiar[ilosc_probek];
	int najstarszy_pomiar;
	int pressed;
	int suma;

	GPIO_TypeDef port;
	uint16_t pin;


} pomiary;

typedef struct {
	uint8_t keycode[4];
} keyboard_report_t;

//end
//zmienne
int obecny_indeks=0;
GPIO_TypeDef *porty[ilosc_wejsc] = {
		GPIOC,
		GPIOC,
		GPIOE,
		GPIOB,
		GPIOB,
		GPIOB,
		GPIOD
};
uint16_t piny[ilosc_wejsc]={
		GPIO_Pin_15,
		GPIO_Pin_13,
		GPIO_Pin_5,
		GPIO_Pin_7,
		GPIO_Pin_5,
		GPIO_Pin_3,
};



char *keycodes[ilosc_wejsc]={
		UP_ARROW, 		// PC15
		DOWN_ARROW, 	// PC13
		LEFT_ARROW, 	// PE5
		RIGHT_ARROW, 	// PB7
		'z', 			//PB5
		'x', 			//PB3
};
int pressed;
pomiary wejscia[ilosc_wejsc];
keyboard_report_t keyboard_report;

//end

//funkcje
void gpio_init()
{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH , ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC , ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE , ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB , ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD , ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE);
int i;

for(i=0;i<ilosc_wejsc;i++)
{

	GPIO_InitTypeDef  GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Pin = piny[i];
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(porty[i], &GPIO_InitStructure);
}

GPIO_InitTypeDef GPIO_InitStructure;
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
GPIO_Init(GPIOD, &GPIO_InitStructure);

GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
GPIO_Init(GPIOA, &GPIO_InitStructure);

}

void exti_init()
{
	NVIC_InitTypeDef NVIC_InitStructure;
	// numer przerwania
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	// priorytet g³ówny
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	// subpriorytet
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	// uruchom dany kana³
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// zapisz wype³nion¹ strukturê do rejestrów
	NVIC_Init(&NVIC_InitStructure);

	EXTI_InitTypeDef EXTI_InitStructure;
	// wybór numeru aktualnie konfigurowanej linii przerwañ
	EXTI_InitStructure.EXTI_Line = GPIO_Pin_0;
	// wybór trybu - przerwanie b¹dŸ zdarzenie
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	// wybór zbocza, na które zareaguje przerwanie
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	// uruchom dan¹ liniê przerwañ
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	SYSCFG_EXTILineConfig(GPIOA, EXTI_PinSource0);
}

void EXTI0_IRQHandler(void)
{
 if(EXTI_GetITStatus(EXTI_Line0) != RESET)
 {
	 int i;
	for (i=0;i<1000000;i++)
	{
		asm("nop");
	}
	int x=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
		 if(x)
		 switch(licznik++)
		 {
			case 0:
				GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
				GPIO_SetBits(GPIOD, GPIO_Pin_12);
				prog=0.7;
				prog_pusc=0.5;
				break;
			case 1:
				GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
				GPIO_SetBits(GPIOD, GPIO_Pin_13);
				prog=0.8;
				prog_pusc=0.55;
				break;
			case 2:
				GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
				GPIO_SetBits(GPIOD, GPIO_Pin_14);
				prog=0.85;
				prog_pusc=0.6;
				break;
			case 3:
				GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
				GPIO_SetBits(GPIOD, GPIO_Pin_15);
				prog=0.9;
				prog_pusc=0.65;
				licznik=0;
				break;
		}
 EXTI_ClearITPendingBit(EXTI_Line0);
 }
}


static uint32_t Demo_USBConfig(void)
{
  USBD_Init(&USB_OTG_dev,
            USB_OTG_FS_CORE_ID,
            &USR_desc,
            &USBD_HID_cb,
            &USR_cb);

  return 0;
}
double test;
void keydown()
{
	keyboard_report.keycode[0] = 0;
	keyboard_report.keycode[1] = 0;
	keyboard_report.keycode[2] = 0;
	keyboard_report.keycode[3] = 0;

	USBD_HID_SendReport (&USB_OTG_dev, &(keyboard_report.keycode[0]), 4);

}

void wypisz()
{
	double prog1 = prog;
	int i;
	char s[ilosc_wejsc];
	int wyslane = 0;
	for(i=0;i<ilosc_wejsc;i++)
	{
		if((wejscia[i].suma/ilosc_probek)>=prog1)
		{
			strcat(s,&keycodes[i]);
			wejscia[i].pressed=1;
			wyslane++;
		}


	}
	if(wyslane!=0)
	{
		napisz(s);
	}


	pressed=wyslane;
}
void pusc()
{
	int i;
	for(i=0;i<ilosc_wejsc;i++)
		{
			if((wejscia[i].suma/ilosc_probek)<=prog_pusc && wejscia[i].pressed==1)
			{
				pusc2(&keycodes[i]);
				wejscia[i].pressed=0;

			}


		}

}
void pusc2(char *s)
{


int wartosc;
int i;
	for(i=0;i<strlen(s);i++)
	{
		wartosc=s[i];
	if(s[i]==RIGHT_ARROW)
	{
		keyboard_report.keycode[3]-=16;
	}
	else if(s[i]==LEFT_ARROW)
	{
		keyboard_report.keycode[3]-=32;
	}
	else if(s[i]==UP_ARROW)
	{
		keyboard_report.keycode[3]-=128;
	}
	else if(s[i]==DOWN_ARROW)
	{
		keyboard_report.keycode[3]-=64;
	}
	else if (s[i]==SPACEBAR)
	{
		keyboard_report.keycode[0] -= 2;

	}
	else if(wartosc>='a' && wartosc<='f')
	{
		keyboard_report.keycode[0] -= pow(2,(wartosc-95));

	}
	else if(s[i]>='g' && s[i]<='n')
	{
		keyboard_report.keycode[1] -= pow(2,(wartosc-103));

	}
	else if(s[i]>='o' && s[i]<='v')
	{
			keyboard_report.keycode[2] -= pow(2,(wartosc-111));

	}
	else if(s[i]>='w' && s[i]<='z')
	{
			keyboard_report.keycode[3] -= pow(2,(wartosc-119));

	}
	}

	USBD_HID_SendReport (&USB_OTG_dev, &(keyboard_report.keycode[0]), 4);
}


void napisz(char *s)
{

	keyboard_report.keycode[0] = 0;
	keyboard_report.keycode[1] = 0;
	keyboard_report.keycode[2] = 0;
	keyboard_report.keycode[3] = 0;

int wartosc;
int i;
	for(i=0;i<strlen(s);i++)
	{
		wartosc=s[i];
	if(s[i]==RIGHT_ARROW)
	{
		keyboard_report.keycode[3]+=16;
	}
	else if(s[i]==LEFT_ARROW)
	{
		keyboard_report.keycode[3]+=32;
	}
	else if(s[i]==UP_ARROW)
	{
		keyboard_report.keycode[3]+=128;
	}
	else if(s[i]==DOWN_ARROW)
	{
		keyboard_report.keycode[3]+=64;
	}
	else if (s[i]==SPACEBAR)
	{
		keyboard_report.keycode[0] += 2;

	}
	else if(wartosc>='a' && wartosc<='f')
	{
		keyboard_report.keycode[0] += pow(2,(wartosc-95));

	}
	else if(s[i]>='g' && s[i]<='n')
	{
		keyboard_report.keycode[1] += pow(2,(wartosc-103));

	}
	else if(s[i]>='o' && s[i]<='v')
	{
			keyboard_report.keycode[2] += pow(2,(wartosc-111));

	}
	else if(s[i]>='w' && s[i]<='z')
	{
			keyboard_report.keycode[3] += pow(2,(wartosc-119));

	}
	}

	USBD_HID_SendReport (&USB_OTG_dev, &(keyboard_report.keycode[0]), 4);
}

//end











void TIM3_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = (2000/ilosc_probek);
	TIM_TimeBaseStructure.TIM_Prescaler = 840;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	TIM_Cmd(TIM3, ENABLE);


	// ustawienie trybu pracy priorytetów przerwañ
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitTypeDef NVIC_InitStructure;
	// numer przerwania
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	// priorytet g³ówny
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	// subpriorytet
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	// uruchom dany kana³
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	// zapisz wype³nion¹ strukturê do rejestrów
	NVIC_Init(&NVIC_InitStructure);

	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);




}
int wart;
int stan;
void TIM3_IRQHandler(void)
{
 if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
 {
	 int i;
	 for(i=0;i<ilosc_wejsc;i++)
	 {
	 wejscia[i].najstarszy_pomiar=wejscia[i].pomiar[obecny_indeks];

	 wart = GPIO_ReadInputDataBit(porty[i],piny[i]);
	 (wart == 0) ? (wejscia[i].pomiar[obecny_indeks]= 1) : (wejscia[i].pomiar[obecny_indeks]= 0) ;
	 if(wejscia[i].pomiar[obecny_indeks])
	 {
		 wejscia[i].suma++;
	 }
	if(wejscia[i].najstarszy_pomiar)
	{
		wejscia[i].suma--;
	}

	 }
	if(obecny_indeks==ilosc_probek-1)
		{
			wypisz();
			pusc();
		}
	obecny_indeks++;
	obecny_indeks%=ilosc_probek;




 // miejsce na kod wywo³ywany w momencie wyst¿pienia przerwania
 TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
 }
}
int n;
int main(int argc, char **argv) {
	Demo_USBConfig();
	TIM3_Config();
	gpio_init();
	exti_init();

			//GPIO_SetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);


			GPIO_InitTypeDef  GPIO_InitStructure;
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
			GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
			GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
			GPIO_Init(GPIOB, &GPIO_InitStructure);

	while(1)
	{
		stan=GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_4);
	}

}

#ifdef  USE_FULL_ASSERT

/**
 x * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  *x @param  file: pointer to the source file name
  * x@param  line: assert_param error line source number
  * @rextval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
 x */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
