#ifndef __ESP_WEBSERVER_H__
#define __ESP_WEBSERVER_H__
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "../serial/serialUart.h"

/**
 * Functions to handle the complete web update
 */
class EspWebServer
{
private:
    static String recvMessageBuffer;
    static String sendMessageBuffer;
    static int selectedUartRxPin;
    static int selectedUartTxPin;
    static int selectedUartBaudrate;
    static bool selectedUartConnected;
    static SerialUart uartConnection;

    static ESP8266WebServer webServer;
    static void redirectHome();
    static void showUploader();
    static void showSerialMonitor();
    static void postUpdate();
    static void onRecvUploadFile();
    static void getLastMessages();
    static void sendMessage();
    static void uartConnect();
    static void uartDisconnect();

    static void addNewRecvMessage(byte *buffer, uint32_t len);
    static String getSendMessageBuffer();
public:
    static void initWebServer();
    static void handleLoop();
   
};

#endif /* #ifndef __ESP_WEBSERVER_H__ */