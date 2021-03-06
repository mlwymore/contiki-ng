/*
 * Copyright (c) 2017, RISE SICS.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         NullNet broadcast example
 * \author
*         Simon Duquennoy <simon.duquennoy@ri.se>
 *
 */

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
#define SEND_INTERVAL (CLOCK_SECOND)
// #define PIN_HIGH_INTERVAL (1 * CLOCK_SECOND)

#if MAC_CONF_WITH_TSCH
#include "net/mac/tsch/tsch.h"
static linkaddr_t coordinator_addr =  {{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};
#endif /* MAC_CONF_WITH_TSCH */

struct data_packet {
  unsigned seqno;
  uint8_t dummy_data[30];
};

/*---------------------------------------------------------------------------*/
PROCESS(nullnet_example_process, "NullNet broadcast example");
// PROCESS(send_reset, "Send reset signal to harvester");
AUTOSTART_PROCESSES(&nullnet_example_process);
// AUTOSTART_PROCESSES(&nullnet_example_process, &send_reset);

// AUTOSTART_PROCESSES(&send_reset);

/*---------------------------------------------------------------------------*/
void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  printf("Receive callback\n");
  if(len == sizeof(struct data_packet)) {
     unsigned count;
     memcpy(&count, data, sizeof(count));
     LOG_INFO("Received %u from ", count);
     LOG_INFO_LLADDR(src);
     LOG_INFO_("\n");
    // LED PIN 10 and GPIO PIN (DP0) is 25
    // GPIO_setOutputEnableDio(10, GPIO_OUTPUT_ENABLE);
    //GPIO_setOutputEnableDio(25, GPIO_OUTPUT_ENABLE);
    // GPIO_setDio(10);
    //GPIO_setDio(25);
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nullnet_example_process, ev, data)
{
  static struct etimer periodic_timer;
  static unsigned count = 0;

  PROCESS_BEGIN();

#if MAC_CONF_WITH_TSCH
  tsch_set_coordinator(linkaddr_cmp(&coordinator_addr, &linkaddr_node_addr));
#endif /* MAC_CONF_WITH_TSCH */

  /* Initialize NullNet */
  nullnet_buf = (uint8_t *)&count;
  nullnet_len = sizeof(count);

  nullnet_set_input_callback(input_callback);

  while(1) {    
    // LOG_INFO_("\n");
    LOG_INFO("I'm alive!\n");
    /*LOG_INFO("Sending %u to ", count);
    LOG_INFO_LLADDR(NULL);
    LOG_INFO_("\n");
    
    memcpy(nullnet_buf, &count, sizeof(count));
    nullnet_len = sizeof(count);

    NETSTACK_NETWORK.output(NULL);
    count++;*/
    etimer_set(&periodic_timer, SEND_INTERVAL);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
  }
  // Function to turn radio off
  // NETSTACK_MAC.off();
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
