#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <ESPTemplateProcessor.h>
#include "settings.h"

void start_webserver(Settings& settings);
void handle_webserver();