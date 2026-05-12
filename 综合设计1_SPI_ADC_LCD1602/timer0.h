#ifndef __TIMER0_H__
#define __TIMER0_H__

#ifndef ulong
#define ulong unsigned long
#endif

void Timer0_Init(void);
ulong Timer0_GetTickMs(void);

#endif
