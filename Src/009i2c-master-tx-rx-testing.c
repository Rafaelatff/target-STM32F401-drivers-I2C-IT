/*
 * 009i2c-master-tx-rx-testing.c
 *
 *  Created on: Dec 27, 2022
 *      Author: rafaela
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h> // to use strlen() function
#include "stm32f401xx.h"

extern void initialise_monitor_handles(); // to work semihosting
// also in debug config. -> 'Startup' add in 'Run Commands': monitor arm semihosting enable

#define MY_ADDR_I2C1 		0x61
#define SLAVE_ADDR_I2C2		0x78
#define MY_ADDR_I2C2 		SLAVE_ADDR_I2C2


// I2C1_SCL - PB8 (AF04)
// I2C1_SDA - PB9 (AF04)

// I2C2_SCL - PB10 (AF04)
// I2C2_SDA - PB3 (AF04)


void I2C1_GPIOInits(void);
void GPIO_ButtonInit(void);
void I2C1_Inits(void);

void delay(void){
	for (uint32_t i=0; i<500000; i++);
}

I2C_Handle_t I2C1Handle;
uint8_t some_data[] = "We are testing I2C master Tx\n"; //must be less than 32 bytes

int main (void){
	uint8_t commandcode;
	uint8_t len;

	initialise_monitor_handles(); // semihosting
	printf("Application is running\n");

	GPIO_ButtonInit(); ////Initialize GPIO pin for B1

	I2C1_GPIOInits(); //Initialize GPIO pins to behave as I2C1 and I2C2 pins
	I2C1_Inits(); //Initialize the I2C1 peripheral parameters
	I2C_PeripheralControl(I2C1, ENABLE);
	I2C_AckControl(I2C1, I2C_ACK_ENABLE); // moved here since there is a function for that

	while(1){
		while(GPIO_Read(GPIOC, GPIO_PIN_NO_13));
		delay();
		I2C_MasterSendData(&I2C1Handle, some_data, strlen((char*)some_data), SLAVE_ADDR, 0);
		delay();
		I2C_MasterReceiveData(&I2C1Handle, rcv_buf,1, SLAVE_ADDR);
		rcv_buf[len+1] = '\0'; //  add null character
		printf("Data: %s", rcv_buf);
	}

}

void I2C1_GPIOInits(void){
	GPIO_Handle_t I2C1Pins, I2C2Pins;

		//I2C1 config GPIO pins
	I2C1Pins.pGPIOx = GPIOB;
	I2C1Pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	I2C1Pins.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_OD;
	I2C1Pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_PIN_PU;
	I2C1Pins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
	I2C1Pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	//SCL
	I2C1Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_8;
	GPIO_Init(&I2C1Pins);
	//SDA
	I2C1Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_9;
	GPIO_Init(&I2C1Pins);

		//I2C2 config GPIO pins
	I2C2Pins.pGPIOx = GPIOB;
	I2C2Pins.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	I2C2Pins.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_OD;
	I2C2Pins.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	I2C2Pins.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
	I2C2Pins.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	//SCL
	I2C2Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_10;
	GPIO_Init(&I2C2Pins);
	//SDA
	I2C2Pins.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_3;
	GPIO_Init(&I2C2Pins);
}

void I2C1_Inits(void){
	I2C1Handle.pI2Cx = I2C1;
	//I2C1Handle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE;
	I2C1Handle.I2C_Config.I2C_ACKControl = ENABLE;
	I2C1Handle.I2C_Config.I2C_DeviceAddress = MY_ADDRESS; //Master address (not slave)
	//I2C1Handle.I2C_Config.I2C_FMDutyCycle = I2C_FM_DUTY_2; // Does't matter since its SM
	I2C1Handle.I2C_Config.I2C_SCLSpeed = I2C_SCL_SPEED_SM;

	I2C_Init(&I2C1Handle);
}

void GPIO_ButtonInit(void){
	GPIO_Handle_t B1;

	//B1 configuration
	B1.pGPIOx = GPIOC;
	B1.GPIO_PinConfig.GPIO_PinNumber = GPIO_PIN_NO_13;
	B1.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_IN;
	B1.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	B1.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD; //It has an external PU

	GPIO_Init(&B1);
}
