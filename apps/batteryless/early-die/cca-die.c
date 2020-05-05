#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "arch/cpu/cc26x0-cc13x0/lib/cc26xxware/driverlib/gpio.h"
// #include "cc26"
#include <string.h>
#include <stdio.h> /* For printf() */

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Configuration */
#define WAIT_INTERVAL (0.005 * CLOCK_SECOND)
#define SEND_INTERVAL (1 * CLOCK_SECOND)

#if MAC_CONF_WITH_TSCH
#include "net/mac/tsch/tsch.h"
static linkaddr_t coordinator_addr =  {{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};
#endif /* MAC_CONF_WITH_TSCH */

/*---------------------------------------------------------------------------*/
PROCESS(nullnet_process, "NullNet broadcast example");
AUTOSTART_PROCESSES(&nullnet_process);
/*---------------------------------------------------------------------------*/
void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  if(len == sizeof(unsigned)) {
    unsigned count;
    unsigned long current_time;

    memcpy(&count, data, sizeof(count));
    current_time = clock_seconds();
    LOG_INFO_("Time: %lu ", current_time);
    LOG_INFO("Received %u from ", count);
    LOG_INFO_LLADDR(src);
    LOG_INFO_("\n");
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nullnet_process, ev, data)
{  
//   static unsigned count = 0;
//   static struct etimer periodic_timer;
//   etimer_set(&periodic_timer, SEND_INTERVAL);

  void clock_init(void);

  PROCESS_BEGIN();

#if MAC_CONF_WITH_TSCH
  tsch_set_coordinator(linkaddr_cmp(&coordinator_addr, &linkaddr_node_addr));
#endif /* MAC_CONF_WITH_TSCH */

  /* Initialize NullNet */
//   nullnet_buf = (uint8_t *)&count;
//   nullnet_len = sizeof(count);

//   nullnet_set_input_callback(input_callback);  

  while(1) {       
    //   PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    LOG_INFO_("I'm Alive and doing CCA\n");
    // CCA Check
    NETSTACK_RADIO.channel_clear();
    // set GPIO Pin 25 or DP0 as output pin
    GPIO_setOutputEnableDio(25, GPIO_OUTPUT_ENABLE);
    // toggle
    GPIO_setDio(25);
  }
  PROCESS_END();
}