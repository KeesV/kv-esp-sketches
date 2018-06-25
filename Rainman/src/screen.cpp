#include "screen.h"

Screen::Screen() : display(OLED_RESET) {
}

void Screen::Start(State& state) {
    this->state = state;

    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
    // init done
    this->display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
    this->display.clearDisplay();

    this->DisplayStationStatus(1, this->state.GetStationStatus(1));
    this->DisplayStationStatus(2, this->state.GetStationStatus(2));
    this->DisplayStationStatus(3, this->state.GetStationStatus(3));
    this->DisplayStationStatus(4, this->state.GetStationStatus(4));
    this->DisplayStationStatus(5, this->state.GetStationStatus(5));
    this->DisplayStationStatus(6, this->state.GetStationStatus(6));
    
    this->display.display();
}

void Screen::DisplayStationStatus(int stationNumber, bool status)
{
    int frontColor = status ? BLACK : WHITE;
    int backColor = status ? WHITE : BLACK;
    int n = stationNumber - 1;
    if(status) {
        this->display.fillRoundRect(n*21 + 1, 43, 20, 20, 4, backColor);
    } else {
        this->display.drawRoundRect(n*21 + 1, 43, 20, 20, 4, frontColor);
    }
    this->display.drawChar(n*21 + 6, 46, stationNumber + '0', frontColor, backColor, 2);
}

void Screen::Handle() {
    
}