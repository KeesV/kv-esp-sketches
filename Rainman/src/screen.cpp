#include "screen.h"

Adafruit_SSD1306 display(OLED_RESET);
unsigned long last_shown_time;
unsigned long last_cleared_time;

int state;

#define state_showing 0
#define state_blank 1

void start_screen() {
    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
    // init done

    // Show image buffer on the display hardware.
    // Since the buffer is intialized with an Adafruit splashscreen
    // internally, this will display the splashscreen.
    display.display();
    last_shown_time = millis();
    state = state_showing;
}

void handle_screen() {
    if(state == state_showing) {
        if(millis() - last_shown_time > 5000) {
            display.clearDisplay();
            display.display();
            last_cleared_time = millis();
            state = state_blank;
        }
    }

    if(state == state_blank) {
        if(millis() - last_cleared_time > 5000) {
            display.setTextSize(2);
            display.setTextColor(WHITE);
            display.setCursor(0,0);
            display.println("Rainman!");
            display.display();
            last_shown_time = millis();
            state = state_showing;
        }
    }
}