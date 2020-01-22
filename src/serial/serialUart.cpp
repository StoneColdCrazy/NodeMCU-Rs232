#include "serialUart.h"


SerialUart::SerialUart()
{
    softSerial = NULL;
    serialToUse = serialTypes::INVALID;
}

void SerialUart::connect(int8_t rxdPin, int8_t txdPin, uint32_t baudrate)
{
    // Close all open ports
    disconnect();

    // Start new
    if ((txdPin == 3) || (rxdPin == 1)) {
        Serial.begin(baudrate);
        serialToUse = serialTypes::SERIAL1;
        if (hardwareSerialSwaped) {
            Serial.swap();
            hardwareSerialSwaped = false;
        }
    } else if ((txdPin == 15) || (rxdPin == 13)) {
        Serial.begin(baudrate);
        serialToUse = serialTypes::SERIAL2;
        if (!hardwareSerialSwaped) {
            Serial.swap();
            hardwareSerialSwaped = true;
        }
        
    } else {
        serialToUse = serialTypes::SOFTSERIAL;
        softSerial = new SoftwareSerial(rxdPin, txdPin);
        softSerial->begin(baudrate);
        softSerial->enableRx(true);
        softSerial->enableTx(true);
    }
}

void SerialUart::disconnect()
{
    if (softSerial != NULL) {
        softSerial->stopListening();
        softSerial->end();
        free(softSerial);
        softSerial = NULL;
    }
    Serial.end();
    serialToUse = serialTypes::INVALID;
}

uint32_t SerialUart::receiveData(byte *buffer, size_t bufferLen, uint32_t timeout)
{
    if ((serialToUse == serialTypes::SERIAL1) || (serialToUse == serialTypes::SERIAL2)) {
        Serial.setTimeout(timeout);
        return Serial.readBytes(buffer, bufferLen);
    } else if ((serialToUse == serialTypes::SOFTSERIAL) && (softSerial != NULL)) {
        softSerial->listen();
        softSerial->setTimeout(timeout);
        return softSerial->readBytes(buffer, bufferLen);
    }
    return -1;
}

bool SerialUart::sendData(byte *buffer, size_t bufferLen)
{
    if ((serialToUse == serialTypes::SERIAL1) || (serialToUse == serialTypes::SERIAL2)) {
        Serial.write(buffer, bufferLen);
        return true;
    } else if ((serialToUse == serialTypes::SOFTSERIAL) && (softSerial != NULL)) {
        softSerial->write(buffer, bufferLen);
        return true;
    }
    return false;
}

