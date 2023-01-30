#!/usr/bin/python3

'''
@file       at86rf215-ping-pong.py
@author     Pere Tuset-Peiro  (peretuset@openmote.com) modified by felipefbs
@version    v0.1
@date       February, 2019
@brief

@copyright  Copyright 2019, OpenMote Technologies, S.L.
            This file is licensed under the GNU General Public License v2.
'''

import argparse
import logging
import signal

import serialCollector as sc
import dbController as dbc


finished = False


def signal_handler(sig, frame):
    global finished
    finished = True


# Set-up logging back-end
logging.basicConfig(
    filename="log",
    filemode='a',
    format='%(asctime)s,%(msecs)d %(name)s %(levelname)s %(message)s',
    datefmt='%d-%m-%Y|%H:%M:%S',
    level=logging.DEBUG
)

# Set up SIGINT signal
signal.signal(signal.SIGINT, signal_handler)

# Create argument parser
parser = argparse.ArgumentParser(description="")
parser.add_argument("-p", "--port", type=str, required=True)
parser.add_argument("-b", "--baudrate", type=int, default=115200)
parser.add_argument("-a", "--action", type=str)

# Parse arguments
args = parser.parse_args()

# Create InfluxDB client
dbClient = dbc.UDPServerDBController()

# Optionally, clear the database
if args.action == 'clear':
    dbClient.removeAll()

# Execute collector
collector = sc.UDPServerSerialCollector(args.port, args.baudrate)
collectorData = collector.collector()

print(f'Starting program at port {args.port} with bauds {args.baudrate}.')

while (not finished) and (udpServerData := next(collectorData)):
    try:
        print(
            f"addr: {udpServerData['addr']}",
            f"txCounter: {udpServerData['txCount']}",
            f"rxCounter: {udpServerData['rxCount']}",
            f"txPower: {udpServerData['txPower']}",
            f"channel: {udpServerData['channel']}",
            f"rssi: {udpServerData['rssi']}",
            sep="\n"
        )

        dbClient.insert(udpServerData)

    except Exception as e:
        print(e)
        logging.error(e)

collector.serial.close()
dbClient.client.close()
