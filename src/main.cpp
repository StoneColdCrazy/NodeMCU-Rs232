#include <Arduino.h>
#include "wifi/wifiFunctions.h"
#include "web/espWebServer.h"

/**
 * Presetup
 */
void setup(void)
{
    WifiFunctions::initWifi();
    EspWebServer::initWebServer();
}

/**
 * Loop
 */
void loop(void)
{
    WifiFunctions::handleLoop();
    EspWebServer::handleLoop();
}