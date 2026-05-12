#ifndef __KEY_H__
#define __KEY_H__

#include <reg52.h>

#ifndef uchar
#define uchar unsigned char
#endif

typedef enum
{
    KEY_EVENT_NONE = 0,
    KEY_EVENT_PREV,
    KEY_EVENT_NEXT,
    KEY_EVENT_AUTO_TOGGLE
} KeyEvent;

KeyEvent Key_Scan10ms(void);

#endif
