#include "webserver.h"

ESP8266WebServer server(80);

// String getContentType(String filename){
//   if(filename.endsWith(".htm")) return "text/html";
//   else if(filename.endsWith(".html")) return "text/html";
//   else if(filename.endsWith(".css")) return "text/css";
//   else if(filename.endsWith(".js")) return "application/javascript";
//   else if(filename.endsWith(".png")) return "image/png";
//   else if(filename.endsWith(".gif")) return "image/gif";
//   else if(filename.endsWith(".jpg")) return "image/jpeg";
//   else if(filename.endsWith(".ico")) return "image/x-icon";
//   else if(filename.endsWith(".xml")) return "text/xml";
//   else if(filename.endsWith(".pdf")) return "application/x-pdf";
//   else if(filename.endsWith(".zip")) return "application/x-zip";
//   else if(filename.endsWith(".gz")) return "application/x-gzip";
//   return "text/plain";
// }

void send_file_from_spiffs(String filename, String contenttype)
{
    if (SPIFFS.exists(filename)) {                            // If the file exists
        File file = SPIFFS.open(filename, "r");                 // Open it
        size_t sent = server.streamFile(file, contenttype); // And send it to the client
        file.close();    // Then close the file again
        Serial.println("File sent to client");                               
    } else {
        Serial.println("File not found in SPIFFS!");
    }
}

String indexProcessor(const String& key) {
    Serial.println(String("KEY IS ") + key);
    if (key == "MqttBrokerHost") return "MQTT Host";
    else if (key == "MqttBrokerPort") return "MQTT Port";
    else if (key == "MqttCommandTopicBase") return "MQTT Command Topic Base";
    else if (key == "MqttStateTopicBase") return "MQTT State Topic Base";
    else if (key == "MqttRetainChecked") return "CHECKED";
    else if (key == "MqttPayloadOn") return "MQTT Payload On";
    else if (key == "MqttPayloadOff") return "MQTT Payload Off";

    return "oops";
}

void handle_index() {
    Serial.println("Got request: index!");
    if (ESPTemplateProcessor(server).send(String("/index.html"), indexProcessor)) {
        Serial.println("Succesfully processed template and sent to client!");
    } else {
        Serial.println("Failed sending index to client!");
        server.send(404, "text/plain", "page not found.");
    }
}

void handle_bootstrap_min_css() {
    Serial.println("Got request: bootstrap.min.css");
    send_file_from_spiffs("/bootstrap/css/bootstrap.min.gz", "text/css");
}

void handle_NotFound() {
    server.send(404, "text/plain", "404: Not Found");
}

void saveSettings() {
    // Save settings here
    Serial.println("Saving settings...");
    String MqttBrokerHost = server.arg("MqttBrokerHost");
    String MqttBrokerPort = server.arg("MqttBrokerPort");
    String MqttCommandTopicBase = server.arg("MqttCommandTopicBase");
    String MqttStateTopicBase = server.arg("MqttStateTopicBase");
    String MqttRetain = server.arg("MqttRetain");
    String MqttPayloadOn = server.arg("MqttPayloadOn");
    String MqttPayloadOff = server.arg("MqttPayloadOff");

    Serial.print("Number of args: ");
    Serial.println(server.args());

    Serial.print("MqttBrokerHost: ");
    Serial.println(MqttBrokerHost);
    Serial.print("MqttBrokerPort: ");
    Serial.println(MqttBrokerPort);
    Serial.print("MqttCommandTopicBase: ");
    Serial.println(MqttCommandTopicBase);
    Serial.print("MqttStateTopicBase: ");
    Serial.println(MqttStateTopicBase);
    Serial.print("MqttRetain: ");
    Serial.println(MqttRetain);
    Serial.print("MqttPayloadOn: ");
    Serial.println(MqttPayloadOn);
    Serial.print("MqttPayloadOff: ");
    Serial.println(MqttPayloadOff);

    server.sendHeader("Location", "/");
    server.send(303);  
}

void start_webserver() {
    Serial.println("Starting webserver...");
    SPIFFS.begin();
    server.on("/", HTTP_GET, handle_index);
    server.on("/", HTTP_POST, saveSettings);
    server.on("/bootstrap/css/bootstrap.min.css", handle_bootstrap_min_css);
    server.onNotFound(handle_NotFound);
    
    server.begin();
    Serial.println("Webserver started!");
}

void handle_webserver() {
    server.handleClient();
}