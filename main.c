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
#include "stm32f4xx_it.h"
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
#include "string.h"
int n=0;
typedef struct {
	uint8_t keycode[4];
} keyboard_report_t;

/** @addtogroup STM32F4-Discovery_Demo
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define TESTRESULT_ADDRESS         0x080FFFFC
#define ALLTEST_PASS               0x00000000
#define ALLTEST_FAIL               0x55555555

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef USB_OTG_HS_INTERNAL_DMA_ENABLED
  #if defined ( __ICCARM__ ) /*!< IAR Compiler */
    #pragma data_alignment = 4   
  #endif
#endif /* USB_OTG_HS_INTERNAL_DMA_ENABLED */
__ALIGN_BEGIN USB_OTG_CORE_HANDLE  USB_OTG_dev __ALIGN_END;
  
uint16_t PrescalerValue = 0;

__IO uint32_t TimingDelay;
__IO uint8_t DemoEnterCondition = 0x00;
__IO uint8_t UserButtonPressed = 0x00;
LIS302DL_InitTypeDef  LIS302DL_InitStruct;
LIS302DL_FilterConfigTypeDef LIS302DL_FilterStruct;  
__IO int8_t X_Offset, Y_Offset, Z_Offset  = 0x00;
uint8_t Buffer[6];

/* Private function prototypes -----------------------------------------------*/
static uint32_t Demo_USBConfig(void);
static void TIM3_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

#define ADC_1_ADDRESS_BASE 0x40012000
// ADC_DR = ADC regular Data Register
#define ADC_DR_ADDRESS_OFFSET 0x4C
__IO uint16_t ADC1ConvertedValue[6] = {0,0,0,0,0,0};
int co;
/*
 int co, co2;
 volatile uint16_t ADCConvertedValue;

 void MY_ADC_init(void)
 {
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE); // wejscie ADC

  //inicjalizacja wejœcia ADC
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  // niezale¿ny tryb pracy przetworników
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
  // zegar g³ówny podzielony przez 2
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
  // opcja istotna tylko dla tryby multi ADC
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
  // czas przerwy pomiêdzy kolejnymi konwersjami
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
  ADC_CommonInit(&ADC_CommonInitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //ADC

  ADC_InitTypeDef ADC_InitStructure;
  //ustawienie rozdzielczoœci przetwornika na maksymaln¹ (12 bitów)
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  //wy³¹czenie trybu skanowania (odczytywaæ bêdziemy jedno wejœcie ADC
  //w trybie skanowania automatycznie wykonywana jest konwersja na wielu
 //wejœciach/kana³ach)
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;
  //w³¹czenie ci¹g³ego trybu pracy
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  //wy³¹czenie zewnêtrznego wyzwalania
  //konwersja mo¿e byæ wyzwalana timerem, stanem wejœcia itd. (szczegó³y w
 //dokumentacji)
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  //wartoœæ binarna wyniku bêdzie podawana z wyrównaniem do prawej
  //funkcja do odczytu stanu przetwornika ADC zwraca wartoœæ 16-bitow¹
  //dla przyk³adu, wartoœæ 0xFF wyrównana w prawo to 0x00FF, w lewo 0x0FF0
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  //liczba konwersji równa 1, bo 1 kana³
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  // zapisz wype³nion¹ strukturê do rejestrów przetwornika numer 1
  ADC_Init(ADC1, &ADC_InitStructure);

  // konfiguracja czasu próbkowania sygna³u
  ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_84Cycles);

  // w³¹czenie wyzwalania DMA po ka¿dym zakoñczeniu konwersji
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
  // w³¹czenie DMA dla ADC
  ADC_DMACmd(ADC1, ENABLE);

  // uruchomienie modu³y ADC
  ADC_Cmd(ADC1, ENABLE);
 }
 void MY_DMA_initP2M(void)
 {
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);

  DMA_InitTypeDef strukturaDoInicjalizacjiDMA;

  // wybór kana³u DMA Strona 3 z 9

  strukturaDoInicjalizacjiDMA.DMA_Channel = DMA_Channel_0;
  // ustalenie rodzaju transferu (memory2memory / peripheral2memory / memory2peripheral)
  strukturaDoInicjalizacjiDMA.DMA_DIR = DMA_DIR_PeripheralToMemory;
  // tryb pracy - pojedynczy transfer b¹dŸ powtarzany
  strukturaDoInicjalizacjiDMA.DMA_Mode = DMA_Mode_Circular;
  // ustalenie priorytetu danego kana³u DMA
  strukturaDoInicjalizacjiDMA.DMA_Priority = DMA_Priority_Medium;
  // liczba danych do przes³ania
  strukturaDoInicjalizacjiDMA.DMA_BufferSize = (uint32_t)1;
  // adres Ÿród³owy
  strukturaDoInicjalizacjiDMA.DMA_PeripheralBaseAddr =
 (uint32_t)(ADC_1_ADDRESS_BASE+ADC_DR_ADDRESS_OFFSET);
  // adres docelowy
  strukturaDoInicjalizacjiDMA.DMA_Memory0BaseAddr = (uint32_t)&ADCConvertedValue;
  // okreslenie, czy adresy maj¹ byæ inkrementowane po ka¿dej przes³anej paczce danych
  strukturaDoInicjalizacjiDMA.DMA_MemoryInc = DMA_MemoryInc_Disable;
  strukturaDoInicjalizacjiDMA.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  // ustalenie rozmiaru przesy³anych danych
  strukturaDoInicjalizacjiDMA.DMA_PeripheralDataSize =
 DMA_PeripheralDataSize_HalfWord;
  strukturaDoInicjalizacjiDMA.DMA_MemoryDataSize =
 DMA_MemoryDataSize_HalfWord;
  // ustalenie trybu pracy - jednorazwe przes³anie danych
  strukturaDoInicjalizacjiDMA.DMA_PeripheralBurst =
 DMA_PeripheralBurst_Single;
  strukturaDoInicjalizacjiDMA.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  // wy³¹czenie kolejki FIFO (nie u¿ywana w tym przykadzie)
  strukturaDoInicjalizacjiDMA.DMA_FIFOMode = DMA_FIFOMode_Disable;
  // wype³nianie wszystkich pól struktury jest niezbêdne w celu poprawnego dzia³ania, wpisanie jednej z dozwolonych wartosci
  strukturaDoInicjalizacjiDMA.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;

  // zapisanie wype³nionej struktury do rejestrów wybranego po³¹czenia DMA
  DMA_Init(DMA2_Stream4, &strukturaDoInicjalizacjiDMA);

  // uruchomienie odpowiedniego po³¹czenia DMA
  DMA_Cmd(DMA2_Stream4, ENABLE);
 }
*/
void ADC1_DMA_Config(void)
{
 ADC_InitTypeDef       ADC_InitStructure;
 ADC_CommonInitTypeDef ADC_CommonInitStructure;
 DMA_InitTypeDef       DMA_InitStructure;
 GPIO_InitTypeDef      GPIO_InitStructure;


 /* Enable ADC1, DMA2 and GPIO clocks ****************************************/
 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2 | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOB, ENABLE);
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);


 DMA_DeInit(DMA2_Stream0);
 /* DMA2 Stream0 channel0 configuration **************************************/
 DMA_InitStructure.DMA_Channel = DMA_Channel_0;
 DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC1ConvertedValue;
 DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(ADC_1_ADDRESS_BASE+ADC_DR_ADDRESS_OFFSET);
 DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
 DMA_InitStructure.DMA_BufferSize = 6;
 DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
 DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
 DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // u32
 DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
 DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
 DMA_InitStructure.DMA_Priority = DMA_Priority_High;
 DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
 DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
 DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
 DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
 DMA_Init(DMA2_Stream0, &DMA_InitStructure);
 DMA_Cmd(DMA2_Stream0, ENABLE);

