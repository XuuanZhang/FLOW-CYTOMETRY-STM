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
// ���������ȫ�ֱ������Է��������ʱ�����÷�����ζ���
int detection_num = 0; 
int arry_value[3] = {0};
int arry = 0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

// ����Ƭ����ǰʱ�䣬��λus��
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
//��ȡϵͳʱ�䣬��λus
uint32_t micros(void)
{
  return getCurrentMicros();
}

//�޲����⣬����ֻΪ������
//int Wave_detection_num(int num)  
//{
////  ��ȡ��ǰ��Ƭ��ʱ�䣬us��
//	printf("%d",micros());
//	int adc_value,Wave_detection_num=0;
//	for(int i=0; i<num; i++)
//	{
//		adc_value = Get_Adc(ADC_CHANNEL_5);  //ֵ��ע����ǣ��˴�������Get_Adc_Average()�����򱨴�

//		if(adc_value >= 2482)  //�˴���ֵΪx/3.3*4095��2482ԼΪʵ�ʵ�ѹ2V
//		{
//		  Wave_detection_num++;
//		}
//		delay_us(10);
//		
//		adc_value = 0;
//	}
////  ��ȡ��ǰ��Ƭ��ʱ�䣬us��
//	printf("%d",micros());
//	
//	return Wave_detection_num;
//}


//�޲����⣬delay��ʽ������ֻΪ������
int Wave_detection_num(int num)  
{	
	//  ��ȡ��ǰ��Ƭ��ʱ�䣬ms��
//	printf("%d\r\n",HAL_GetTick());
	int time_on,time_off = 0;
	int adc_value,Wave_detection_num=0;
	//  ��ȡ��ǰ��Ƭ��ʱ�䣬ms��
	time_on = HAL_GetTick();
	while(1)
	{
	  time_off=HAL_GetTick(); 
		if(time_off-time_on <= num)
	   {
		   adc_value = Get_Adc(ADC_CHANNEL_5);
		
		   if(adc_value >= 2482)  //�˴���ֵΪx/3.3*4095��2482ԼΪʵ�ʵ�ѹ2V
		   { 
		     Wave_detection_num++;
		   }
		   delay_us(50);
	   }
		 else{
			 break;
		 }
	 }
////  ��ȡ��ǰ��Ƭ��ʱ�䣬us��
//	printf("%d",micros());
    //  ��ȡ��ǰ��Ƭ��ʱ�䣬ms��
//	printf("%d\r\n",HAL_GetTick());
	return Wave_detection_num;
}


////�޲����⣬delay��ʽ������ֻΪ������  ����λ�ĺ���,���Ǵ����HEX��
//void Waven()  
//{
//	int adc_value;
//  adc_value = Get_Adc(ADC_CHANNEL_5);  //ֵ��ע����ǣ��˴�������Get_Adc_Average()�����򱨴�
//	adc_value = adc_value & 0xfff;
////	printf("%d",sizeof adc_value);
//	printf("%03x",adc_value);     // ������λ  
//}

//�޲����⣬delay��ʽ������ֻΪ������  ����λ�ĺ��������Ǵ����ASCII��
void Waven()  
{
	int adc_value;
	u8 temp[2]={0};
  adc_value = Get_Adc(ADC_CHANNEL_5);  //ֵ��ע����ǣ��˴�������Get_Adc_Average()�����򱨴�
	temp[1] = adc_value & 0xff;
	temp[0] = (adc_value & 0xff00) >> 8; 
//	printf("%x",temp[1]);
//	printf("%x",temp[0]);
  HAL_UART_Transmit(&UART1_Handler, temp, 2, 0xFFFF);
}


// λ�Ʋ���
void shift(int arr[],int value)
{
		int a,b=0;
	  a=arr[1]; 
	  b=arr[2];
	  arr[0]=a;
	  arr[1]=b;
	  arr[2]=value;
}


//�����⣬����ֻΪ������
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



