#include "contiki.h"
#include "arch/cpu/cc26x0-cc13x0/lib/cc26xxware/driverlib/gpio.h"

/*---------------------------------------------------------------------------*/
PROCESS(gpio_process, "GPIO reset process");
AUTOSTART_PROCESSES(&gpio_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(gpio_process, ev, data)
{
    PROCESS_BEGIN();

    // set LED 2 Pin 15 on
    GPIO_setOutputEnableDio(15, GPIO_OUTPUT_ENABLE);
    GPIO_setDio(15);

    PROCESS_END();    
}
/*---------------------------------------------------------------------------*/
