#include "settings.h"

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

void start_settings() {
    EEPROM.begin(MAX_EEPROM_SIZE);
}

void saveToEeprom(String val, int startAddr, int maxLength) {
    // Write length of the string in the first 2 bytes of the field, so that we know how long to read when getting the value
    String length = String(val.length());
    if(length.length() < 2) { length = "0" + length; }
    Serial.print("Writing with length: ");
    Serial.println(length);
    EEPROM.write(startAddr, length[0]);
    EEPROM.write(startAddr + 1, length[1]);

    // Then write the actual data
    int addr = startAddr + 2;
    for (int i = 0; i < val.length() && i < maxLength-2; ++i)
    {
        EEPROM.write(addr, val[i]);
        Serial.print("Wrote: ");
        Serial.println(val[i]);
        addr++; 
    }
    EEPROM.commit();
}

String readFromEeprom(int startAddr, int maxLength)
{
    String value;

    //Read length from first 2 bytes of field
    String lengthString;
    lengthString += char(EEPROM.read(startAddr));
    lengthString += char(EEPROM.read(startAddr + 1));
    int lengthToRead = lengthString.toInt();
    Serial.print("Going to read with length: ");
    Serial.println(lengthToRead);

    if(lengthToRead > 0)
    {
        for (int i = startAddr + 2; i < (2 + startAddr + lengthToRead) && (i - startAddr < maxLength - 2); ++i)
        {
            char c = char(EEPROM.read(i));
            value += c;
        }
        Serial.print("Read from EEPROM: ");
        Serial.println(value);
        return value;
    } else {
        String retval = "";
        return retval; //If we don't have anything in EEPROM we'll just return an empty string
    }
}

void eraseAllSettings() {
    for(int i = 0; i < MAX_EEPROM_SIZE; i++ ) {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
}

void saveMqttBrokerHost(String value)
{
    if(value.length() > 0)
    {
        saveToEeprom(value, MqttBrokerHostStartAddr, MqttBrokerHostLength);
    } else {
        Serial.println("Invalid value received for MqttBrokerHost. Cannot save to EEPROM.");
    }
}

String getMqttBrokerHost()
{
    return readFromEeprom(MqttBrokerHostStartAddr, MqttBrokerHostLength);
}

void saveMqttBrokerPort(String value)
{
    if(value.length() > 0)
    {
        saveToEeprom(value, MqttBrokerPortStartAddr, MqttBrokerPortLength);
    } else {
        Serial.println("Invalid value received for MqttBrokerPort. Cannot save to EEPROM.");
    }
}

String getMqttBrokerPort()
{
    return readFromEeprom(MqttBrokerPortStartAddr, MqttBrokerPortLength);
}

void saveMqttCommandTopicBase(String value)
{
    if(value.length() > 0)
    {
        saveToEeprom(value, MqttCommandTopicBaseStartAddr, MqttCommandTopicBaseLength);
    } else {
        Serial.println("Invalid value received for MqttCommandTopicBase. Cannot save to EEPROM.");
    }
}

String getMqttCommandTopicBase()
{
    return readFromEeprom(MqttCommandTopicBaseStartAddr, MqttCommandTopicBaseLength);
}

void saveMqttStateTopicBase(String value)
{
    if(value.length() > 0)
    {
        saveToEeprom(value, MqttStateTopicBaseStartAddr, MqttStateTopicBaseLength);
    } else {
        Serial.println("Invalid value received for MqttStateTopicBase. Cannot save to EEPROM.");
    }
}

String getMqttStateTopicBase()
{
    return readFromEeprom(MqttStateTopicBaseStartAddr, MqttStateTopicBaseLength);
}

void saveMqttRetain(String value)
{
    if(value.length() == 0)
    {
        saveToEeprom("0", MqttRetainStartAddr, MqttRetainLength);
    } else {
        saveToEeprom("1", MqttRetainStartAddr, MqttRetainLength);
    }
}

String getMqttRetain()
{
    return readFromEeprom(MqttRetainStartAddr, MqttRetainLength);
}

void saveMqttPayloadOn(String value)
{
    if(value.length() > 0)
    {
        saveToEeprom(value, MqttPayloadOnStartAddr, MqttPayloadOnLength);
    } else {
        Serial.println("Invalid value received for MqttPayloadOn. Cannot save to EEPROM.");
    }
}

String getMqttPayloadOn()
{
    return readFromEeprom(MqttPayloadOnStartAddr, MqttPayloadOnLength);
}

void saveMqttPayloadOff(String value)
{
    if(value.length() > 0)
    {
        saveToEeprom(value, MqttPayloadOffStartAddr, MqttPayloadOffLength);
    } else {
        Serial.println("Invalid value received for MqttPayloadOff. Cannot save to EEPROM.");
    }
}

String getMqttPayloadOff()
{
    return readFromEeprom(MqttPayloadOffStartAddr, MqttPayloadOffLength);
}
