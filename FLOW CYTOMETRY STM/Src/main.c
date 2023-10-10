/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"
#include "mcp4728.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// 将变量变成全局变量可以方便后续定时器不用反复多次定义
int detection_num = 0; 
int arry_value[3] = {0};
int arry = 0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// 捕获单片机当前时间，单位us级
__STATIC_INLINE uint32_t GXT_SYSTICK_IsActiveCounterFlag(void)
{
  return ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) == (SysTick_CTRL_COUNTFLAG_Msk));
}
static uint32_t getCurrentMicros(void)
{
  /* Ensure COUNTFLAG is reset by reading SysTick control and status register */
  GXT_SYSTICK_IsActiveCounterFlag();
  uint32_t m = HAL_GetTick();
  const uint32_t tms = SysTick->LOAD + 1;
  __IO uint32_t u = tms - SysTick->VAL;
  if (GXT_SYSTICK_IsActiveCounterFlag()) {
    m = HAL_GetTick();
    u = tms - SysTick->VAL;
  }
  return (m * 1000 + (u * 1000) / tms);
}
//获取系统时间，单位us
uint32_t micros(void)
{
  return getCurrentMicros();
}

//无波峰检测，仅仅只为测试用
//int Wave_detection_num(int num)  
//{
////  获取当前单片机时间，us级
//	printf("%d",micros());
//	int adc_value,Wave_detection_num=0;
//	for(int i=0; i<num; i++)
//	{
//		adc_value = Get_Adc(ADC_CHANNEL_5);  //值得注意的是，此处若采用Get_Adc_Average()函数则报错

//		if(adc_value >= 2482)  //此处数值为x/3.3*4095，2482约为实际电压2V
//		{
//		  Wave_detection_num++;
//		}
//		delay_us(10);
//		
//		adc_value = 0;
//	}
////  获取当前单片机时间，us级
//	printf("%d",micros());
//	
//	return Wave_detection_num;
//}


//无波峰检测，delay方式，仅仅只为测试用
int Wave_detection_num(int num)  
{	
	//  获取当前单片机时间，ms级
//	printf("%d\r\n",HAL_GetTick());
	int time_on,time_off = 0;
	int adc_value,Wave_detection_num=0;
	//  获取当前单片机时间，ms级
	time_on = HAL_GetTick();
	while(1)
	{
	  time_off=HAL_GetTick(); 
		if(time_off-time_on <= num)
	   {
		   adc_value = Get_Adc(ADC_CHANNEL_5);
		
		   if(adc_value >= 2482)  //此处数值为x/3.3*4095，2482约为实际电压2V
		   { 
		     Wave_detection_num++;
		   }
		   delay_us(50);
	   }
		 else{
			 break;
		 }
	 }
////  获取当前单片机时间，us级
//	printf("%d",micros());
    //  获取当前单片机时间，ms级
//	printf("%d\r\n",HAL_GetTick());
	return Wave_detection_num;
}


////无波峰检测，delay方式，仅仅只为测试用  （三位的函数,但是传输的HEX）
//void Waven()  
//{
//	int adc_value;
//  adc_value = Get_Adc(ADC_CHANNEL_5);  //值得注意的是，此处若采用Get_Adc_Average()函数则报错
//	adc_value = adc_value & 0xfff;
////	printf("%d",sizeof adc_value);
//	printf("%03x",adc_value);     // 补足三位  
//}

//无波峰检测，delay方式，仅仅只为测试用  （两位的函数，但是传输的ASCII）
void Waven()  
{
	int adc_value;
	u8 temp[2]={0};
  adc_value = Get_Adc(ADC_CHANNEL_5);  //值得注意的是，此处若采用Get_Adc_Average()函数则报错
	temp[1] = adc_value & 0xff;
	temp[0] = (adc_value & 0xff00) >> 8; 
//	printf("%x",temp[1]);
//	printf("%x",temp[0]);
  HAL_UART_Transmit(&UART1_Handler, temp, 2, 0xFFFF);
}


// 位移操作
void shift(int arr[],int value)
{
		int a,b=0;
	  a=arr[1]; 
	  b=arr[2];
	  arr[0]=a;
	  arr[1]=b;
	  arr[2]=value;
}


