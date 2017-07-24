/* LL drivers common to all LL examples */
#include "stm32l4xx_ll_bus.h"
#include "stm32l4xx_ll_rcc.h"
#include "stm32l4xx_ll_system.h"
#include "stm32l4xx_ll_utils.h"
#include "stm32l4xx_ll_pwr.h"
#include "stm32l4xx_ll_exti.h"
#include "stm32l4xx_ll_gpio.h"
/* LL drivers specific to LL examples IPs */
#include "stm32l4xx_ll_adc.h"
#include "stm32l4xx_ll_comp.h"
#include "stm32l4xx_ll_cortex.h"
#include "stm32l4xx_ll_crc.h"
#include "stm32l4xx_ll_dac.h"
#include "stm32l4xx_ll_dma.h"
#include "stm32l4xx_ll_i2c.h"
#include "stm32l4xx_ll_iwdg.h"
#include "stm32l4xx_ll_lptim.h"
#include "stm32l4xx_ll_lpuart.h"
#include "stm32l4xx_ll_opamp.h"
#include "stm32l4xx_ll_rng.h"
#include "stm32l4xx_ll_rtc.h"
#include "stm32l4xx_ll_spi.h"
#include "stm32l4xx_ll_swpmi.h"
#include "stm32l4xx_ll_tim.h"
#include "stm32l4xx_ll_usart.h"
#include "stm32l4xx_ll_wwdg.h"

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

#include "usb_host.h"

#include "usbh_msc.h"
#include "ff.h"

#include "LCD.h"
#include "BMP.h"

#include <stdio.h>
#include <stdlib.h>


extern USBH_HandleTypeDef hUsbHostFS;
extern ApplicationTypeDef Appli_state;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void StartDefaultTask(void const * argument);

int main(void)
{
  SystemClock_Config();

  MX_GPIO_Init();

  xTaskCreate((TaskFunction_t)StartDefaultTask, "defaultTask", 1024, NULL, 1, NULL);

  vTaskStartScheduler();

  while (1)
  {


  }


}

void SystemClock_Config(void)
{

  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4)
  {

  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);

  LL_RCC_MSI_Enable();

  while(LL_RCC_MSI_IsReady() != 1)
  {
    
  }
  LL_RCC_MSI_EnableRangeSelection();

  LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_6);

  LL_RCC_MSI_SetCalibTrimming(0);

  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2);

  LL_RCC_PLL_EnableDomain_SYS();

  LL_RCC_PLL_Enable();

  while(LL_RCC_PLL_IsReady() != 1)
  {
    
  }
  LL_RCC_PLLSAI1_ConfigDomain_48M(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 24, LL_RCC_PLLSAI1Q_DIV_2);

  LL_RCC_PLLSAI1_EnableDomain_48M();

  LL_RCC_PLLSAI1_Enable();

  while(LL_RCC_PLLSAI1_IsReady() != 1)
  {
    
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);

  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);

  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  
  }
  LL_Init1msTick(80000000);

  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);

  LL_SetSystemCoreClock(80000000);

  LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_PLLSAI1);

  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),15, 0));
}

static void MX_GPIO_Init(void)
{

  LL_GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

  /**/
  LL_GPIO_SetOutputPin(GPIOC, LL_GPIO_PIN_9);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
  /**/
  LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_0);

  /**/
  GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);	

}

char* pDirectoryFiles[MAX_BMP_FILES];
FATFS USB_FatFs;
uint8_t str[20];

uint8_t bmpcounter = 0;
uint8_t filesnumbers = 0;

void StartDefaultTask(void const * argument)
{
  MX_USB_HOST_Init();
	ILI9341_Init();
	ILI9341_Clear(0xF800);
	
	while(Appli_state != APPLICATION_READY){
		ILI9341_Clear(0xFFFF);
		ILI9341_Clear(0x0000);
	}

      for (uint8_t counter = 0; counter < MAX_BMP_FILES; counter++)
      {
        pDirectoryFiles[counter] = pvPortMalloc(11); 
      }
			f_mount(&USB_FatFs, (TCHAR const*)"/", 0);
	
  filesnumbers = Storage_GetDirectoryBitmapFiles ("0:", pDirectoryFiles);  
		
	for(;;){
		
 sprintf((char*)str, "%-11.11s", pDirectoryFiles[bmpcounter -1]);
      
        Storage_OpenReadFile(0, 0, (const char*)str);
      

      bmpcounter++;
      if(bmpcounter > filesnumbers)
      {
        bmpcounter = 1;
      }
			vTaskDelay(500);
	}
  /* USER CODE END 5 */ 
}
