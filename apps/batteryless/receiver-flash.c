#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "arch/cpu/cc26x0-cc13x0/lib/cc26xxware/driverlib/gpio.h"
#include "os/storage/cfs/cfs-coffee.h"
#include "dev/button-hal.h"

// #include "cc26"
#include <string.h>
#include <stdio.h> /* For printf() */

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Configuration */
#define WAIT_INTERVAL (0.005 * CLOCK_SECOND)
#define SEND_INTERVAL (CLOCK_SECOND)

/* Variables: the application specific event value */
static process_event_t PACKET_RECEIVED;

#if MAC_CONF_WITH_TSCH
#include "net/mac/tsch/tsch.h"
static linkaddr_t coordinator_addr =  {{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};
#endif /* MAC_CONF_WITH_TSCH */

struct data_packet {
  unsigned seqno;
  uint8_t dummy_data[30];
};

/*---------------------------------------------------------------------------*/
PROCESS(nullnet_process, "NullNet receiver");
PROCESS(flashwrite_process, "Flash write");
PROCESS(flashread_process, "Flash Read");
PROCESS(flashclear_process, "Flash Remove file");
PROCESS(gpio_process, "GPIO reset process");
AUTOSTART_PROCESSES(&nullnet_process, &flashwrite_process, &flashread_process, &flashclear_process, &gpio_process);

/*---------------------------------------------------------------------------*/
void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  if(len == sizeof(struct data_packet)) {
    static unsigned count;
    unsigned long current_time;

    memcpy(&count, data, sizeof(count));
    current_time = clock_seconds();
    LOG_INFO_("Time: %lu ", current_time);
    LOG_INFO("Received %u from ", count);
    LOG_INFO_LLADDR(src);
    LOG_INFO_("\n");
    // set GPIO Pin 25 or DP0 as output pin
    GPIO_setOutputEnableDio(25, GPIO_OUTPUT_ENABLE);
    // toggle
    GPIO_setDio(25);

    // Generate an messeage receive event
    process_post(&flashwrite_process, PACKET_RECEIVED, &count);
    process_post(&gpio_process, PROCESS_EVENT_MSG, &count);
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(nullnet_process, ev, data)
{  
  static unsigned count = 0;
  static struct etimer periodic_timer;
  // etimer_set(&periodic_timer, SEND_INTERVAL);

  void clock_init(void);

  PROCESS_BEGIN();

  /* allocate the required event */
  PACKET_RECEIVED = process_alloc_event();

  #if MAC_CONF_WITH_TSCH
  tsch_set_coordinator(linkaddr_cmp(&coordinator_addr, &linkaddr_node_addr));
  #endif /* MAC_CONF_WITH_TSCH */

  /* Initialize NullNet */
  nullnet_buf = (uint8_t *)&count;
  nullnet_len = sizeof(count);

  nullnet_set_input_callback(input_callback);  

  LOG_INFO_LLADDR(&linkaddr_node_addr);

  while(1) {       
    LOG_INFO_("I'm Alive\n");
    etimer_set(&periodic_timer, SEND_INTERVAL);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
  }

  PROCESS_END();

  }
  /*---------------------------------------------------------------------------*/
  void flash_write(int fd, int * data) 
  {
    int write_status = 0, len = 1;

    // Write received count to flash
    write_status = cfs_write(fd, (void*)data, len);
    if (write_status != len) {
        LOG_INFO_("File write failed!\n");
    } else {
        LOG_INFO_("Flash written successfully\n");
        LOG_INFO_("wbuf = %d\n", *data);
    }
  }
  /*---------------------------------------------------------------------------*/
  PROCESS_THREAD(flashwrite_process, ev, data)
  {
    PROCESS_BEGIN();

    static int flash_fd = 1;

    // Open flash to write with read and write flags
    flash_fd = cfs_open("flash.txt", CFS_WRITE|CFS_READ);    
    LOG_INFO_("File descriptor : %d\n", flash_fd);
    // If the file can't be opened fd = -1
    if (flash_fd == -1) {
        LOG_INFO_("File open failed!\n");
        while(1);
    }   

    while(1) {
      PROCESS_YIELD();
      if (ev == PACKET_RECEIVED) {
        // Write to flash
        flash_write(flash_fd, data);
      }
    }

    // Close file
    cfs_close(flash_fd);

    PROCESS_END();    
}
/*---------------------------------------------------------------------------*/
void flash_read(int fd) 
{
  int i = 0, bytes_read = 0, len = 1, eof = 0;
  static char rbuf;

  eof = cfs_seek(fd, 0, CFS_SEEK_END);
  LOG_INFO_("EOF = %d\n", eof);

  while(i < eof) {
    // To read seek to position 
    cfs_seek(fd, i, CFS_SEEK_SET);
    bytes_read = cfs_read(fd, &rbuf, len);
    LOG_INFO_("rbuf = %d, size = %d bytes for i = %d\n", rbuf, bytes_read, i);       
    i++;
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(flashread_process, ev, data)
{
  button_hal_button_t *btn;

  PROCESS_BEGIN();

  btn = button_hal_get_by_index(0);
  static int flash_fd = 1;

  // Open flash to write with read and write flags
  flash_fd = cfs_open("flash.txt", CFS_WRITE|CFS_READ);    
  LOG_INFO_("File descriptor : %d\n", flash_fd);
  // If the file can't be opened fd = -1
  if (flash_fd == -1) {
      LOG_INFO_("File open failed!\n");
      while(1);
  }  

  while(1) {

    PROCESS_YIELD();

    if(ev == button_hal_press_event) {
      btn = (button_hal_button_t *)data;
      LOG_INFO_("Press event (%s)\n", BUTTON_HAL_GET_DESCRIPTION(btn));

      if(btn == button_hal_get_by_id(BUTTON_HAL_ID_BUTTON_ZERO)) {
        LOG_INFO_("This was button 0, on pin %u\n", btn->pin);
        flash_read(flash_fd);
      }
    }    
  }

  // Close file
  cfs_close(flash_fd);

  PROCESS_END();    
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(flashclear_process, ev, data)
{
  button_hal_button_t *btn;

  PROCESS_BEGIN();

  btn = button_hal_get_by_index(0);

  while(1) {
    PROCESS_YIELD();

    if(ev == button_hal_press_event) {
      btn = (button_hal_button_t *)data;
      LOG_INFO_("Press event (%s)\n", BUTTON_HAL_GET_DESCRIPTION(btn));

      if(btn == button_hal_get_by_id(BUTTON_HAL_ID_BUTTON_ONE)) {
        LOG_INFO_("This was button 1, on pin %u\n", btn->pin);
        cfs_remove("flash.txt");
        LOG_INFO_("Flash File removed\n");
      }
    }    
  }

  PROCESS_END();    
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(gpio_process, ev, data)
{
    PROCESS_BEGIN();

    static struct etimer wait_timer;
    etimer_set(&wait_timer, WAIT_INTERVAL);

    while (1)
    {
        // Wait for event from the nullnet receiver
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_MSG);

        etimer_reset(&wait_timer);
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&wait_timer));
        // set GPIO Pin 25 or DP0 as output pin
        GPIO_setOutputEnableDio(25, GPIO_OUTPUT_ENABLE);
        // toggle
        GPIO_clearDio(25);
    }

    PROCESS_END();    
}
/*---------------------------------------------------------------------------*/
