#include "app.h"
#include "timer0.h"

void main(void)
{
    Timer0_Init();
    App_Init();

    while (1)
    {
        App_Task();
    }
}
