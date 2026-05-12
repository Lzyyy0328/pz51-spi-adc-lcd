#include "app.h"
#include "timer0.h"
#include "key.h"
#include "et2046.h"
#include "lcd.h"

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

#ifndef ulong
#define ulong unsigned long
#endif

#define APP_CHANNEL_COUNT          2u
#define APP_VREF_MV                3300u
#define APP_SAMPLE_PERIOD_MS       100ul
#define APP_LCD_REFRESH_PERIOD_MS  200ul
#define APP_KEY_SCAN_PERIOD_MS     10ul
#define APP_AUTO_SWITCH_PERIOD_MS  1000ul
#define APP_SAMPLE_AVG_COUNT       4u

static const uchar g_channel_cmds[APP_CHANNEL_COUNT] =
{
    ET2046_CMD_AIN0,
    ET2046_CMD_AIN1
};

static uint  g_channel_raw[APP_CHANNEL_COUNT];
static uchar g_current_channel = 0;
static bit   g_auto_mode = 0;

static void App_HandleKey(KeyEvent key_event)
{
    if (key_event == KEY_EVENT_PREV)
    {
        if (g_current_channel == 0)
        {
            g_current_channel = APP_CHANNEL_COUNT - 1;
        }
        else
        {
            g_current_channel--;
        }
    }
    else if (key_event == KEY_EVENT_NEXT)
    {
        g_current_channel++;
        if (g_current_channel >= APP_CHANNEL_COUNT)
        {
            g_current_channel = 0;
        }
    }
    else if (key_event == KEY_EVENT_AUTO_TOGGLE)
    {
        g_auto_mode = !g_auto_mode;
    }
}

static void App_SampleChannels(void)
{
    uchar i;

    for (i = 0; i < APP_CHANNEL_COUNT; i++)
    {
        g_channel_raw[i] = ET2046_ReadAvg(g_channel_cmds[i], APP_SAMPLE_AVG_COUNT);
    }
}

static uint App_ConvertToMv(uint adc_raw)
{
    ulong value;

    value = (ulong)adc_raw * (ulong)APP_VREF_MV;
    value = (value + 2047ul) / 4095ul;

    return (uint)value;
}

static void App_WriteLine(uchar addr, char *line)
{
    uchar i;

    LcdWriteCom(addr);
    for (i = 0; i < 16; i++)
    {
        LcdWriteData(line[i]);
    }
}

static void App_FormatLine1(char *line, uchar channel, uint adc_raw)
{
    uchar i;

    for (i = 0; i < 16; i++)
    {
        line[i] = ' ';
    }

    line[0] = 'C';
    line[1] = 'H';
    line[2] = ':';
    line[3] = '0' + channel;
    line[5] = 'A';
    line[6] = 'D';
    line[7] = 'C';
    line[8] = ':';

    line[9]  = (char)('0' + (adc_raw / 1000) % 10);
    line[10] = (char)('0' + (adc_raw / 100) % 10);
    line[11] = (char)('0' + (adc_raw / 10) % 10);
    line[12] = (char)('0' + adc_raw % 10);
}

static void App_FormatLine2(char *line, uint voltage_mv)
{
    uchar i;
    uint frac_mv;

    for (i = 0; i < 16; i++)
    {
        line[i] = ' ';
    }

    frac_mv = voltage_mv % 1000;

    line[0] = 'U';
    line[1] = ':';
    line[2] = (char)('0' + (voltage_mv / 1000));
    line[3] = '.';
    line[4] = (char)('0' + (frac_mv / 100) % 10);
    line[5] = (char)('0' + (frac_mv / 10) % 10);
    line[6] = (char)('0' + frac_mv % 10);
    line[7] = 'V';
    line[9] = 'A';
    line[10] = 'U';
    line[11] = 'T';
    line[12] = 'O';
    line[13] = ':';
    line[14] = 'O';
    line[15] = g_auto_mode ? 'N' : 'F';
}

static void App_RefreshLcd(void)
{
    char line1[16];
    char line2[16];
    uint adc_raw;
    uint voltage_mv;

    adc_raw = g_channel_raw[g_current_channel];
    voltage_mv = App_ConvertToMv(adc_raw);

    App_FormatLine1(line1, g_current_channel, adc_raw);
    App_FormatLine2(line2, voltage_mv);

    App_WriteLine(0x80, line1);
    App_WriteLine(0xC0, line2);
}

void App_Init(void)
{
    LcdInit();
    App_SampleChannels();
    App_RefreshLcd();
}

void App_Task(void)
{
    static ulong last_key_scan_ms = 0;
    static ulong last_sample_ms = 0;
    static ulong last_lcd_refresh_ms = 0;
    static ulong last_auto_switch_ms = 0;

    ulong now_ms;
    KeyEvent key_event;

    now_ms = Timer0_GetTickMs();

    if ((now_ms - last_key_scan_ms) >= APP_KEY_SCAN_PERIOD_MS)
    {
        last_key_scan_ms = now_ms;
        key_event = Key_Scan10ms();
        App_HandleKey(key_event);
    }

    if ((now_ms - last_sample_ms) >= APP_SAMPLE_PERIOD_MS)
    {
        last_sample_ms = now_ms;
        App_SampleChannels();
    }

    if (g_auto_mode && ((now_ms - last_auto_switch_ms) >= APP_AUTO_SWITCH_PERIOD_MS))
    {
        last_auto_switch_ms = now_ms;
        g_current_channel++;
        if (g_current_channel >= APP_CHANNEL_COUNT)
        {
            g_current_channel = 0;
        }
    }

    if ((now_ms - last_lcd_refresh_ms) >= APP_LCD_REFRESH_PERIOD_MS)
    {
        last_lcd_refresh_ms = now_ms;
        App_RefreshLcd();
    }
}
