#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "webserver.h"
#include "screen.h"
#include "settings.h"

const char* ssid     = "KInternet";
const char* password = "+RSWKCpvCm\\j6`4";

Settings settings;

void setup() {
    Serial.begin(115200);
    pinMode(D0, OUTPUT);
    pinMode(D1, OUTPUT);
    pinMode(D2, OUTPUT);
    pinMode(D3, OUTPUT);
    pinMode(D4, OUTPUT);
    pinMode(D5, OUTPUT);

    start_screen();

    /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
    would try to act as both a client and an access-point and could cause
    network-issues with your other WiFi-devices on your WiFi-network. */

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    start_webserver(settings);
}

void loop() {
    handle_webserver();
    yield();
    handle_screen();
    yield();
}