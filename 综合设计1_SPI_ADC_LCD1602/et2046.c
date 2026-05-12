#include "et2046.h"

sbit ET2046_DOUT = P3^7;
sbit ET2046_CLK  = P3^6;
sbit ET2046_DIN  = P3^4;
sbit ET2046_CS   = P3^5;

static void ET2046_SPI_Write(uchar dat)
{
    uchar i;

    for (i = 0; i < 8; i++)
    {
        ET2046_DIN = dat >> 7;
        dat <<= 1;

        ET2046_CLK = 0;
        _nop_();
        ET2046_CLK = 1;
    }

    ET2046_CLK = 0;
}

static uint ET2046_SPI_Read12(void)
{
    uchar i;
    uint dat = 0;

    ET2046_CLK = 0;
    for (i = 0; i < 12; i++)
    {
        dat <<= 1;

        ET2046_CLK = 1;
        _nop_();
        ET2046_CLK = 0;

        dat |= ET2046_DOUT;
    }

    return dat;
}

uint ET2046_ReadRaw(uchar cmd)
{
    uchar i;
    uint adc_value;

    ET2046_CLK = 0;
    ET2046_CS = 0;

    ET2046_SPI_Write(cmd);

    for (i = 0; i < 6; i++)
    {
        _nop_();
    }

    ET2046_CLK = 1;
    _nop_();
    _nop_();
    ET2046_CLK = 0;

    adc_value = ET2046_SPI_Read12();

    ET2046_CS = 1;

    return adc_value;
}

uint ET2046_ReadAvg(uchar cmd, uchar sample_count)
{
    uchar i;
    unsigned long sum = 0;

    if (sample_count == 0)
    {
        return ET2046_ReadRaw(cmd);
    }

    for (i = 0; i < sample_count; i++)
    {
        sum += ET2046_ReadRaw(cmd);
    }

    return (uint)(sum / sample_count);
}
