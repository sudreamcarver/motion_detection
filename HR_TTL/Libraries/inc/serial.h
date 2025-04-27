#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdint.h>
void serial_init(void);
void serial_sendbyte(uint8_t Byte);
void serial_sendarray(uint8_t *Array, uint16_t Length);
void serial_sendstring(char *string);
void serial_sendnumber(uint32_t Number, uint8_t Length);
uint8_t serial_GetRxData(void);
uint8_t serial_GetRxFlag(void);

#endif
