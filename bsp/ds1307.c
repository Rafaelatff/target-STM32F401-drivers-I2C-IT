/*
 * ds1307.c
 *
 *  Created on: 28 de dez de 2022
 *      Author: r.freitas
 */

#include <stdint.h>
#include <string.h>
#include "ds1307.h"

// I2C addresses
// Register addresses
// Data structure to handle the information <3
// Function prototypes which are exposed to applications
// Application configurable items

/*
 * 	Local function prototypes
 */
static void ds1307_i2c_pin_config(void);
static void ds1307_i2c_config(void);
static void ds1303_write(uint8_t reg_addr, uint8_t value);
static uint8_t ds1307_read(uint8_t reg_addr);

/*
 * 	Variables
 */
I2C_Handle_t g_ds1307I2cHandle; // g_ for global

uint8_t ds1307_init(void){

	// 1. Initialize the I2C pins
	ds1307_i2c_pin_config();

	// 2. Initialize the I2C peripheral
	ds1307_i2c_config();

	// 3. Enable the I2C peripheral
	I2C_PeripheralControl(DS1307_I2C, ENABLE);

	// 4. Make clock halt (CH) equal to 0
	ds1303_write (DS1307_ADDR_SEC, 0x00);

	// 5. Read back CH bit
	uint8_t clock_state = ds1307_read(DS1307_ADDR_SEC);

	return (clock_state >> 7) & 0x1; // CH = bit masked with 0x1;
	// if this function return the value 1, CH = 1 - FAIL
	// if this function return the value 0, CH = 0 - SUCCESS
}

void ds1307_set_current_time(RTC_time_t *rtc_time){

}
void ds1307_get_current_time(RTC_time_t *rtc_time){

}

void ds1307_set_current_date(RTC_date_t *rtc_date){

}
void ds1307_get_current_date(RTC_date_t *rtc_date){

}

/*
 * 	Local (helper) functions
 */

static void ds1307_i2c_pin_config(void){
	GPIO_Handle_t i2c_sda, i2c_scl;

	memset(&i2c_sda, 0, sizeof(i2c_sda));
	memset(&i2c_scl, 0, sizeof(i2c_scl));

	// I2C1_SDA - PB9 (AF04)
	i2c_sda.pGPIOx = DS1307_I2C_GPIO_PORT;
	i2c_sda.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
	i2c_sda.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	i2c_sda.GPIO_PinConfig.GPIO_PinNumber = GPIO_I2C_SDA_PIN;
	i2c_sda.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_OD;
	i2c_sda.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	i2c_sda.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_Init(&i2c_sda);

	// I2C1_SCL - PB8 (AF04)
	i2c_sda.pGPIOx = DS1307_I2C_GPIO_PORT;
	i2c_scl.GPIO_PinConfig.GPIO_PinAltFunMode = 4;
	i2c_scl.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFN;
	i2c_scl.GPIO_PinConfig.GPIO_PinNumber = GPIO_I2C_SCL_PIN;
	i2c_scl.GPIO_PinConfig.GPIO_PinOpType = GPIO_OP_TYPE_OD;
	i2c_scl.GPIO_PinConfig.GPIO_PinPuPdControl = GPIO_NO_PUPD;
	i2c_scl.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_FAST;
	GPIO_Init(&i2c_scl);
}

static void ds1307_i2c_config(void){
	g_ds1307I2cHandle.pI2Cx = DS1307_I2C;
	g_ds1307I2cHandle.I2C_Config.I2C_ACKControl = I2C_ACK_ENABLE; // Don't know why (doesn't work bf set PE)
	g_ds1307I2cHandle.I2C_Config.I2C_SCLSpeed = DS1307_I2C_SPEED;
	I2C_Init(&g_ds1307I2cHandle);
}

static void ds1303_write(uint8_t reg_addr, uint8_t value){
	uint8_t tx[2];
	tx[0] = reg_adr;
	tx[1] = value;
	I2C_MasterSendData(&g_ds1307I2cHandle, tx, 2, DS1307_I2C_ADDR, 0);
}

static uint8_t ds1307_read(uint8_t reg_addr){
	uint8_t data;
	I2C_MasterSendData(&g_ds1307I2cHandle, &reg_addr, 1, DS1307_I2C_ADDR, 0);
	I2C_MasterReceiveData(&g_ds1307I2cHandle, &data, 1, DS1307_I2C_ADDR);
	return data;
}
