#ifndef __SERIAL_UART_H__
#define __SERIAL_UART_H__
#include <Arduino.h>
#include <SoftwareSerial.h>

/**
 * Functions to open an serial port
 */
class SerialUart
{
private:
    enum serialTypes {
        INVALID = -1,
        SERIAL1 = 0,
        SERIAL2 = 1,
        SOFTSERIAL = 2
    };
    serialTypes serialToUse;
    bool hardwareSerialSwaped = false;
    SoftwareSerial *softSerial;

public:
    SerialUart();
    void connect(int8_t rxdPin, int8_t txdPin, uint32_t baudrate);
    void disconnect();

    uint32_t receiveData(byte *buffer, size_t bufferLen, uint32_t timeout);
    bool sendData(byte *buffer, size_t bufferLen);
};

#endif /* __SERIAL_UART_H__ */