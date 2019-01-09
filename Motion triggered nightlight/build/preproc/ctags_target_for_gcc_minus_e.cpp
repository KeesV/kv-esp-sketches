# 1 "e:\\Git\\kv-esp-sketches\\Motion triggered nightlight\\MotionTriggeredNightlight\\MotionTriggeredNightlight.ino"
# 1 "e:\\Git\\kv-esp-sketches\\Motion triggered nightlight\\MotionTriggeredNightlight\\MotionTriggeredNightlight.ino"
# 2 "e:\\Git\\kv-esp-sketches\\Motion triggered nightlight\\MotionTriggeredNightlight\\MotionTriggeredNightlight.ino" 2
# 3 "e:\\Git\\kv-esp-sketches\\Motion triggered nightlight\\MotionTriggeredNightlight\\MotionTriggeredNightlight.ino" 2
# 4 "e:\\Git\\kv-esp-sketches\\Motion triggered nightlight\\MotionTriggeredNightlight\\MotionTriggeredNightlight.ino" 2

const char* ssid = "KInternet";
const char* password = "+RSWKCpvCm\\j6`4";
const char* mqtt_server = "home.lan";
const char* mqttClientIdPrefix = "nightlight-";
const char* stateBrightnessTopic = "bedroomnightlight/state/brightness"; //Publish actual brightness here
const char* stateMotionTopic = "bedroomnightlight/state/motion"; //Publish motion here
const char* commandBrightnessTopic = "bedroomnightlight/command/brightness"; //Subscribe to this topic to receive brightness changes
const char* commandTopic = "bedroomnightlight/command/switch"; //Subscribe to this topic to receive ON/OFF changes
const char* stateTopic = "bedroomnightlight/state/switch"; //Publish ON/OFF state here

// payloads by default (on/off)
const char* LIGHT_ON = "ON";
const char* LIGHT_OFF = "OFF";
const char* MOTION_ON = "ON";
const char* MOTION_OFF = "OFF";

// Vars to keep track of internal state
int CurrentDimValue = 0; //Current dim value for the LEDs
int DesiredDimValue = 0; //Desired dim value for the LEDs. This value depends on the status of the motion sensors.
int MaxDimValue = 255; //Maximum dim value for the LEDs. This is set through MQTT. Goes from 0 - 255.

bool currentSwitchState = false; //Whether the LED should be on or not. True = on, false = off.
bool previouslyTripped = false; //Previous value of motion sensors tripped or not. Used to only send updates when it was changed.
unsigned long reachedFadeMaxAt = 0;

//States for the state machine






int state = 0;





// Fading parameters




WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setup() {
  // Set motion sensor pins as inputs
  pinMode(D5 /*the pin the first motion sensor is attached to*/, 0x00);
  pinMode(D6 /*the pin the second motion sensor is attached to*/, 0x00);

  // Set our LED pin as an output pin
  pinMode(D1 /* Pin to which we have connected the LED*/, 0x01);
  analogWriteRange(255);

  Serial.begin(115200);
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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

  /* Connect to MQTT broker */
  mqttClient.setServer(mqtt_server, 1883);
  /* Set callback function for when we receive a message */
  mqttClient.setCallback(messageReceivedCallback);

  // Allow OTA updates
  ArduinoOTA.setHostname("MotionNightlight");
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

  // We are ready, let's dance!
  for(int i = 0; i< 10; i++)
  {
    analogWrite(D1 /* Pin to which we have connected the LED*/, 255);
    delay(100);
    analogWrite(D1 /* Pin to which we have connected the LED*/, 0);
    delay(100);
  }
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

  if(String(commandTopic).equals(p_topic)) {
    Serial.println("Message received on command topic");
    setSwitchState(payload);
  } else if (String(commandBrightnessTopic).equals(p_topic)) {
    Serial.println("Message received on brightness topic");
    setMaxBrightness(payload.toInt());
  }
}

