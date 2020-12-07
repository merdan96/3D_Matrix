/*
 * matrix_display.c
 *
 * Created: 04-Dec-20 11:27:11 PM
 *  Author: MohamedMerdan
 */ 

#include "matrix_display.h"

//			   Y  X       Z
uint8_t matrix[8][8] = {{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
						{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
						{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
						{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
						{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
						{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
						{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
						{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

void clearMatrix(){
	for(uint8_t i = 0;i < 64;i++){
		matrix[i % 8][i / 8] = 0x00;
	}
}

void setPoint(uint8_t X, uint8_t Y, uint8_t Z){
	//serialPrintf("(/d, /d, /d),", X, Y, Z);
	matrix[Y][X] |= 1 << Z;
}

void clearPoint(uint8_t X, uint8_t Y, uint8_t Z){
	matrix[Y][X] &= ~(1 << Z);
}

void line(uint8_t X0, uint8_t Y0, uint8_t Z0, uint8_t X1, uint8_t Y1, uint8_t Z1){
	setPoint(X0, Y0, Z0);
	
	uint8_t dx = (X1 > X0) ? (X1 - X0) : (X0 - X1);
	uint8_t dy = (Y1 > Y0) ? (Y1 - Y0) : (Y0 - Y1);
	uint8_t dz = (Z1 > Z0) ? (Z1 - Z0) : (Z0 - Z1);
	
	int8_t xs = (X1 > X0) ? 1 : -1;
	int8_t ys = (Y1 > Y0) ? 1 : -1;
	int8_t zs = (Z1 > Z0) ? 1 : -1;
	
//	# Driving axis is X-axis
	if (dx >= dy && dx >= dz){
		int16_t p1 = 2 * dy - dx;
		int16_t p2 = 2 * dz - dx;
		while (X0 != X1){
			X0 += xs;
			if (p1 >= 0){
				Y0 += ys;
				p1 -= 2 * dx;
			}
			if (p2 >= 0){
				Z0 += zs;
				p2 -= 2 * dx;
			}
			p1 += 2 * dy;
			p2 += 2 * dz;
			setPoint(X0, Y0, Z0);
		}
	}
	
//	# Driving axis is Y-axis
	else if (dy >= dx && dy >= dz){
		int16_t p1 = 2 * dx - dy;
		int16_t p2 = 2 * dz - dy;
		while (Y0 != Y1){
			Y0 += ys;
			if (p1 >= 0){
				X0 += xs;
				p1 -= 2 * dy;
			}
			if (p2 >= 0){
				Z0 += zs;
				p2 -= 2 * dy;
			}
			p1 += 2 * dx;
			p2 += 2 * dz;
			setPoint(X0, Y0, Z0);
		}
	}
//	# Driving axis is Z-axis
	
	else{
		int16_t p1 = 2 * dx - dz;
		int16_t p2 = 2 * dy - dz;
		while (Z0 != Z1){
			Z0 += zs;
			if (p1 >= 0){
				X0 += xs;
				p1 -= 2 * dz;
			}
			if (p2 >= 0){
				Y0 += ys;
				p2 -= 2 * dz;
			}
			p1 += 2 * dx;
			p2 += 2 * dy;
			setPoint(X0, Y0, Z0);
		}
	}
	
}