//波峰检测，仅仅只为测试用
int Wavepeak_detection_num(int num)
{
	int adc_value[3]={0};
	int detection_num=0;

	for(int m=0;m<=num;m++)
	{ 
		int b = Get_Adc(ADC_CHANNEL_5);
		shift(adc_value,b);
		

		if( adc_value[1] >= 3722)
		{
			if(adc_value[1] >= adc_value[0] && adc_value[1] >= adc_value[2])
			{
				detection_num++;
			}
		}
		delay_us(50);
	}	
	
	return detection_num;
}



// 定时器更新中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint32_t TIM3_Cnt1 = 0;
	int adc_value;
	u8 temp[2]={0};
	
	// 判断是定时器3发生的中断
	if(htim->Instance == TIM3)
	{
		// 定时器3每中断一次，计数器自加1
		TIM3_Cnt1 ++;

    adc_value = Get_Adc(ADC_CHANNEL_5);  //值得注意的是，此处若采用Get_Adc_Average()函数则报错
		shift(arry_value,adc_value);
		if(arry_value[1] >= 2482)  //此处数值为x/3.3*4095，2482约为实际电压2V
		 { 
			 if(arry_value[1] >= arry_value[0] && arry_value[1] >= arry_value[2])
			 {
		     detection_num++;
			 }
		 }
		
		temp[1] = adc_value & 0xff;
	  temp[0] = (adc_value & 0xff00) >> 8; 
//	  printf("%x",temp[1]);
//	  printf("%x",temp[0]);
    HAL_UART_Transmit(&UART1_Handler, temp, 2, 0xFFFF);
			

		// 计数10000次，定时5s
		if(TIM3_Cnt1 >= 20000)
		{
			// 清除计数器
			TIM3_Cnt1 = 0;
      // 翻转LED_R的电平状态
      // HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_9);
			
			printf("%d",detection_num);
			detection_num = 0;
			// 清除定时器初始化过程中的更新中断标志，避免定时器一启动就中断
      __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);		
      // 使能定时器3更新中断并关闭定时器
      HAL_TIM_Base_Stop_IT(&htim3);
		}
	}
}

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */
 
/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	unsigned int crist;
	int can;
	int t=0;
	
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  delay_init(168);               	//初始化延时函数
	uart_init(256000);             	//初始化USART
	MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_DAC_Init();
  MX_I2C1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
   while (1)
  {
    /* USER CODE END WHILE */
		
    if(USART_RX_STA&0x8000)
		{					   
			
			if(USART_RX_BUF[0]=='P')
				{
//					#这里为测试代码
//					printf("\r\n\r\n");//插入换行
//					printf("%d\n",(int) USART_RX_BUF[1]);
//					if(USART_RX_BUF[1]==0x00)
//					{
//						a=(int) USART_RX_BUF[2];
//						b=(int) USART_RX_BUF[3];
//						printf("\r\n\r\n");//插入换行
//						crist = a*100 + b;
//						HAL_Delay(200);
//						printf("%d\n",crist);
//						printf("\r\n\r\n");//插入换行
//						printf("obsidian\n");
//				}
					
//					#这里为全通道相同数值
//					printf("\r\n\r\n");
					crist = USART_RX_BUF[1] << 8;
					crist += USART_RX_BUF[2];
					printf("%d\n",crist);	
					MCP4728_Write_AllChannels_Same(&hi2c1, crist);					
				}
				
			if(USART_RX_BUF[0]== 'Q')
				{

//					#这里为单通道赋予数值
//					printf("\r\n\r\n");
					can=(int) USART_RX_BUF[1];
					crist = USART_RX_BUF[2] << 8;
					crist += USART_RX_BUF[3];
					printf("%d\n",crist);				
					MCP4728_Write_SingleChannel(&hi2c1, can, crist);
						
				}
				
			if(USART_RX_BUF[0]== 'R')
			{
				//这里判别通道的方式为一个通道对应一个GPIO引脚，所以拉高拉低相应引脚即可，此处0x01对应PF10,高频电磁阀
				if(USART_RX_BUF[1]== 0x01)
				{
					if(USART_RX_BUF[2]== 0x00)
					{
						HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_RESET);
						HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9,GPIO_PIN_RESET);
						printf("off");
					}
					if(USART_RX_BUF[2]== 0x01)
					{
						HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_SET);
						HAL_GPIO_WritePin(GPIOF,GPIO_PIN_9,GPIO_PIN_SET);
						printf("on");
					}
					if(USART_RX_BUF[2]== 0x02)
					{
						while(t<=4)
						{
					   HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_SET);
					   HAL_Delay(100);
					   HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_RESET);
						 HAL_Delay(100);
				     t+=1;
						 printf("over");
						}
						t=0;
					}
				}
				
				
				if(USART_RX_BUF[1]== 0x02) //02，03对应普通电磁阀
				{
					if(USART_RX_BUF[2]== 0x00)
					{
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_RESET);
						printf("off");
					}
					if(USART_RX_BUF[2]== 0x01)
					{
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_1,GPIO_PIN_SET);
						printf("on");
					}
				}
				
				
				if(USART_RX_BUF[1]== 0x03)
				{
					if(USART_RX_BUF[2]== 0x00)
					{
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_RESET);
						printf("off");
					}
					if(USART_RX_BUF[2]== 0x01)
					{
						HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_SET);
						printf("on");
					}
				}
			}
					