// ��ʱ�������жϻص�����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint32_t TIM3_Cnt1 = 0;
	int adc_value;
	u8 temp[2]={0};
	
	// �ж��Ƕ�ʱ��3�������ж�
	if(htim->Instance == TIM3)
	{
		// ��ʱ��3ÿ�ж�һ�Σ��������Լ�1
		TIM3_Cnt1 ++;

    adc_value = Get_Adc(ADC_CHANNEL_5);  //ֵ��ע����ǣ��˴�������Get_Adc_Average()�����򱨴�
		shift(arry_value,adc_value);
		if(arry_value[1] >= 2482)  //�˴���ֵΪx/3.3*4095��2482ԼΪʵ�ʵ�ѹ2V
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
			

		// ����10000�Σ���ʱ5s
		if(TIM3_Cnt1 >= 20000)
		{
			// ���������
			TIM3_Cnt1 = 0;
      // ��תLED_R�ĵ�ƽ״̬
      // HAL_GPIO_TogglePin(GPIOF, GPIO_PIN_9);
			
			printf("%d",detection_num);
			detection_num = 0;
			// �����ʱ����ʼ�������еĸ����жϱ�־�����ⶨʱ��һ�������ж�
      __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);		
      // ʹ�ܶ�ʱ��3�����жϲ��رն�ʱ��
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
  delay_init(168);               	//��ʼ����ʱ����
	uart_init(256000);             	//��ʼ��USART
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
//					#����Ϊ���Դ���
//					printf("\r\n\r\n");//���뻻��
//					printf("%d\n",(int) USART_RX_BUF[1]);
//					if(USART_RX_BUF[1]==0x00)
//					{
//						a=(int) USART_RX_BUF[2];
//						b=(int) USART_RX_BUF[3];
//						printf("\r\n\r\n");//���뻻��
//						crist = a*100 + b;
//						HAL_Delay(200);
//						printf("%d\n",crist);
//						printf("\r\n\r\n");//���뻻��
//						printf("obsidian\n");
//				}
					
//					#����Ϊȫͨ����ͬ��ֵ
//					printf("\r\n\r\n");
					crist = USART_RX_BUF[1] << 8;
					crist += USART_RX_BUF[2];
					printf("%d\n",crist);	
					MCP4728_Write_AllChannels_Same(&hi2c1, crist);					
				}
				
			if(USART_RX_BUF[0]== 'Q')
				{

//					#����Ϊ��ͨ��������ֵ
//					printf("\r\n\r\n");
					can=(int) USART_RX_BUF[1];
					crist = USART_RX_BUF[2] << 8;
					crist += USART_RX_BUF[3];
					printf("%d\n",crist);				
					MCP4728_Write_SingleChannel(&hi2c1, can, crist);
						
				}
				
			if(USART_RX_BUF[0]== 'R')
			{
				//�����б�ͨ���ķ�ʽΪһ��ͨ����Ӧһ��GPIO���ţ���������������Ӧ���ż��ɣ��˴�0x01��ӦPF10,��Ƶ��ŷ�
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
				
				
				if(USART_RX_BUF[1]== 0x02) //02��03��Ӧ��ͨ��ŷ�
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
//				// �˲���Ϊ���˲�����
////				for(int i=0;i<3;i++)
////		    {
////		      adcx[i]=Get_Adc_Average(ADC_CHANNEL_5,1);//��ȡͨ��5��ת��ֵ��20��ȡƽ��
//////		      LCD_ShowxNum(134,130,adcx[i],4,16,0);    //��ʾADCC�������ԭʼֵ
////		      temp=(float)adcx[i]*(3.3/4096);          //��ȡ�����Ĵ�С����ʵ�ʵ�ѹֵ������3.1111
////		      arr1[i]=temp;                            //��ֵ�������ָ�adcx��������ΪadcxΪu16����
//////		      LCD_ShowxNum(134,150,arr1[i],1,16,0);    //��ʾ��ѹֵ���������֣�3.1111�Ļ������������ʾ3
//////			    temp-=arr1[i];                           //���Ѿ���ʾ����������ȥ��������С�����֣�����3.1111-3=0.1111
//////		      temp*=1000;                           //С�����ֳ���1000�����磺0.1111��ת��Ϊ111.1���൱�ڱ�����λС����
//////		      LCD_ShowxNum(150,150,temp,3,16,0X80); //��ʾС�����֣�ǰ��ת��Ϊ��������ʾ����������ʾ�ľ���111.
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
        
				// �˲���Ϊ���delay_us����
				printf("%d", Wave_detection_num(10000));
				// �����˲����ļ��
//				Wavepeak_detection_num(10000);
//				printf("Get Daze!");
			
//				printf("Get Daze!Currect number is %d\r\n", Wavepeak_detection_num(10000));
	    } 
			
			if(USART_RX_BUF[0]==0x48)
		  {	
				
		  // �����ʱ����ʼ�������еĸ����жϱ�־�����ⶨʱ��һ�������ж�
      __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);		
      // ʹ�ܶ�ʱ��3�����жϲ�������ʱ��
      HAL_TIM_Base_Start_IT(&htim3);
	    } 
			
			if(USART_RX_BUF[0]==0x49)
		  {	
				
		  // �����ʱ����ʼ�������еĸ����жϱ�־�����ⶨʱ��һ�������ж�
      __HAL_TIM_CLEAR_IT(&htim3, TIM_IT_UPDATE);		
      // ʹ�ܶ�ʱ��3�����жϲ��رն�ʱ��
      HAL_TIM_Base_Stop_IT(&htim3);
	    } 
			
			while(__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_TC)!=SET);		//�ȴ����ͽ���
//			printf("\r\n\r\n");//���뻻��
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
