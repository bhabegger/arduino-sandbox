#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Wire.h>
#include "WifiCredentials.h"

#define I2C_SDA 15
#define I2C_SCL 13

WebServer server(80);

const int led = 33;
const char* indexHtml = "<html>\n  <head>\n<script\n  src=\"https://code.jquery.com/jquery-3.5.1.min.js\"\n  integrity=\"sha256-9/aliU8dGd2tb6OSsuzixeV4y/faTqgFtohetphbbj0=\"\n  crossorigin=\"anonymous\">\n</script>\n<link\n  rel=\"stylesheet\" \n  href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\"\n  integrity=\"sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T\"\n  crossorigin=\"anonymous\">\n</link>\n<script\n  src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js\"\n  integrity=\"sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1\"\n  crossorigin=\"anonymous\">\n</script>\n<script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js\" integrity=\"sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM\" crossorigin=\"anonymous\"></script>\n<script>\nconst key_enter = 13;\nconst key_up    = 38;\nconst key_down  = 40;\nconst key_right = 39;\nconst key_left  = 37;\nconst key_esc   = 27;\nconst key_space = 32;\nconst key_ctrl  = 17;\nconst key_alt   = 18;\nconst key_shift = 16;\n\nfunction updateConsole(data,status,jqXHR) {\n  $(\"#console\").html(data);\n}\n\nvar commands = {\n  stop: function() {\n    console.log(\"Stop\");\n    $.get(\"/stop\",null,updateConsole);\n  },\n  forward: function() {\n    console.log(\"Forward\");\n    $.get(\"/forward\",null,updateConsole)\n  },\n  backward: function() {\n    console.log(\"Backward\");\n    $.get(\"/backward\",null,updateConsole)\n  },\n  left: function() {\n    console.log(\"Left\");\n    $.get(\"/left\",null,updateConsole)\n  },\n  right: function() {\n    console.log(\"Right\");\n    $.get(\"/right\",null,updateConsole)\n  }\n}\n\n$(function() {\n  $(\"body\").keydown(function(evt) {\n    switch(evt.which) {\n       case key_up: \n         commands.forward();\n         break;\n       case key_down:\n         commands.backward();\n         break;\n      case key_right:\n         commands.right();\n         break;\n      case key_left:\n         commands.left();\n         break;\n      case key_space:\n         commands.stop();\n         break;\n    }\n  });\n\n  $(\"#stop\").click(commands.stop);\n  $(\"#forward\").click(commands.forward);\n  $(\"#backward\").click(commands.backward);\n  $(\"#left\").click(commands.left);\n  $(\"#right\").click(commands.right);\n});\n\n</script>\n</head>\n<body>\n  <div class=\"container-fluid\">\n    <div id=\"cam\" class=\"alert alert-secondary d-flex justify-content-center\" role=\"alert\">\n    </div>\n  </div>\n<div class=\"container\">\n  <div class=\"row p-2\">\n    <div class=\"col-sm d-flex justify-content-center\">\n    </div>\n    <div class=\"col-sm d-flex justify-content-center\">\n      <button id=\"forward\" type=\"button\" class=\"btn btn-dark\">Forward</button>\n    </div>\n    <div class=\"col-sm d-flex justify-content-center\">\n    </div>\n  </div>\n  <div class=\"row p-2\">\n    <div class=\"col-sm d-flex justify-content-center\">\n      <button id=\"left\" type=\"button\" class=\"btn btn-dark\">Left</button>\n    </div>\n    <div class=\"col-sm d-flex justify-content-center\">\n      <button id=\"stop\" type=\"button\" class=\"btn btn-dark\">Stop</button>\n    </div>\n    <div class=\"col-sm d-flex justify-content-center\">\n      <button id=\"right\" type=\"button\" class=\"btn btn-dark\">Right</button>\n    </div>\n  </div>\n    <div class=\"row p-2\">\n    <div class=\"col-sm d-flex justify-content-center\">\n    </div>\n    <div class=\"col-sm d-flex justify-content-center\">\n      <button id=\"backward\" type=\"button\" class=\"btn btn-dark\">Backward</button>\n    </div>\n    <div class=\"col-sm d-flex justify-content-center\">\n    </div>\n  </div>\n  </div>\n  <div class=\"container-fluid\">\n    <div id=\"console\" class=\"alert alert-secondary d-flex justify-content-center\" role=\"alert\">\n          Welcome\n    </div>\n  </div>\n</body>\n</html>";

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);

  setupI2C();
  setupWifi();
  setupMDNS();
  setupHttpEndpoints();
}

void setupI2C() {
  Wire.begin(I2C_SDA,I2C_SCL);
}

void setupWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setupMDNS() {
  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  } else {
    Serial.println("Failed to start MDNS responder");
  }
}

void i2cSend(char* msg) {
  Wire.beginTransmission(4);
  Wire.write(msg);
  Wire.endTransmission();
}

void setupHttpEndpoints() {

  server.on("/", handleRoot);

  server.on("/left", []() {
    Serial.println("left");
    i2cSend("left");
    server.send(200, "text/plain", "Going left");
  });

  server.on("/right", []() {
    Serial.println("right");
    i2cSend("right");
    server.send(200, "text/plain", "Going right");
  });

  server.on("/stop", []() {
    Serial.println("stop");
    i2cSend("stop");
    server.send(200, "text/plain", "Stopping");
  });

  server.on("/forward", []() {
    Serial.println("forward");
    i2cSend("forward");
    server.send(200, "text/plain", "Going forward");
  });

  server.on("/backward", []() {
    Serial.println("backward");
    i2cSend("backward");
    server.send(200, "text/plain", "Going backward");
  });

  server.on("/auto", []() {
    Serial.println("auto");
    i2cSend("auto");
    server.send(200, "text/plain", "Activating automatic mode");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/html", indexHtml);
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

void loop(void) {
  server.handleClient();
}
