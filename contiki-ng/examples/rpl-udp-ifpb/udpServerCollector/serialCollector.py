from serial import Serial


class UDPServerSerialCollector:

    def __init__(self, port, baudrate, timeout=1):
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.serial = Serial(port=port, baudrate=baudrate, timeout=timeout)

    def __collect_last(self):
        message = self.serial.readlines(self.timeout)

        if message:
            try:
                message = message[0].decode()
                addr, txCount, rxCount, txPower, channel, rssi = message.split(',')

                return {
                    'addr': addr,
                    'txCount': txCount,
                    'rxCount': rxCount,
                    'txPower': txPower,
                    'channel': channel,
                    'rssi': rssi,
                }
            
            except Exception as e:
                return self.__collect_last()

    def collector(self):
        while True:
            data = self.__collect_last() 
            if data:
                yield data
