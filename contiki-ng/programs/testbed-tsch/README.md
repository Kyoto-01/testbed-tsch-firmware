# rpl-udp-network

A simple RPL network with UDP communication where clients share their information with the server:
it includes a DAG root (`udp-server.c`) and DAG nodes (`udp-clients.c`).
This example runs without a border router -- this is a stand-alone RPL network.

The DAG root also acts as UDP server. The DAG nodes are UDP client. Clients periodically send a UDP request, which simply includes a text with their information. The server then acknowledges the request, echoing its contents to the client.

## build.sh

Script to build Contiki-NG 6TiSCH client/server firmwares.


```
build.sh [-f fimware_type] [-p tx_power] [-s server_usb] [-l hop_sequence_len] [-h hop_sequence]
```

## Visualização do tráfego da porta serial

```
make TARGET=openmote BOARD=openmote-b PORT=<usb_device_name> serialview
```