/*
 * matrix_display.h
 *
 * Created: 04-Dec-20 10:59:43 PM
 *  Author: MohamedMerdan
 */ 

#include <stdint.h>
#include "USART.h"

#ifndef MATRIX_DISPLAY_H_
#define MATRIX_DISPLAY_H_

void clearMatrix();
void setPoint(uint8_t X, uint8_t Y, uint8_t Z);
void clearPoint(uint8_t X, uint8_t Y, uint8_t Z);
void line(uint8_t X0, uint8_t Y0, uint8_t Z0, uint8_t X1, uint8_t Y1, uint8_t Z1);


#endif /* MATRIX_DISPLAY_H_ */