#include "wifiFunctions.h"
#include <params.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>


/**
 * Intialize WiFi with manager
 */
void WifiFunctions::initWifi()
{
    WiFiManager wifiManager;
    #ifndef DEBUG_SERIAL_ENABLE
    wifiManager.setDebugOutput(false);
    #endif
    wifiManager.setAPCallback(wifiManagerConfigModeCallback);   
    wifiManager.setConnectTimeout(30);

    String hostname(HOSTNAME);
    hostname += String(ESP.getChipId(), HEX);
    dbgSerialPrintln("Try autoconnect WiFi");

    if (!wifiManager.autoConnect((const char *)hostname.c_str())) {
        delay(3000);
        WiFi.disconnect(true);
        ESP.reset();
        delay(5000);
    }

    // start mdns
    if (!MDNS.begin(hostname)) {
        dbgSerialPrintln("Error setting up MDNS responder!");
        while(1) {
            delay(1000);
        }
    }
    dbgSerialPrintln("mDNS responder started");
    dbgSerialPrintln("WiFi autoconnect success");
    dbgSerialPrint("Connected with IP: ");
    dbgSerialPrintln(WiFi.localIP());
}

/**
 * Initialize ArduinoOTA updater
 */
void WifiFunctions::initArduinoOta()
{
    String hostname(HOSTNAME);
    hostname += String(ESP.getChipId(), HEX);
    ArduinoOTA.setHostname(hostname.c_str());
    ArduinoOTA.setPort(ARDUINO_OTA_PORT);
    if (ARDUINO_OTA_AUTH_ENABLE) {
        ArduinoOTA.setPassword(ARDUINO_OTA_AUTH_PASS); 
    }

    ArduinoOTA.onStart(onStartArduinoOTA);
    ArduinoOTA.onEnd(onEndArduinoOTA);
    ArduinoOTA.onProgress(onProgressArduinoOTA);
    ArduinoOTA.onError(onErrorArduinoOTA);
    ArduinoOTA.begin();
}

/**
 * Handle loop requests
 */
void WifiFunctions::handleLoop()
{
	MDNS.update();
    ArduinoOTA.handle();
}

/**
 * We entered configuration mode
 */
void WifiFunctions::wifiManagerConfigModeCallback (WiFiManager *myWiFiManager)
{
    dbgSerialPrintln("Entered config mode");
    dbgSerialPrint("Listen on accesspoint and IP: ");
    dbgSerialPrintln(WiFi.softAPIP());
}

/**
 * Start the OTA update
 */
void WifiFunctions::onStartArduinoOTA()
{
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
    } else {
        // SPIFFS
        type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    dbgSerialPrintln("Start updating " + type);
}

/**
 * End the OTA update
 */
void WifiFunctions::onEndArduinoOTA()
{
    dbgSerialPrintln("\nEnd");
}

/**
 * Progress the OTA update
 */
void WifiFunctions::onProgressArduinoOTA(unsigned int progress, unsigned int total)
{
    dbgSerialPrint("Progress: " + String(progress / (total / 100)) + "\r");
}

/**
 * Error while executing OTA update
 */
void WifiFunctions::onErrorArduinoOTA(ota_error_t error)
{
    dbgSerialPrint("Error: ");
    if (error == OTA_AUTH_ERROR) {
        dbgSerialPrintln("Auth Failed");
    }
    else if (error == OTA_BEGIN_ERROR) {
        dbgSerialPrintln("Begin Failed");
    }
    else if (error == OTA_CONNECT_ERROR) {
        dbgSerialPrintln("Connect Failed");
    }
    else if (error == OTA_RECEIVE_ERROR) {
        dbgSerialPrintln("Receive Failed");
    }
    else if (error == OTA_END_ERROR) {
        dbgSerialPrintln("End Failed");
    }
}
