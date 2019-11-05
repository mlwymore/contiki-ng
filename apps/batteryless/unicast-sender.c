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

#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "net/mac/mac.h"
#include "net/packetbuf.h"
#include <string.h>
#include <stdio.h> /* For printf() */
#include "arch/cpu/cc26x0-cc13x0/lib/cc26xxware/driverlib/gpio.h"


/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Configuration */
#define SEND_INTERVAL (1 * CLOCK_SECOND)
static linkaddr_t dest_addr =         {{ 0x00, 0x12, 0x4b, 0x00, 0x0c, 0x4a, 0x43, 0x86 }}; //Sensortag 16

#if MAC_CONF_WITH_TSCH
#include "net/mac/tsch/tsch.h"
static linkaddr_t coordinator_addr =  {{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};
#endif /* MAC_CONF_WITH_TSCH */

struct data_packet {
  unsigned seqno;
  uint8_t dummy_data[30];
};

/*---------------------------------------------------------------------------*/
PROCESS(nullnet_example_process, "NullNet unicast example");
AUTOSTART_PROCESSES(&nullnet_example_process);

/*---------------------------------------------------------------------------*/
void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  if(len == sizeof(struct data_packet)) {
    unsigned count;
    memcpy(&count, data, sizeof(count));
    LOG_INFO("Received %u from ", count);
    LOG_INFO_LLADDR(src);
    LOG_INFO_("\n");
  }
}
/*---------------------------------------------------------------------------*/
void mac_sent_callback(void * ptr, int status, int transmissions) {
  if(status == MAC_TX_OK) {
    LOG_INFO("Sent successfully.\n");
    GPIO_clearDio(25);
  } else if(status == MAC_TX_NOACK) {
    LOG_INFO("No ACK!\n");
  } else {
    LOG_INFO("Send failed.\n");
  }
  process_post(&nullnet_example_process, PROCESS_EVENT_CONTINUE, NULL);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nullnet_example_process, ev, data)
{
  //static struct etimer periodic_timer;
  static struct data_packet pkt;
  static int count = 0;

  PROCESS_BEGIN();

#if MAC_CONF_WITH_TSCH
  tsch_set_coordinator(linkaddr_cmp(&coordinator_addr, &linkaddr_node_addr));
#endif /* MAC_CONF_WITH_TSCH */

  /* Initialize NullNet */
  //nullnet_buf = (uint8_t *)&pkt;
  //nullnet_len = sizeof(struct data_packet);
  //nullnet_set_input_callback(input_callback);

  //etimer_set(&periodic_timer, SEND_INTERVAL);
  GPIO_setOutputEnableDio(25, GPIO_OUTPUT_ENABLE);


  while(1) {
    LOG_INFO("Sending %u to ", pkt.seqno);
    LOG_INFO_LLADDR(&dest_addr);
    LOG_INFO_("\n");
    
    pkt.seqno = count;
    /*memcpy(nullnet_buf, &pkt, sizeof(struct data_packet));
    nullnet_len = sizeof(struct data_packet);

    NETSTACK_NETWORK.output(&dest_addr);*/
    LOG_INFO_("GPIO read before set: %lu\n", GPIO_readDio(25));
    // set GPIO Pin 25 or DP0 as output pin
    GPIO_setDio(25);

    int dio25=0;
    int tries = 0;
    while(!(dio25=GPIO_readDio(25))) {tries++;}
    LOG_INFO_("GPIO read tries: %d\n", tries);  
    packetbuf_clear();
    packetbuf_copyfrom(&pkt, sizeof(struct data_packet));
    packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, &dest_addr);
    packetbuf_set_addr(PACKETBUF_ADDR_SENDER, &linkaddr_node_addr);
    NETSTACK_MAC.send(mac_sent_callback, NULL);

    count++;
    //etimer_set(&periodic_timer, SEND_INTERVAL);
    //PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_CONTINUE);
  

  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
