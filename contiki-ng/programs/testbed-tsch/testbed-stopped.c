#include "contiki.h"
#include "sys/log.h"

#define LOG_MODULE (uint8_t *)("testbed stopped")
#define LOG_LEVEL LOG_LEVEL_INFO

PROCESS(testbed_stopped_process, "testbed stopped");

AUTOSTART_PROCESSES(&testbed_stopped_process);

PROCESS_THREAD(testbed_stopped_process, ev, data) {

    PROCESS_BEGIN();

    LOG_INFO("Stopped Mote.\n");

    PROCESS_END();
}