/* Configure ADC1 Channel 11-12-14-15 pin as analog input ******************************/
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
 GPIO_Init(GPIOC, &GPIO_InitStructure);

 /* Configure ADC1 Channel 08-09 pin as analog input ******************************/
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 ;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
 GPIO_Init(GPIOB, &GPIO_InitStructure);



 /* ADC Common Init **********************************************************/

 ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
 ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
 ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
 ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
 ADC_CommonInit(&ADC_CommonInitStructure);


 /* ADC1 Init ****************************************************************/
 ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
 ADC_InitStructure.ADC_ScanConvMode = ENABLE;
 ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
 ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
 ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
 ADC_InitStructure.ADC_NbrOfConversion = 6;
 ADC_InitStructure.ADC_ExternalTrigConv = 0x00;
 ADC_Init(ADC1, &ADC_InitStructure);

 /* Enable ADC1 DMA */
 ADC_DMACmd(ADC1, ENABLE);

 /* ADC1 regular channel12 configuration *************************************/
 ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_15Cycles);
 ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 2, ADC_SampleTime_15Cycles);
 ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 3, ADC_SampleTime_15Cycles);
 ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 4, ADC_SampleTime_15Cycles);
 ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 5, ADC_SampleTime_15Cycles);
 ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 6, ADC_SampleTime_15Cycles);


 /* Enable DMA request after last transfer (Single-ADC mode) */
 ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);

 /* Enable ADC1 */
 ADC_Cmd(ADC1, ENABLE);
}
unsigned int counter;
int main(int argc, char **argv) {
	Demo_USBConfig();
	TIM3_Config();
	ADC1_DMA_Config();
	ADC_SoftwareStartConv(ADC1);

	while(1)
	{
	 co = ADC1ConvertedValue[0];
	 counter=TIM3->CNT;




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

static void TIM3_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = 2500/20;
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
int adc_pc1=0,adc_pc2=0,adc_pc4=0,adc_pc5=0,adc_pb0=0,adc_pb1=0;
void wypisz()
{
	char s[12];
	 if(adc_pc1<=500)
	 {
		strncat(s,"->",2);

	 }
	 if(adc_pc2<=500)
	 	 {
	 		 strncat(s,"<-",2);
	 	 }
	 if(adc_pc4<=500)
	 	 {
		 strncat(s,"/|",2);
	 	 }
	 	 if(adc_pc5<=500)
	 	 {
	 		strncat(s,"|/",2);

	 	 }

	 if(adc_pb0<=500)
	 	 {
		 strncat(s,"||",2);

	 	 }
	 if(adc_pb1<=500)
	 	 {
		 strncat(s,"a",2);
	 	 }
	 napisz(s);
}


void TIM3_IRQHandler(void)
 {
 if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
 {
	 n++;
	 adc_pc1+=ADC1ConvertedValue[0];
	 adc_pc2+=ADC1ConvertedValue[1];
	 adc_pc4+=ADC1ConvertedValue[2];
	 adc_pc5+=ADC1ConvertedValue[3];
	 adc_pb0+=ADC1ConvertedValue[4];
	 adc_pb1+=ADC1ConvertedValue[5];
	 if(n==20)
	 {
		 adc_pc1/=20;
		 adc_pc2/=20;
		 adc_pc4/=20;
		 adc_pc5/=20;
		 adc_pb0/=20;
		 adc_pb1/=20;
		 wypisz();
		 n=0;
	 }
 // miejsce na kod wywo³ywany w momencie wyst¿pienia przerwania
 TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
 }
 }




#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
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
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
