#include <inttypes.h>
#include <stdint.h>

#include "contiki.h"
#include "dev/radio.h"
#include "net/ipv6/simple-udp.h"
#include "net/mac/tsch/tsch.h"
#include "net/netstack.h"
#include "net/routing/routing.h"
#include "random.h"
#include "sys/log.h"

/*---------------------------------------------------------------------------*/

#define LOG_MODULE "Client"
#define LOG_LEVEL LOG_LEVEL_INFO

#define UDP_CLIENT_PORT 8765
#define UDP_SERVER_PORT 5678

#define SEND_INTERVAL (5 * CLOCK_SECOND)

#ifndef TX_POWER
#define TX_POWER 3
#endif

#define DATALEN 100

/*---------------------------------------------------------------------------*/

static struct simple_udp_connection udp_conn;
static long long tx, rx;

/*---------------------------------------------------------------------------*/

PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);

/*---------------------------------------------------------------------------*/

void print6addr(uip_ipaddr_t *addr6) {
    uint8_t *octets = addr6->u8;
    int i;
    
    for (i = 0; i < 16; ++i) {
        printf("%02x", octets[i]);
    }
}

/*---------------------------------------------------------------------------*/

static void
udp_rx_callback(struct simple_udp_connection *c,
                const uip_ipaddr_t *sender_addr,
                uint16_t sender_port,
                const uip_ipaddr_t *receiver_addr,
                uint16_t receiver_port,
                const uint8_t *data,
                uint16_t datalen) {
    ++rx;
}

/*---------------------------------------------------------------------------*/

void format_infos(char *str) {
    static radio_value_t tx_power;

    NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &tx_power);

    snprintf(str, sizeof(str[0]) * DATALEN, "%llu,%llu,%lu,%lu",
              (uint64_t)tx, (uint64_t)rx, (uint32_t)tx_power, (uint32_t)tsch_current_channel);
}

/*---------------------------------------------------------------------------*/

void send_infos() {
    static char str[DATALEN];
    static uip_ipaddr_t dest_ipaddr;

    if (NETSTACK_ROUTING.node_is_reachable() &&
        NETSTACK_ROUTING.get_root_ipaddr(&dest_ipaddr)) {

        format_infos(str);
        print6addr(&dest_ipaddr);
        printf(",%s\n", str);

        /* send to DAG root */
        simple_udp_sendto(&udp_conn, str, strlen(str), &dest_ipaddr);

        ++tx;
    } else {
        printf("Not reachable yet\n");
    }
}

/*---------------------------------------------------------------------------*/

PROCESS_THREAD(udp_client_process, ev, data) {
    static struct etimer periodic_timer;

    PROCESS_BEGIN();

    /* set transmission power */
    NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, TX_POWER);

    /* initialize UDP connection */
    simple_udp_register(&udp_conn, UDP_CLIENT_PORT, NULL, UDP_SERVER_PORT, udp_rx_callback);

    /* initialize app parameters */
    tx = rx = 0;

    LOG_INFO("Client is running!\n");

    etimer_set(&periodic_timer, SEND_INTERVAL);

    while (1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        send_infos();
        etimer_set(&periodic_timer, SEND_INTERVAL);
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
