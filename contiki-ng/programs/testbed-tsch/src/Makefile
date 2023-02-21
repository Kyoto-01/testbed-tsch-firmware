CONTIKI_PROJECT = testbed-client testbed-server
all: $(CONTIKI_PROJECT)

CONTIKI=../../..
MODULES += os/services/shell
MAKE_MAC = MAKE_MAC_TSCH

TX_POWER = 3

CFLAGS += -DWITH_TSCH=1
CFLAGS += -DTSCH_CONF_AUTOSTART=1
CFLAGS += -DTX_POWER=$(TX_POWER)

include $(CONTIKI)/Makefile.include
