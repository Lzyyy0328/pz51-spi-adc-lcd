#include <reg52.h>
#include "timer0.h"

static volatile ulong g_timer0_tick_ms = 0;

void Timer0_Init(void)
{
    TMOD &= 0xF0;
    TMOD |= 0x01;

    TH0 = 0xFC;
    TL0 = 0x18;

    ET0 = 1;
    EA = 1;
    TR0 = 1;
}

ulong Timer0_GetTickMs(void)
{
    ulong tick_snapshot;

    EA = 0;
    tick_snapshot = g_timer0_tick_ms;
    EA = 1;

    return tick_snapshot;
}

void Timer0_ISR(void) interrupt 1
{
    TH0 = 0xFC;
    TL0 = 0x18;

    g_timer0_tick_ms++;
}
