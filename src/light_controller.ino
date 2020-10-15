/*
 *  ESP32 Light Controller
 *  by Cristian Malita
 *
 * 
 *
 */
 
#include "Arduino.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <IRremote.h>

IRsend irsend;

int currentStatus = 0;
int currentBrightness = 4;
String currentMode = "normal";
String currentColor = "white";
 
const char* ssid = "Nevada";
const char* password = "tenacity.recede.press";
 
WebServer server(80);

//Get Status
void getStatus() {
  String message = "{\"powered_on\" : ";
  message += String(currentStatus);
  message += ", \"brightness\" : ";
  message += String(currentBrightness);
  message += ", \"mode\" : ";
  message += "\"" + currentMode + "\"";
  message += ", \"color\" : ";
  message += "\"" + currentColor + "\"";
  message += "}";
  server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
  server.send(200, "text/json", message);
}
 
// Power on
void powerOn() {
    irsend.sendNEC(0xFFE01F, 32); //Power on
    currentStatus = 1; //Register powered on
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    server.send(200, "text/json", "{\"message\": \"Lamp powered on!\"}");
    
}

// Power off
void powerOff() {
    irsend.sendNEC(0xFF609F, 32); //Power off
    currentStatus = 0; //Register powered off
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    server.send(200, "text/json", "{\"message\": \"Lamp powered off!\"}");
}

void setColor(String color) {
  if (color == "white") {
    irsend.sendNEC(0xFFC03F, 32);
    currentColor = "white";
  } else if (color == "red") {
    irsend.sendNEC(0xFF10EF, 32);
    currentColor = "red";
    String message = "{\"message\": \"Color set to red\"}";
    server.send(200, "text/json", message);
  } else if (color == "green") {
    irsend.sendNEC(0xFF906F, 32);
    currentColor = "green";
  } else if (color == "blue") {
    irsend.sendNEC(0xFF50AF, 32);
    currentColor = "blue";
  } else if (color == "yellow") {
    irsend.sendNEC(0xFF18E7, 32);
    currentColor = "yellow";
  } else if (color == "purple") {
    irsend.sendNEC(0xFF708F, 32);
    currentColor = "purple";
  } else if (color == "lime") {
    irsend.sendNEC(0xFFB04F, 32);
    currentColor = "lime";
  } else if (color == "skyblue") {
    irsend.sendNEC(0xFFA857, 32);
    currentColor = "skyblue";
  } else if (color == "turquoise") {
    irsend.sendNEC(0xFF8877, 32);
    currentColor = "turquoise";
  } else if (color == "violet") {
    irsend.sendNEC(0xFF48B7, 32);
    currentColor = "violet";
  } else if (color == "pink") {
    irsend.sendNEC(0xFF58A7, 32);
    currentColor = "pink";
  } else {
    irsend.sendNEC(0xFFC03F, 32);
    currentColor = "white";
    Serial.println("Invalid color, defaulting to white...");
  }
  String message = "{\"message\": \"Color set to " + currentColor + "\"}";
  server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
  server.send(200, "text/json", message);
}

void setMode(String mode) {
  if (mode == "normal") {
    irsend.sendNEC(0xFFC03F, 32);
    currentMode = "normal";
    currentColor = "white";
  } else if (mode == "strobe") {
    currentMode = "strobe";
    irsend.sendNEC(0xFFC837, 32);
  } else if (mode == "flash") {
    currentMode = "flash";
    irsend.sendNEC(0xFFF00F, 32);
  } else if (mode == "smooth") {
    currentMode = "smooth";
    irsend.sendNEC(0xFFE817, 32);
  } else {
    currentMode = "normal";
    setColor(currentColor);
    Serial.println("Invalid mode, defaulting to normal...");
  }
  String message = "{\"message\": \"Mode set to " + currentMode + "\"}";
  server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
  server.send(200, "text/json", message);
}

void setBrightness(int level) {
  if (level < 1 || level > 4) {
    Serial.println("Brightness level has to be between 1 and 4.");  
  }  else {
    if (level > currentBrightness) {
        int diff = level - currentBrightness;
        for (int i = 0; i < diff; i++) {
          irsend.sendNEC(0xFF00FF, 32);
        } 
    } else if (level < currentBrightness) {
      int diff = currentBrightness - level;
        for (int i = 0; i < diff; i++) {
          irsend.sendNEC(0xFF40BF, 32);
        } 
    }
  }
    currentBrightness = level;
    String message = "{\"message\": \"Brightness set to " + String(currentBrightness) + "\"}";
    server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
    server.send(200, "text/json", message);
 }

 void reset() {
  irsend.sendNEC(0xFFE01F, 32); //Power on
  currentStatus = 1; //Register powered on
  irsend.sendNEC(0xFFC03F, 32);
  currentColor = "white";
  currentMode = "normal";
  irsend.sendNEC(0xFF00FF, 32);
  irsend.sendNEC(0xFF00FF, 32);
  irsend.sendNEC(0xFF00FF, 32);
  irsend.sendNEC(0xFF00FF, 32);
  currentBrightness = 1;
  String message = "{\"message\": \"Lamp has been reset\"}";
  server.sendHeader(F("Access-Control-Allow-Origin"), F("*"));
  server.send(200, "text/json", message);
}

// Set params
void setParam() {
    if (server.args() == 0) {
      String message = "{\"error\": \"No arguments provided\"}";
      server.send(400, "text/json", message);
    } else if (server.args() > 1) {
      String message = "{\"error\": \"Only one argument allowed\"}";
      server.send(400, "text/json", message);
    } else if (server.argName(0) == "color") {
      setColor(server.arg(0));
    } else if (server.argName(0) == "mode") {
      setMode(server.arg(0));
    } else if (server.argName(0) == "brightness") {
      setBrightness(server.arg(0).toInt());
    } else {
      String message = "{\"error\": \"Unknown argument\"}";
      server.send(400, "text/json", message);
    }
}
 
// Define routing
void restServerRouting() {
    server.on("/", HTTP_GET, []() {
        server.send(200, F("text/html"),
            F("Welcome to the Zibix Smart Lamp"));
    });
    server.on(F("/poweron"), HTTP_GET, powerOn);
    server.on(F("/poweroff"), HTTP_GET, powerOff);
    server.on(F("/status"), HTTP_GET, getStatus);
    server.on(F("/set"), HTTP_GET, setParam);
    server.on(F("/reset"), HTTP_GET, reset);
}
 
// Manage not found URL
void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}
 
void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  reset();
 
  // Activate mDNS this is used to be able to connect to the server
  // with local DNS hostmane esp8266.local
  if (MDNS.begin("zibix")) {
    Serial.println("MDNS responder started");
  }
 
  // Set server routing
  restServerRouting();
  // Set not found response
  server.onNotFound(handleNotFound);
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void) {
  server.handleClient();
}
