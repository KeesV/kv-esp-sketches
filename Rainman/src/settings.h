#include <EEPROM.h>
#include <Arduino.h>

void start_settings();

void saveMqttBrokerHost(String value);
String getMqttBrokerHost();
void saveMqttBrokerPort(String value);
String getMqttBrokerPort();
void saveMqttCommandTopicBase(String value);
String getMqttCommandTopicBase();
void saveMqttStateTopicBase(String value);
String getMqttStateTopicBase();
void saveMqttRetain(String value);
String getMqttRetain();
void saveMqttPayloadOn(String value);
String getMqttPayloadOn();
void saveMqttPayloadOff(String value);
String getMqttPayloadOff();

void eraseAllSettings();