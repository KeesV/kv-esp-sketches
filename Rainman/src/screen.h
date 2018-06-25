#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "state.h"

#define OLED_RESET -1

class Screen {
    public:
    Screen();
    void Start(State& state);
    void Handle();

    private:
    Adafruit_SSD1306 display;
    State state;
    void DisplayStationStatus(int stationNumber, bool status);
};