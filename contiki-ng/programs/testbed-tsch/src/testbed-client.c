#include <inttypes.h>
#include <stdint.h>

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

    output = (uint32_t *)txpwr;
}

static void get_ch(uint32_t *output) {
    *output = (uint32_t)tsch_current_channel;
}

static void get_addr6(uip_ipaddr_t *output) {
    static uip_ipaddr_t addr6;

    if (!addr6.u8[0]) {
        addr6 = uip_ds6_get_global(-1)->ipaddr;
        if (!addr6.u8[0])
            return;
    }

    output = &addr6;
}

static uint32_t get_root_addr6(uip_ipaddr_t *output) {
    static uip_ipaddr_t addr6;
    static uint32_t reachable;

    reachable = NETSTACK_ROUTING.get_root_ipaddr(&addr6);

    output = &addr6;

    return reachable;
}

static void ipaddr_to_str(uip_ipaddr_t *addr, uint8_t *output) {
    if (addr->u8[0]) {
        ascii_to_hex((char *)addr->u8, (char *)output);
    } else {
        output = (uint8_t *)("");
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

    printf("%s", output);
}

static void send_data() {
    static uip_ipaddr_t src_ipaddr;
    static uip_ipaddr_t dest_ipaddr;

    get_txpwr(&ndata.txpwr);
    get_ch(&ndata.ch);

    get_addr6(&src_ipaddr);
    ipaddr_to_str(&src_ipaddr, sdata.addrsend);

    if (
        NETSTACK_ROUTING.node_is_reachable() &&
        get_root_addr6(&dest_ipaddr)) {
        ipaddr_to_str(&dest_ipaddr, sdata.addrrecv);

        build_data(&ndata, sdata.data);

        /* send to DAG root */
        simple_udp_sendto(
            &udp_conn,
            sdata.data,
            strlen((char *)sdata.data),
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

    PROCESS_BEGIN();

    sdata.firmtype = FIRMWARE_TYPE;

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