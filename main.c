#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"

#include "FreeRTOS.h"
#include "task.h"

#include "matrix_display.h"

#define SPIx_RCC				RCC_APB2Periph_SPI1
#define SPIx						SPI1
#define SPI_GPIO_RCC		RCC_APB2Periph_GPIOA
#define SPI_GPIO				GPIOA
#define SPI_PIN_MOSI		GPIO_Pin_7
#define SPI_PIN_MISO		GPIO_Pin_6
#define SPI_PIN_SCK			GPIO_Pin_5
#define SPI_PIN_SS			GPIO_Pin_4

extern uint8_t matrix[8][8];

void SPIx_Init(void);
uint8_t SPIx_Transfer(uint8_t data);
void SPIx_EnableSlave(void);
void SPIx_DisableSlave(void);

void _delay_us(uint16_t _us){
	uint16_t i, l;
	for(i = 0;i < _us;i++){
		for(l = 0;l < 8;l++){
			__NOP();
		}
	}
}

void _delay_ms(uint16_t _ms){
	
	uint16_t i, j, l;
	for(i = 0;i < _ms;i++){
		for(j = 0;j < 1000;j++){
			for(l = 0;l < 8;l++){
				__NOP();
			}
		}
	}
}


void patternB(void *p){
	while(1){
		for(uint8_t i = 0;i < 8;i++){
			clearMatrix();
			line(7 - i,7,0,i, 0, 7);
			vTaskDelay(50);
		}
		for(uint8_t i = 0;i < 8;i++){
			clearMatrix();
			line(0,7 - i,0,7, i, 7);
			vTaskDelay(50);
		}
		for(uint8_t i = 0;i < 8;i++){
			clearMatrix();
			line(i,0,0,7-i, 7, 7);
			vTaskDelay(50);
		}
		for(uint8_t i = 0;i < 8;i++){
			clearMatrix();
			line(7,i,0,0, 7-i, 7);
			vTaskDelay(50);
		}
	}
}

void matrix_refresh(void *p){
	while(1){
		for(uint8_t i = 0;i < 64;i++){
			uint8_t row  = i / 8;
			uint8_t col  = i % 8;
			SPIx_Transfer((1 << row));
			SPIx_Transfer(~(1 << col));
			SPIx_Transfer(~matrix[row][col]);
			SPIx_EnableSlave();
			_delay_us(1);
			SPIx_DisableSlave();
			vTaskDelay(1);
		}
	}
}

int main(void)
{
	SPIx_Init();
	//ledInit();
	SPIx_Transfer(0x01);
	SPIx_Transfer(~(0x01));
	SPIx_Transfer(~(0x05));
	SPIx_EnableSlave();
	_delay_us(1);
	SPIx_DisableSlave();
	
	xTaskCreate(patternB, (const char*)"pattern", 128, NULL, 1, NULL);
	xTaskCreate(matrix_refresh, (const char*) "refresh", 128, NULL, 1, NULL);
	// Start RTOS scheduler
	vTaskStartScheduler();

}


void SPIx_Init()
{
	// Initialization struct
	SPI_InitTypeDef SPI_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// Step 1: Initialize SPI
	RCC_APB2PeriphClockCmd(SPIx_RCC, ENABLE);
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
	SPI_Init(SPIx, &SPI_InitStruct); 
	SPI_Cmd(SPIx, ENABLE);
	
	// Step 2: Initialize GPIO
	RCC_APB2PeriphClockCmd(SPI_GPIO_RCC, ENABLE);
	// GPIO pins for MOSI, MISO, and SCK
	GPIO_InitStruct.GPIO_Pin = SPI_PIN_MOSI | SPI_PIN_MISO | SPI_PIN_SCK;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
	// GPIO pin for SS
	GPIO_InitStruct.GPIO_Pin = SPI_PIN_SS;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO, &GPIO_InitStruct);
	
	// Disable SPI slave device
	SPIx_DisableSlave();
}

uint8_t SPIx_Transfer(uint8_t data)
{
	// Write data to be transmitted to the SPI data register
	SPIx->DR = data;
	// Wait until transmit complete
	while (!(SPIx->SR & (SPI_I2S_FLAG_TXE)));
	// Wait until receive complete
	while (!(SPIx->SR & (SPI_I2S_FLAG_RXNE)));
	// Wait until SPI is not busy anymore
	while (SPIx->SR & (SPI_I2S_FLAG_BSY));
	// Return received data from SPI data register
	return SPIx->DR;
}

void SPIx_EnableSlave()
{
	// Set slave SS pin low
	SPI_GPIO->BRR = SPI_PIN_SS;
}

void SPIx_DisableSlave()
{
	// Set slave SS pin high
	SPI_GPIO->BSRR = SPI_PIN_SS;
}
