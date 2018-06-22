#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <EEPROM.h>
#include <Arduino.h>

// String MqttBrokerHost;          //35 bytes | start 0, length 35
// String MqttBrokerPort;          //8 bytes  | start 36, length 8
// String MqttCommandTopicBase;    //35 bytes | start 44, length 35
// String MqttStateTopicBase;      //35 bytes | start 79, length 35
// String MqttRetain;              //5 bytes  | start 114, length 5
// String MqttPayloadOn;           //10 bytes | start 119, length 10
// String MqttPayloadOff;          //10 bytes | start 129, length 10
//Total nr of bytes: 133

#define MAX_EEPROM_SIZE 512

#define MqttBrokerHostStartAddr         0
#define MqttBrokerPortStartAddr         36
#define MqttCommandTopicBaseStartAddr   44
#define MqttStateTopicBaseStartAddr     79
#define MqttRetainStartAddr             114
#define MqttPayloadOnStartAddr          119
#define MqttPayloadOffStartAddr         129
#define MqttBrokerHostLength            35
#define MqttBrokerPortLength            8
#define MqttCommandTopicBaseLength      35
#define MqttStateTopicBaseLength        35
#define MqttRetainLength                5
#define MqttPayloadOnLength             10
#define MqttPayloadOffLength            10

class Settings
{
    private:
    String MqttBrokerHost;
    String MqttBrokerPort;
    String MqttCommandTopicBase;
    String MqttStateTopicBase;
    String MqttRetain;
    String MqttPayloadOn;
    String MqttPayloadOff;
    void ReadAllSettingsFromEeprom();

    public:
    Settings();
    String GetMqttBrokerHost();
    void SetMqttBrokerHost(String value);
    void SetMqttBrokerPort(String value);
    String GetMqttBrokerPort();
    void SetMqttCommandTopicBase(String value);
    String GetMqttCommandTopicBase();
    void SetMqttStateTopicBase(String value);
    String GetMqttStateTopicBase();
    void SetMqttRetain(String value);
    String GetMqttRetain();
    void SetMqttPayloadOn(String value);
    String GetMqttPayloadOn();
    void SetMqttPayloadOff(String value);
    String GetMqttPayloadOff();
    
    void Save();
    void EraseAll();
    void DumpToSerial();
};

#endif