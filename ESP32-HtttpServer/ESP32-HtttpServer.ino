#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "WifiCredentials.h"

WebServer server(80);

const int led = 33;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/html", "<html><body>Hello from ESP-32!</body></html>");
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
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
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
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

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/left", []() {
    Serial.println("left");
    server.send(200, "text/plain", "Going left");
  });

  server.on("/right", []() {
    Serial.println("right");
    server.send(200, "text/plain", "Going right");
  });

  server.on("/stop", []() {
    Serial.println("stop");
    server.send(200, "text/plain", "Stopping");
  });

  server.on("/forward", []() {
    Serial.println("forward");
    server.send(200, "text/plain", "Going forward");
  });

  server.on("/backward", []() {
    Serial.println("backward");
    server.send(200, "text/plain", "Going backward");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
