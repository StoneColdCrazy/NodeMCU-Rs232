#ifndef __PARAMS_H__
#define __PARAMS_H__

/**
 * Host data
 */
#define HOSTNAME "ESP-" 
#define VERSION "0.1"
#define WEBPORT 80
#define ARDUINO_OTA_PORT 8266 // Default is 8266
#define ARDUINO_OTA_AUTH_ENABLE false
#define ARDUINO_OTA_AUTH_PASS "admin"

/** 
 * Debug Serial
 */
//#define DEBUG_SERIAL_ENABLE
#define dbgSerial Serial

/**
 * Configure DEBUG serial commands
 */
#ifdef DEBUG_SERIAL_ENABLE
#define dbgSerialPrint(a)    dbgSerial.print(a)
#define dbgSerialPrintln(a)  dbgSerial.println(a)
#else
#define dbgSerialPrint(a)    do{}while(0)
#define dbgSerialPrintln(a)  do{}while(0)
#endif

#endif /* #ifndef __PARAMS_H__ */