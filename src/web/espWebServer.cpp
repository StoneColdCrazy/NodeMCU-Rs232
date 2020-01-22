#include "espWebServer.h"
#include <params.h>
#include <Updater.h>
#include <WiFiClientSecure.h>
#include "mainPageData.h"
#include "uploadPage.h"
#include "serialmonitorPage.h"

ESP8266WebServer EspWebServer::webServer(WEBPORT);
String EspWebServer::recvMessageBuffer = "";
String EspWebServer::sendMessageBuffer = "";
int EspWebServer::selectedUartRxPin = 13;
int EspWebServer::selectedUartTxPin = 15;
int EspWebServer::selectedUartBaudrate = 115200;
bool EspWebServer::selectedUartConnected = false;
SerialUart EspWebServer::uartConnection;
byte recvBuffer[1025];

/**
 * Intialize webserver
 */
void EspWebServer::initWebServer()
{
    webServer.on("/", HTTP_GET, showSerialMonitor);
    webServer.on("/monitor", HTTP_GET, showSerialMonitor);
    webServer.on("/uartrx", HTTP_GET, getLastMessages);
    webServer.on("/uarttx", HTTP_POST, sendMessage);
    webServer.on("/uartconnect", HTTP_GET, uartConnect);
    webServer.on("/uartdisconnect", HTTP_GET, uartDisconnect);
    webServer.on("/upload", HTTP_GET, showUploader);
    webServer.on("/update", HTTP_POST, postUpdate, onRecvUploadFile);
    webServer.onNotFound(redirectHome);
    webServer.begin();
}

/**
 * Intialize webserver calls
 */
void EspWebServer::handleLoop()
{
    webServer.handleClient();

    // Active connection?!
    if (!selectedUartConnected) {
        return;
    }

    // UART Send
    String sendData = getSendMessageBuffer();
    if (sendData.length() > 0) {
        uartConnection.sendData((byte*)(sendData.c_str()), sendData.length());
        addNewRecvMessage((byte*)(sendData.c_str()), sendData.length());
    }

    // UART Recv
    uint32_t recvBytes = uartConnection.receiveData(recvBuffer, 1024, 10);
    if (recvBytes > 0) {
        addNewRecvMessage(recvBuffer, recvBytes);
    }
}

/**
 * Redirect response to web client
 */
void EspWebServer::redirectHome() 
{
    dbgSerialPrintln("Redirect home");
    webServer.sendHeader("Location", String("/"), true);
    webServer.sendHeader("Cache-Control", "no-cache, no-store");
    webServer.sendHeader("Pragma", "no-cache");
    webServer.sendHeader("Expires", "-1");
    webServer.send(302, "text/plain", "");
    webServer.client().stop();
}

/**
 * Show bin uploader
 */
void EspWebServer::showUploader()
{
    String hostname(HOSTNAME);
    hostname += String(ESP.getChipId(), HEX);

    String targetOta = "";
    targetOta += "HostName: " + hostname + "<br>";
    targetOta += "IP: " + WiFi.localIP().toString() + "<br>";
    targetOta += "Port: " + String(ARDUINO_OTA_PORT) + "<br>";

    String webPage = PAGE_HEADER + 
        PAGE_NAVIGATION +
        PAGE_BODY_START +
        WEB_UPLOAD_PAGE +
        PAGE_BODY_SCRIPTS +
        WEB_UPLOAD_PAGE_SCRIPT +
        PAGE_FOOTER;

    webPage.replace("%target%", targetOta);
    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/html", webPage);
}

/**
 * Show serial monitor
 */
void EspWebServer::showSerialMonitor()
{
    String webPage = PAGE_HEADER + 
        PAGE_NAVIGATION +
        PAGE_BODY_START +
        WEB_SERIALMONITOR_PAGE +
        PAGE_BODY_SCRIPTS +
        WEB_SERIALMONITOR_PAGE_SCRIPT +
        PAGE_FOOTER;

    if ((selectedUartRxPin == 1) && (selectedUartTxPin == 3)) {
        webPage.replace("###SERIAL###", "0");
    } else if ((selectedUartRxPin == 13) && (selectedUartTxPin == 15)) {
        webPage.replace("###SERIAL###", "1");
    } else {
        webPage.replace("###SERIAL###", "2");
    }
    
    webPage.replace("###BAUD###", String(selectedUartBaudrate));
    webPage.replace("###RXPIN###", String(selectedUartRxPin));
    webPage.replace("###TXPIN###", String(selectedUartTxPin));
    if (selectedUartConnected) {
        webPage.replace("###CONNECTED###", "true");
    } else {
        webPage.replace("###CONNECTED###", "false");
    }
    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/html", webPage);
}

