#include <inttypes.h>
#include <stdint.h>

#ifndef STDLIB_H
#include <stdlib.h>
#endif

#include "contiki.h"
#include "dev/radio.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uip-ds6.h"
#include "net/mac/tsch/tsch.h"
#include "net/netstack.h"
#include "net/routing/routing.h"
#include "random.h"
#include "sys/log.h"
#include "utils/constants.h"
#include "utils/utils.h"

#define FIRMWARE_TYPE (uint8_t *)("client")

#define SEND_INTERVAL (5 * CLOCK_SECOND)
#define DATALEN 100

#define LOG_MODULE FIRMWARE_TYPE
#define LOG_LEVEL LOG_LEVEL_INFO

struct serial_data {
    uint8_t *firmtype;
    uint8_t *addrsend;
    uint8_t *addrrecv;
    uint8_t *data;
};

struct net_data {
    uint64_t tx;
    uint64_t rx;
    uint32_t txpwr;
    uint32_t ch;
};

static struct simple_udp_connection udp_conn;

static struct serial_data sdata;

static struct net_data ndata = {.tx = 0, .rx = 0};

static void sendto_serial(struct serial_data *data) {
    printf(
        "%s,%s,%s,%s\n",
        data->firmtype,
        data->addrsend,
        data->addrrecv,
        data->data);
}

static void get_txpwr(uint32_t *output) {
    static radio_value_t txpwr;

    NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &txpwr);

    *output = (uint32_t)txpwr;
}

static void get_ch(uint32_t *output) {
    *output = (uint32_t)tsch_current_channel;
}

static void get_addr6(uip_ipaddr_t *output) {
    if (!output->u8[0]) {
        *output = uip_ds6_get_global(-1)->ipaddr;
        if (!output->u8[0]) {
            *output = uip_ds6_get_link_local(-1)->ipaddr;
            if (!output->u8[0]) {
                return;
            }
        }
    }
}

static uint32_t get_root_addr6(uip_ipaddr_t *output) {
    return NETSTACK_ROUTING.get_root_ipaddr(output);
}

static uint8_t *ipaddr_to_str(uip_ipaddr_t *addr) {
    if (addr->u8[0]) {
        return ascii_to_hex(addr->u8, 16);
    } else {
        return (uint8_t *)'\0';
    }
}

static void build_data(struct net_data *data, uint8_t *output) {
    snprintf(
        (char *)output, sizeof(char*) * DATALEN,
        "%llu,%llu,%lu,%lu",
        data->tx,
        data->rx,
        data->txpwr,
        data->ch);
}

static void send_data() {
    static uip_ipaddr_t dest_ipaddr;
    static uint8_t data[DATALEN];

    get_txpwr(&ndata.txpwr);
    get_ch(&ndata.ch);

    if (
        NETSTACK_ROUTING.node_is_reachable() &&
        get_root_addr6(&dest_ipaddr)) {
        free(sdata.addrrecv);
        sdata.addrrecv = ipaddr_to_str(&dest_ipaddr);

        build_data(&ndata, data);
        sdata.data = data;

        /* send to DAG root */
        simple_udp_sendto(
            &udp_conn,
            data,
            strlen((char *)data),
            &dest_ipaddr);

        sendto_serial(&sdata);

        ++ndata.tx;
    } else {
        LOG_INFO("Not reachable yet\n");
    }
}

static void udp_rx_callback(
    struct simple_udp_connection *c,
    const uip_ipaddr_t *sender_addr,
    uint16_t sender_port,
    const uip_ipaddr_t *receiver_addr,
    uint16_t receiver_port,
    const uint8_t *data,
    uint16_t datalen) {
    ++ndata.rx;
}

PROCESS(testbed_client_process, "testbed client");

AUTOSTART_PROCESSES(&testbed_client_process);

PROCESS_THREAD(testbed_client_process, ev, data) {
    static struct etimer periodic_timer;
    static uip_ipaddr_t ipaddr;

    PROCESS_BEGIN();

    /* Setting static data */

    // Firmware type (server or client)
    sdata.firmtype = FIRMWARE_TYPE;

    // Mote address
    get_addr6(&ipaddr);
    sdata.addrsend = ipaddr_to_str(&ipaddr);

    /* set transmission power */
    NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, TX_POWER);

    /* initialize UDP connection */
    simple_udp_register(
        &udp_conn,
        UDP_CLIENT_PORT,
        NULL,
        UDP_SERVER_PORT,
        udp_rx_callback);

    LOG_INFO("TSCH testbed client is running!\n");

    etimer_set(&periodic_timer, SEND_INTERVAL);

    while (1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
        send_data();
        etimer_set(&periodic_timer, SEND_INTERVAL);
    }

    PROCESS_END();
}