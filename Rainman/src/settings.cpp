#include "settings.h"

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
Settings::Settings() {
    EEPROM.begin(MAX_EEPROM_SIZE);
    this->ReadAllSettingsFromEeprom();
}

void Settings::ReadAllSettingsFromEeprom() {
    this->SetMqttBrokerHost(readFromEeprom(MqttBrokerHostStartAddr, MqttBrokerHostLength));
    this->SetMqttBrokerPort(readFromEeprom(MqttBrokerPortStartAddr, MqttBrokerPortLength));
    this->SetMqttCommandTopicBase(readFromEeprom(MqttCommandTopicBaseStartAddr, MqttCommandTopicBaseLength));
    this->SetMqttStateTopicBase(readFromEeprom(MqttStateTopicBaseStartAddr, MqttStateTopicBaseLength));
    this->SetMqttRetain(readFromEeprom(MqttRetainStartAddr, MqttRetainLength));
    this->SetMqttPayloadOn(readFromEeprom(MqttPayloadOnStartAddr, MqttPayloadOnLength));
    this->SetMqttPayloadOff(readFromEeprom(MqttPayloadOffStartAddr, MqttPayloadOffLength));
}

void Settings::EraseAll() {
    for(int i = 0; i < MAX_EEPROM_SIZE; i++ ) {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
    this->ReadAllSettingsFromEeprom();
}

void Settings::Save() {
    saveToEeprom(this->GetMqttBrokerHost(), MqttBrokerHostStartAddr, MqttBrokerHostLength);
    saveToEeprom(this->GetMqttBrokerPort(), MqttBrokerPortStartAddr, MqttBrokerPortLength);
    saveToEeprom(this->GetMqttCommandTopicBase(), MqttCommandTopicBaseStartAddr, MqttCommandTopicBaseLength);
    saveToEeprom(this->GetMqttStateTopicBase(), MqttStateTopicBaseStartAddr, MqttStateTopicBaseLength);
    saveToEeprom(this->GetMqttRetain(), MqttRetainStartAddr, MqttRetainLength);
    saveToEeprom(this->GetMqttPayloadOn(), MqttPayloadOnStartAddr, MqttPayloadOnLength);
    saveToEeprom(this->GetMqttPayloadOff(), MqttPayloadOffStartAddr, MqttPayloadOffLength);
}

void Settings::DumpToSerial() {
    Serial.print("MqttBrokerHost: ");       Serial.println(this->GetMqttBrokerHost());
    Serial.print("MqttBrokerPort: ");       Serial.println(this->GetMqttBrokerPort());
    Serial.print("MqttCommandTopicBase: "); Serial.println(this->GetMqttCommandTopicBase());
    Serial.print("MqttStateTopicBase: ");   Serial.println(this->GetMqttStateTopicBase());
    Serial.print("MqttRetain: ");           Serial.println(this->GetMqttRetain());
    Serial.print("MqttPayloadOn: ");        Serial.println(this->GetMqttPayloadOn());
    Serial.print("MqttPayloadOff: ");       Serial.println(this->GetMqttPayloadOff());
}

void Settings::SetMqttBrokerHost(String value) {
    this->MqttBrokerHost = value;
}

String Settings::GetMqttBrokerHost() {
    return this->MqttBrokerHost;
}

void Settings::SetMqttBrokerPort(String value) {
    this->MqttBrokerPort = value;
}
String Settings::GetMqttBrokerPort() {
    return this->MqttBrokerPort;
}
void Settings::SetMqttCommandTopicBase(String value) {
    this->MqttCommandTopicBase = value;
}
String Settings::GetMqttCommandTopicBase() {
    return this->MqttCommandTopicBase;
}
void Settings::SetMqttStateTopicBase(String value) {
    this->MqttStateTopicBase = value;
}
String Settings::GetMqttStateTopicBase() {
    return this->MqttStateTopicBase;
}
void Settings::SetMqttRetain(String value) {
    this->MqttRetain = (value.length() > 0 ? "1" : "0");
}
String Settings::GetMqttRetain() {
    return this->MqttRetain;
}
void Settings::SetMqttPayloadOn(String value) {
    this->MqttPayloadOn = value;
}
String Settings::GetMqttPayloadOn() {
    return this->MqttPayloadOn;
}
void Settings::SetMqttPayloadOff(String value) {
    this->MqttPayloadOff = value;
}
String Settings::GetMqttPayloadOff() {
    return this->MqttPayloadOff;
}
