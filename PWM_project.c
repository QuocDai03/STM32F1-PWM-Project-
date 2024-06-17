#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

unsigned char maled_cc[10] = {0xAF, 0xA0, 0x6E, 0xEA, 0xE1, 0xCB, 0xCF, 0xA2, 0xEF, 0xEB};
unsigned char led_position[5] = { 0x08,0x10,0x20,0x40,0x80};  // Position of LED

int count=0;
uint8_t D;// Duty cycle

// Variables for USART
char data[3]; // array receives data sent from PC
int a=0; 

//----------Functions for module using LED 7-segment 5 digits------
void clock_74HC594 ()
{
	GPIOB->ODR |= (1<<3);
	GPIOB->ODR &= ~(1<<3);
}
void LATH()
{
	
	GPIOE->ODR &= ~(1<<0);
	GPIOE->ODR |= (1<<0);
	GPIOE->ODR &= ~(1<<0);
}
void data_74HC594(uint8_t n)
{
	for(char i=0;i<=7;i++)
	{
		if ((n & (1<<(7-i))) ==0) 
		{	
			GPIOB->ODR &= ~(1<<5);
		}
		else GPIOB->ODR |= (1<<5); 
		clock_74HC594();
	}
}

// Functin for Display on module LED 7-segment 5 digits
void Xuat_LED(int c)
{
	uint16_t tram= ((c%10000)%1000)/100;
	uint16_t chuc=(((c%10000)%1000)%100)/10;
	uint16_t dv=(((c%10000)%1000)%100)%10;
	
	data_74HC594(led_position[0]);
	data_74HC594(0x00);
	LATH();
	HAL_Delay(3);
			
			
	data_74HC594(led_position[1]);
	data_74HC594(0x00);
	LATH();
	HAL_Delay(3);
			
	if (count<100)
	{
		data_74HC594(led_position[2]);
		data_74HC594(0x00);
		LATH();
		HAL_Delay(3);
	}
	else if (count==100)
	{
		data_74HC594(led_position[2]);
		data_74HC594(maled_cc[1]);
		LATH();
		HAL_Delay(3);
	}
	if (count>=10)
	{
		data_74HC594(led_position[3]);
		data_74HC594(maled_cc[chuc]);
		LATH();
		HAL_Delay(3);
	}
	else if (count<10)
	{
		data_74HC594(led_position[3]);
		data_74HC594(0x00);
		LATH();
		HAL_Delay(3);
	}
	
	data_74HC594(led_position[4]);
	data_74HC594(maled_cc[dv]);
	LATH();
	HAL_Delay(3);
	
}

//-----------USART function-----------------
void send_char(char data)
{
	while((USART1->SR&(1<<6))==0);
	USART1->DR = data;
}
void send_string(char *str)
{
	while (*str) send_char(*str++);
}

//----------Function for modifying duty cycle-------
void TIM2_PWM_SetDuty (uint16_t duty)
{
	TIM2->CCR3=duty;
}




int main(void)
{
	// --------Configuration for using LED 7 segments 5 digits---------- 
	// Pin PB3, PB5, PE0, PE1, PE2, PE3: Output push pull
	RCC->APB2ENR |= (1<<3) | (1<<6);
	GPIOB->CRL = 0x00;//Clear
	GPIOE->CRL =0x00;
	GPIOB-> CRL |= (1<<(4*3)) | (1<<(4*5));
	GPIOE->CRL |= (1<<(4*1)) | (1<<(4*0)) | (1<<(4*2)) | (1<<(4*3)); // PE3 output push-pull: cai loa
	
	// Pin PB4: Input pull up
	GPIOB->CRL  |=(8<<(4*4)); //Input
	GPIOB->ODR |= (1<<4);

	// Activate IC74HC594
	GPIOE->ODR &= ~(1<<2);// PE2 chan kich hoat IC74HC594 (ghi muc 0 de kich hoat)
	
	// Configuration of for adjusting the level of brightness of LED by using PWM mode
	RCC->APB2ENR |= (1<<0); //clock AFIO
	AFIO->MAPR |= (3<<8); //full remap timer 2 (Doi chan PA2 (mac dinh) thanh chan PB10 (du bi)
	RCC->APB2ENR |= (1<<3); //clock for GPIOB
	GPIOB->CRH |= (9<<(4*2)); //output AF PB10 -CH3
	
	// ----------Configuration of for using PWM mode with Timer 2----------
	RCC->APB1ENR |= (1<<0); // clock for timer 2
	TIM2->PSC =7;
	TIM2->ARR =100; // f=10kHz (STM32F1 =8MHz)
	TIM2->CCR3 = 0; //Duty cylcle: D=0%
	TIM2->CCMR2 |= (7<<4); //CCM2 dung cho CH3, CH4; PWM mode 2: Duty cycle là thoi gian muc THAP
	TIM2->CCER |= (1<<8); // Enable CH3
	TIM2->CR1 |= (1<<0); //enable timer 2

	//--------Configuration of USART1----------
	// TX PA9; RX:PA10
	RCC->APB2ENR |= (1<<0); //clock AFIO
	RCC -> APB2ENR |= (1<<14) ; // Clock for USART1
	GPIOA->CRH =0;//Clear
	GPIOA->CRH |= (9<<4) | (4<<8); //PA9(TX)-> output AF PP; PA10(RX)-> input floating mode
	USART1-> BRR = (52 << 4) | (1<<0) ; //baudrate 9600
	USART1-> CR1 |= (1<<2) | (1<<3) ;// Enable TX, RX
	USART1-> CR1 |= (1<<13) ;// Enable USART1
	USART1 -> CR1 |= (1<<5) ; // Enable RX interrupt
	NVIC -> ISER[1] |= (1<<5) ; // Enable USART1 interrupt	

	while (1)
  	{
		D=count;
		Xuat_LED(count);
		TIM2_PWM_SetDuty(D);
	}
}

// ----------Vector Interrupt of USART1----------
void USART1_IRQHandler (void)  
{
	//clear USARTx interrupt flag
	// flag is cleared by read or clear register USARTx_DR
	data[a++] = USART1->DR;
	if (USART1->DR == ';')
	{
		for(char i=0;i<2;i++)
		{
			data[i]='\0';
		}
		a=0;
	}
	if (a==3)
	{
		count= atoi(data);
		if (count >=100) count =100;
		a=0;
	}
}