//      if(USART_RX_BUF[0]==0x53)
//		  {			
//				Waven();
//				// 此部分为加了波峰检测
////				for(int i=0;i<3;i++)
////		    {
////		      adcx[i]=Get_Adc_Average(ADC_CHANNEL_5,1);//获取通道5的转换值，20次取平均
//////		      LCD_ShowxNum(134,130,adcx[i],4,16,0);    //显示ADCC采样后的原始值
////		      temp=(float)adcx[i]*(3.3/4096);          //获取计算后的带小数的实际电压值，比如3.1111
////		      arr1[i]=temp;                            //赋值整数部分给adcx变量，因为adcx为u16整形
//////		      LCD_ShowxNum(134,150,arr1[i],1,16,0);    //显示电压值的整数部分，3.1111的话，这里就是显示3
//////			    temp-=arr1[i];                           //把已经显示的整数部分去掉，留下小数部分，比如3.1111-3=0.1111
//////		      temp*=1000;                           //小数部分乘以1000，例如：0.1111就转换为111.1，相当于保留三位小数。
//////		      LCD_ShowxNum(150,150,temp,3,16,0X80); //显示小数部分（前面转换为了整形显示），这里显示的就是111.
//////		      LED0=!LED0;
////		      delay_us(100);	
////		    }
////		    if(2<=arr1[0] && 2<=arr1[1] && 2<=arr1[2])
////		    {
//////			    if(adcx[0]<=adcx[1])
//////			    {
//////				    if(adcx[2]<=adcx[1])
//////				    {
////					      count++;
////					      printf("Get Daze!Current num is %d\r\n",count);
//////				    }
//////		      }
////		    }
//////		    delay_us(100);
//      }
//       
			if(USART_RX_BUF[0]==0x54)
		  {	
        
				// 此部分为检测delay_us功能
				printf("%d", Wave_detection_num(10000));
				// 加入了波峰后的检测
//				Wavepeak_detection_num(10000);
//				printf("Get Daze!");
			
//				printf("Get Daze!Currect number is %d\r\n", Wavepeak_detection_num(10000));
	    } 
			
			if(USART_RX_BUF[0]==0x48)
		  {	
				
		  // 清除定时器初始化过程中的更新中断标志，避免定时器一启动就中断
      __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);		
      // 使能定时器3更新中断并启动定时器
      HAL_TIM_Base_Start_IT(&htim3);
	    } 
			
			if(USART_RX_BUF[0]==0x49)
		  {	
				
		  // 清除定时器初始化过程中的更新中断标志，避免定时器一启动就中断
      __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);		
      // 使能定时器3更新中断并关闭定时器
      HAL_TIM_Base_Stop_IT(&htim3);
	    } 
			
			while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);		//等待发送结束
//			printf("\r\n\r\n");//插入换行
			USART_RX_STA=0;
			}
	  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