/**
 * When posting new binary
 */
void EspWebServer::postUpdate()
{
    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/plain", ( Update.hasError() ) ? "FAIL" : "OK");
    //esp_wifi_wps_disable();
    ESP.restart();
}

/**
 * When recving uploaded binary
 */
void EspWebServer::onRecvUploadFile()
{
    HTTPUpload& upload = webServer.upload();
    if(upload.status == UPLOAD_FILE_START) {
        dbgSerialPrintln("Update: " + String(upload.filename.c_str()));
        if(!Update.begin(-1)) { //start with max available size
            Update.printError(dbgSerial);
        }
    }
    else if(upload.status == UPLOAD_FILE_WRITE) {
        /* flashing firmware to ESP*/
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
            Update.printError(dbgSerial);
        }
    }
    else if(upload.status == UPLOAD_FILE_END) {
        if(Update.end(true)){ //true to set the size to the current progress
            dbgSerialPrintln("Update Success: " + String(upload.totalSize));
            dbgSerialPrintln("Rebooting...");
        } else {
            Update.printError(dbgSerial);
        }
    }
}

/**
 * Return UART last messages in recv
 */
void EspWebServer::getLastMessages()
{
    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/plain", recvMessageBuffer);
    recvMessageBuffer = "";
}

/**
 * Add new data recived
 * 
 * @param buffer 
 * @param len 
 */
void EspWebServer::addNewRecvMessage(byte *buffer, uint32_t len)
{
    buffer[len] = 0;
    recvMessageBuffer += String((char*)buffer);
}

/**
 * Send data to uart
 */
void EspWebServer::sendMessage()
{
    String data = webServer.arg("senddata");
    sendMessageBuffer += data + "\r\n";
    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/plain", "");
}

/**
 * Returns inner send buffer data
 * 
 * @return String 
 */
String EspWebServer::getSendMessageBuffer()
{
    String returnData = sendMessageBuffer;
    sendMessageBuffer = "";
    return returnData;
}

/**
 * Connect UART
 */
void EspWebServer::uartConnect()
{
    String targetSerial = webServer.arg("serial");
    String targetBaudrate = webServer.arg("baudrate");
    String targetRxPin = webServer.arg("rxpin");
    String targetTxPin = webServer.arg("txpin");

    if ((targetSerial.length() <= 0) ||
        (targetBaudrate.length() <= 0) ||
        (targetRxPin.length() <= 0) ||
        (targetTxPin.length() <= 0)
    ) {
        webServer.sendHeader("Connection", "close");
        webServer.send(400, "text/plain", "");
        return;
    }

    if (selectedUartConnected) {
        uartConnection.disconnect();
        selectedUartConnected = false;
    }

    selectedUartBaudrate = targetBaudrate.toInt();
    selectedUartRxPin = targetRxPin.toInt();
    selectedUartTxPin = targetTxPin.toInt();
    int targetSerialSelected = targetSerial.toInt();
    if (targetSerialSelected == 0) {
        selectedUartRxPin = 1;
        selectedUartTxPin = 3;
    } else if (targetSerialSelected == 0) {
        selectedUartRxPin = 13;
        selectedUartTxPin = 15;
    } else if (targetSerialSelected == 0) {
        bool isValidRxPin = (selectedUartRxPin >= 0 && selectedUartRxPin <= 5) || (selectedUartRxPin >= 12 && selectedUartRxPin <= 15);
        bool isValidTxPin = (selectedUartTxPin >= 0 && selectedUartTxPin <= 5) || (selectedUartTxPin >= 12 && selectedUartTxPin <= 15);
        if (!isValidRxPin || !isValidTxPin) {
            webServer.sendHeader("Connection", "close");
            webServer.send(400, "text/plain", "");
            return;
        }
    }

    uartConnection.connect(selectedUartRxPin, selectedUartTxPin, selectedUartBaudrate);
    selectedUartConnected = true;
    recvMessageBuffer = "";
    sendMessageBuffer = "";

    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/plain", "");
}

/**
 * Disconnect UART
 */
void EspWebServer::uartDisconnect()
{
    uartConnection.disconnect();
    selectedUartConnected = false;
    recvMessageBuffer = "";
    sendMessageBuffer = "";
    webServer.sendHeader("Connection", "close");
    webServer.send(200, "text/plain", "");
}