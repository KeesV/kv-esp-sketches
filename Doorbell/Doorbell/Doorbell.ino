#include <FS.h>                   //this needs to be first, or it all crashes and burns...

#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

#define PIN_RELAY D1
#define PIN_SWITCH D4

WiFiManager wifiManager;
WiFiClient espClient;
PubSubClient mqttClient(espClient);

//define your default values here, if there are different values in config.json, they are overwritten.
char mqtt_server[40];
char mqtt_port[6] = "1883";
char mqtt_topic_command_enabled[40];
char mqtt_topic_state_enabled[40];
char mqtt_topic_notify[40];
char mqtt_topic_ring[40];

//flag for saving data
bool shouldSaveConfig = false;

unsigned long ringStarttime;
bool shouldRing;
bool doorbellEnabled = true;

unsigned long lastNotifySent = 0;

int buttonState;             // the current reading from the input pin
int lastButtonState = HIGH;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

const char* mqttClientIdPrefix = "doorbell-";

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup() {
  // Set motion sensor pins as inputs
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_SWITCH, INPUT_PULLUP);

  Serial.begin(115200);
  Serial.println("Starting up ESP");

  // Attach an interrupt to the pin, assign the onChange function as a handler and trigger on changes (LOW or HIGH).
  //attachInterrupt(PIN_SWITCH, onChangeSwitch, CHANGE);

  //read configuration from FS json
  Serial.println("Mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("Mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("Reading config file");
      File configFile = SPIFFS.open("/config.json", "r");

      if (configFile) {
        Serial.println("Opened config file");
        size_t size = configFile.size();

        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);

        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);

        if (json.success()) {
          Serial.println("\nParsed json");
          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(mqtt_port, json["mqtt_port"]);
          strcpy(mqtt_topic_command_enabled, json["mqtt_topic_command_enabled"]);
          strcpy(mqtt_topic_state_enabled, json["mqtt_topic_state_enabled"]);
          strcpy(mqtt_topic_notify, json["mqtt_topic_notify"]);
          strcpy(mqtt_topic_ring, json["mqtt_topic_ring"]);
          
        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read
  
  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  WiFiManagerParameter custom_mqtt_topic_command_enabled("topic_command_enabled", "mqtt enabled command topic", mqtt_topic_command_enabled, 40);
  WiFiManagerParameter custom_mqtt_topic_state_enabled("topic_state_enabled", "mqtt enabled state topic", mqtt_topic_state_enabled, 40);
  WiFiManagerParameter custom_mqtt_topic_notify("topic_notify", "mqtt notify topic", mqtt_topic_notify, 40);
  WiFiManagerParameter custom_mqtt_topic_ring("topic_ring", "mqtt ring topic", mqtt_topic_ring, 40);

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  //add all your parameters here
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_topic_command_enabled);
  wifiManager.addParameter(&custom_mqtt_topic_state_enabled);
  wifiManager.addParameter(&custom_mqtt_topic_notify);
  wifiManager.addParameter(&custom_mqtt_topic_ring);

  // Connect to wifi, or start access point
  Serial.println("Connecting to WiFi...");
  if(!wifiManager.autoConnect("ESP-Doorbell", "Doorbell123!")) {
    Serial.println("failed to connect and hit timeout. Resetting ESP.");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  Serial.println("Connected to WiFi!");
  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  //read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(mqtt_topic_command_enabled, custom_mqtt_topic_command_enabled.getValue());
  strcpy(mqtt_topic_state_enabled, custom_mqtt_topic_state_enabled.getValue());
  strcpy(mqtt_topic_notify, custom_mqtt_topic_notify.getValue());
  strcpy(mqtt_topic_ring, custom_mqtt_topic_ring.getValue());
  
  //save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("Saving config");

    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();

    json["mqtt_server"] = mqtt_server;
    json["mqtt_port"] = mqtt_port;
    json["mqtt_topic_command_enabled"] = mqtt_topic_command_enabled;
    json["mqtt_topic_state_enabled"] = mqtt_topic_state_enabled;
    json["mqtt_topic_notify"] = mqtt_topic_notify;
    json["mqtt_topic_ring"] = mqtt_topic_ring;

    File configFile = SPIFFS.open("/config.json", "w");

    if (!configFile) {
      Serial.println("Failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }

  /* Connect to MQTT broker */
  mqttClient.setServer(mqtt_server, atoi(mqtt_port));
  /* Set callback function for when we receive a message */
  mqttClient.setCallback(messageReceivedCallback);

  // Allow OTA updates
  ArduinoOTA.setHostname("Doorbell");
	ArduinoOTA.onStart([]() {
		Serial.println("Start Ota");
	});
	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd Ota");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("OTA Progress: %u%%\r", (progress / (total / 100)));
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("OTA Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
	});
	ArduinoOTA.begin();

  //Setup done, short audible feedback
  digitalWrite(PIN_RELAY, HIGH);
  delay(250);
  digitalWrite(PIN_RELAY, LOW);
}

void messageReceivedCallback(char* p_topic, byte* p_payload, unsigned int p_length) {
  Serial.print("Message arrived [");
  Serial.print(p_topic);
  Serial.print("] ");

  // concat the payload into a string
  String payload;
  for (uint8_t i = 0; i < p_length; i++) {
    payload.concat((char)p_payload[i]);
  }
  Serial.print(payload);
  Serial.println();

  if(String(mqtt_topic_command_enabled).equals(p_topic)) {
    Serial.println("Message received on Enabled topic");
    if(String("1").equals(payload)) {
      Serial.println("Doorbell enabled");
      doorbellEnabled = true;
      mqttClient.publish(mqtt_topic_state_enabled,"1", true);      
    } else {
      Serial.println("Doorbell disabled");
      doorbellEnabled = false;
      mqttClient.publish(mqtt_topic_state_enabled,"0", true);            
    }
  } else if (String(mqtt_topic_ring).equals(p_topic)) {
    // We don't care what the message is. Just ring on any message.
    Serial.println("Message received on Ring topic. Ringing doorbell!");
    shouldRing = true;
    ringStarttime = millis();
  }
}

void mqttReconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = mqttClientIdPrefix;
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");
      // ... and resubscribe
      mqttClient.subscribe(mqtt_topic_command_enabled);
      mqttClient.subscribe(mqtt_topic_ring);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void ringDoorbell() {
  if(shouldRing) {
    if(millis() - ringStarttime > 2000) {
      shouldRing = false;
      digitalWrite(PIN_RELAY, LOW);
    } else {
      digitalWrite(PIN_RELAY, HIGH);
    }
  }
}

// Gets called by the interrupt.
void readSwitch() {
    // read the state of the switch into a local variable:
  int reading = digitalRead(PIN_SWITCH);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;
      Serial.print("Switch state: ");
      Serial.println(!buttonState);
      if(doorbellEnabled) {
        digitalWrite(PIN_RELAY, !buttonState);
      }

      if(!buttonState)
      {
        mqttClient.publish(mqtt_topic_notify, "1", false);
      } else {
        mqttClient.publish(mqtt_topic_notify, "0", false);        
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}

void loop() {
  ArduinoOTA.handle();
  yield();

  // put your main code here, to run repeatedly:
  if (!mqttClient.connected()) {
    mqttReconnect();
  }
  mqttClient.loop();
  yield();

  readSwitch();
  yield();

  ringDoorbell();
  yield();
}
