#ifndef __WIFI_FUNCTIONS_H__
#define __WIFI_FUNCTIONS_H__
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>

/**
 * Functions to handle the complete wifi initialize
 */
class WifiFunctions
{
private:  
    static void wifiManagerConfigModeCallback(WiFiManager *myWiFiManager);   
    static void onStartArduinoOTA();
    static void onEndArduinoOTA();
    static void onProgressArduinoOTA(unsigned int progress, unsigned int total);
    static void onErrorArduinoOTA(ota_error_t error);

public:
    static void initWifi();
    static void initArduinoOta();
    static void handleLoop();
};

#endif /* #ifndef __WIFI_FUNCTIONS_H__ */