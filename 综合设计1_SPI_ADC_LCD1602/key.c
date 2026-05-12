#include "key.h"

sbit KEY1 = P3^1;
sbit KEY2 = P3^0;
sbit KEY3 = P3^2;

#define KEY_MASK_1 0x01u
#define KEY_MASK_2 0x02u
#define KEY_MASK_3 0x04u

static uchar Key_ReadRawState(void)
{
    uchar state = 0;

    if (KEY1)
    {
        state |= KEY_MASK_1;
    }
    if (KEY2)
    {
        state |= KEY_MASK_2;
    }
    if (KEY3)
    {
        state |= KEY_MASK_3;
    }

    return state;
}

KeyEvent Key_Scan10ms(void)
{
    static uchar last_raw_state = 0x07;
    static uchar stable_state = 0x07;
    static uchar debounce_count = 0;

    uchar raw_state;
    uchar changed;
    uchar press_mask;

    raw_state = Key_ReadRawState();

    if (raw_state != last_raw_state)
    {
        last_raw_state = raw_state;
        debounce_count = 0;
        return KEY_EVENT_NONE;
    }

    if (debounce_count < 2)
    {
        debounce_count++;
        return KEY_EVENT_NONE;
    }

    changed = stable_state ^ raw_state;
    if (changed == 0)
    {
        return KEY_EVENT_NONE;
    }

    stable_state = raw_state;
    press_mask = changed & (uchar)(~raw_state);

    if (press_mask & KEY_MASK_1)
    {
        return KEY_EVENT_PREV;
    }
    if (press_mask & KEY_MASK_2)
    {
        return KEY_EVENT_NEXT;
    }
    if (press_mask & KEY_MASK_3)
    {
        return KEY_EVENT_AUTO_TOGGLE;
    }

    return KEY_EVENT_NONE;
}
