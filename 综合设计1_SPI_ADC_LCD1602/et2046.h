#ifndef __ET2046_H__
#define __ET2046_H__

#include <reg52.h>
#include <intrins.h>

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

/*
 * ET2046 control command (12-bit mode, software-SPI compatible with existing XPT2046 sample):
 *  - AIN2 = 0xA4, AIN3 = 0xE4 are verified in 原实验27示例.
 *  - AIN0/AIN1 command values are extended by the same command encoding pattern.
 */
#define ET2046_CMD_AIN0 0x84u
#define ET2046_CMD_AIN1 0xC4u
#define ET2046_CMD_AIN2 0xA4u
#define ET2046_CMD_AIN3 0xE4u

uint ET2046_ReadRaw(uchar cmd);
uint ET2046_ReadAvg(uchar cmd, uchar sample_count);

#endif
