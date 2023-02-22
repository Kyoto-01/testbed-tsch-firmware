/*
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

#ifndef STDLIB_H
#include <stdlib.h>
#endif

#include "contiki.h"
#include "dev/radio.h"
#include "net/ipv6/simple-udp.h"
#include "net/ipv6/uip-ds6.h"
#include "net/netstack.h"
#include "net/routing/routing.h"
#include "sys/log.h"
#include "utils/constants.h"
#include "utils/utils.h"

#define FIRMWARE_TYPE (uint8_t *)("server")

#define LOG_MODULE FIRMWARE_TYPE
#define LOG_LEVEL LOG_LEVEL_INFO

struct serial_data {
    uint8_t *firmtype;
    uint8_t *addrsend;
    uint8_t *addrrecv;
    uint8_t *data;
    int32_t rssi;
};

static struct simple_udp_connection udp_conn;
static struct serial_data sdata;

static void sendto_serial(struct serial_data *data) {
    printf(
        "%s,%s,%s,%s,%ld\n",
        data->firmtype,
        data->addrsend,
        data->addrrecv,
        data->data,
        data->rssi);
}

static void get_rssi(int32_t *output) {
    static radio_value_t *rssi;

    NETSTACK_RADIO.get_value(RADIO_PARAM_LAST_RSSI, rssi);

    *output = (int32_t)rssi;
}

static void get_addr6(uip_ipaddr_t *output) {
    if (!output->u8[0]) {
        *output = uip_ds6_get_link_local(-1)->ipaddr;
        if (!output->u8[0]) {
            return;
        }
    }
}

static uint8_t *ipaddr_to_str(uip_ipaddr_t *addr) {
    if (addr->u8[0]) {
        return ascii_to_hex(addr->u8, 16);
    } else {
        return (uint8_t *)'\0';
    }
}

static void recv_data(uip_ipaddr_t *sender_addr, uint8_t *data) {
    get_rssi(&sdata.rssi);

    free(sdata.addrsend);
    sdata.addrsend = ipaddr_to_str(sender_addr);

    sdata.data = data;
}

static void udp_rx_callback(
    struct simple_udp_connection *c,
    const uip_ipaddr_t *sender_addr,
    uint16_t sender_port,
    const uip_ipaddr_t *receiver_addr,
    uint16_t receiver_port,
    const uint8_t *data,
    uint16_t datalen) {
    recv_data((uip_ipaddr_t *)sender_addr, (uint8_t *)data);

    simple_udp_sendto(&udp_conn, data, datalen, sender_addr);

    sendto_serial(&sdata);
}

PROCESS(testbed_server_process, "testbed server");

AUTOSTART_PROCESSES(&testbed_server_process);

PROCESS_THREAD(testbed_server_process, ev, data) {
    static uip_ipaddr_t recv_addr;

    PROCESS_BEGIN();

    /* Setting static data */

    // Firmware type (server or client)
    sdata.firmtype = FIRMWARE_TYPE;

    // Mote address
    get_addr6(&recv_addr);
    sdata.addrrecv = ipaddr_to_str(&recv_addr);

    /* Set transmission power */
    NETSTACK_RADIO.set_value(RADIO_PARAM_TXPOWER, TX_POWER);

    /* Initialize DAG root */
    NETSTACK_ROUTING.root_start();

    /* Initialize UDP connection */
    simple_udp_register(
        &udp_conn,
        UDP_SERVER_PORT,
        NULL,
        UDP_CLIENT_PORT,
        udp_rx_callback);

    LOG_INFO("TSCH testbed server is running!\n");

    PROCESS_END();
}
