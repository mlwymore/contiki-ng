#include "contiki.h"
//#include "arch/platform/cc26x0-cc13x0/contiki-conf.h"
//#include "arch/platform/cc26x0-cc13x0/cfs-coffee-arch.h"
#include "os/storage/cfs/cfs-coffee.h"
// #include "arch/cpu/cc26x0-cc13x0/lib/cc26xxware/driverlib/flash.h"
// #include "driverlib/flash.h"
#include <string.h>
#include <stdio.h> /* For printf() */

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Configuration */
#define SEND_INTERVAL (1 * CLOCK_SECOND)

/*---------------------------------------------------------------------------*/
PROCESS(flash_process, "Flash read write example");
AUTOSTART_PROCESSES(&flash_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(flash_process, ev, data)
{  
  // static uint8_t write_data[] = {0xA, 1, 2};
  // uint8_t *data_ptr = write_data;
  static struct etimer periodic_timer;

  PROCESS_BEGIN();
  
  etimer_set(&periodic_timer, SEND_INTERVAL);

  int test_fd = 1;
  test_fd = cfs_open("blah.txt",CFS_WRITE|CFS_READ);
  LOG_INFO_("File descriptor: %d\n",test_fd);
  if (test_fd == -1) {
    LOG_INFO_("File open failed!\n");
    while(1);
  }
  int i = 0;
  char len = 1;
  char wbuf,rbuf;

  while(1) {
    wbuf = i;
      if (cfs_write(test_fd,(void*)&wbuf,len) != len) {
        LOG_INFO_("File write failed!\n");
      } 
      cfs_seek(test_fd,i,CFS_SEEK_SET);
      if (cfs_read(test_fd,&rbuf,len) != len) {
        LOG_INFO_("File read failed!\n");
      } else if (rbuf != i) {
        LOG_INFO_("File read not correct, read: %d and expected: %d\n", rbuf, i);
      } else {
        LOG_INFO_("Read successful\n");
      }
     
      i++;

  }

  cfs_close(test_fd);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
