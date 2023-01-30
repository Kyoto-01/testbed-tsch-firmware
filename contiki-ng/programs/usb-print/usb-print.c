#include "contiki.h"

#include <stdio.h>

#define PRINT_INTERVAL (2 * CLOCK_SECOND)

PROCESS(usb_print_process, "USB print Process");

AUTOSTART_PROCESSES(&usb_print_process);

PROCESS_THREAD(usb_print_process, ev, data) {
    PROCESS_BEGIN();

    static struct etimer print_timer;
    static char* msg = "My first Contiki-ng program is here!";
    static int counter = 0;

    etimer_set(&print_timer, PRINT_INTERVAL);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&print_timer));

        printf("[%d] %s\n", counter, msg);
        counter++;

        etimer_set(&print_timer, PRINT_INTERVAL);
    }

    PROCESS_END();
}