void setSwitchState(String newState) {
  if(newState.equals(String(LIGHT_ON))) {
    Serial.println("Switching light on...");
    currentSwitchState = true;
    // Publish new state to state topic
    mqttClient.publish(stateTopic, LIGHT_ON, true);
  } else if (newState.equals(String(LIGHT_OFF))) {
    Serial.println("Switching light off");
    currentSwitchState = false;
    // Publish new state to state topic
    mqttClient.publish(stateTopic, LIGHT_OFF, true);
  }
}

void setMaxBrightness(int newMaxBrightness) {
  Serial.print("Setting maximum brightness to: ");
  Serial.println(newMaxBrightness);

  MaxDimValue = newMaxBrightness;
  //Convert number back into char array and publish it
  char buf[5];
  sprintf(buf, "%i\n", MaxDimValue);
  mqttClient.publish(stateBrightnessTopic, buf, true);
}

void setTripped(bool tripped) {
  Serial.print("Tripped: ");
  Serial.println(tripped);

  if(tripped) {
    mqttClient.publish(stateMotionTopic, MOTION_ON, true);
  } else {
    mqttClient.publish(stateMotionTopic, MOTION_OFF, true);
  }

  previouslyTripped = tripped;
}

void mqttReconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = mqttClientIdPrefix;
    clientId += String(random(0xffff), 16);
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("connected");
      // ... and resubscribe
      mqttClient.subscribe(commandBrightnessTopic);
      mqttClient.subscribe(commandTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

bool atDesiredDimValue()
{
  return (abs(CurrentDimValue - DesiredDimValue) < 1 /* how many points to fade the LED by*/);
}

void fadeToLevel() {
  if (!atDesiredDimValue()) {
    int delta = ( DesiredDimValue - CurrentDimValue ) < 0 ? -1 : 1;

    CurrentDimValue += delta * 1 /* how many points to fade the LED by*/;
    Serial.print("analog write: ");
    Serial.println(CurrentDimValue);
    analogWrite(D1 /* Pin to which we have connected the LED*/, 255-CurrentDimValue); //Inverted pin
    delay(10 /*delay between fade steps, in MS*/);
  } else if(DesiredDimValue == 0)
  {
    CurrentDimValue = 0;
    analogWrite(D1 /* Pin to which we have connected the LED*/, 255); //Inverted pin, so 255 means off 
  }
}

void loop() {
  ArduinoOTA.handle();
  yield();

  bool tripped1 = digitalRead(D5 /*the pin the first motion sensor is attached to*/) == 0x1;
  bool tripped2 = digitalRead(D6 /*the pin the second motion sensor is attached to*/) == 0x1;
  bool currentlyTripped = tripped1 || tripped2;

  // put your main code here, to run repeatedly:
  if (!mqttClient.connected()) {
    mqttReconnect();
  }
  mqttClient.loop();

  if(previouslyTripped != currentlyTripped) {
    setTripped(currentlyTripped);
  }

  switch(state)
  {
    case 0:
      if(currentlyTripped && currentSwitchState == true)
      {
        state = 1;
      }
    break;
    case 1:
      DesiredDimValue = MaxDimValue;
      state = 2;
    break;
    case 2:
      if(atDesiredDimValue())
      {
        reachedFadeMaxAt = millis();
        state = 3;
      }
    break;
    case 3:
      if(currentlyTripped) {
        reachedFadeMaxAt = millis();
      }

      if(millis() - reachedFadeMaxAt > 5000 /*Time to wait before fading down after no motion is detected anymore*/)
      {
        DesiredDimValue = 0;
        state = 4;
      }
    break;
    case 4:
      if(currentlyTripped)
      {
        DesiredDimValue = MaxDimValue;
        state = 2;
      }
      if(atDesiredDimValue())
      {
        state = 0;
      }
    break;
  }

  fadeToLevel();

  // //Set PWM for LED to set value
  // if(currentSwitchState == true) {
  //   analogWrite(LED_PIN, 255-currentBrightness);
  // } else  {
  //   analogWrite(LED_PIN, 255);
  // }

  // Give others a chance to do their work
  yield();
}
