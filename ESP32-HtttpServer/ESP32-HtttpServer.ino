#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Wire.h>
#include "esp_camera.h"
#include "camera_pins.h"
#include "HttpServerIndex.h"
#include "WifiCredentials.h"

#define I2C_SDA 15
#define I2C_SCL 13

WebServer server(80);

const int led = 33;
const char* indexHtml = INDEX_HTML;


const char* boundary = "\r\n--123456789000000000000987654321\r\n";
size_t boundary_len = strlen(boundary);

void setup(void) {
    pinMode(led, OUTPUT);
    digitalWrite(led, 0);
    Serial.begin(115200);

    setupCamera();
    setupI2C();
    setupWifi();
    setupMDNS();
    setupHttpEndpoints();
}

void setupCamera() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    //init with high specs to pre-allocate larger buffers
    if(psramFound()){
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 10;
        config.fb_count = 2;
    } else {
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }
    config.frame_size = FRAMESIZE_VGA;
    esp_camera_init(&config);
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

    server.on("/capture", []() {
        camera_fb_t * fb = esp_camera_fb_get();
        if(!fb) {
            server.send(500, "text/pain", "Camera failed to capture image");
        } else {
            server.sendHeader("Content-Disposition", "inline; filename=capture.jpg");
            server.sendHeader("Access-Control-Allow-Origin", "*");
            server.setContentLength(fb->len);
            server.send(200, "image/jpeg", "");
            server.client().write((const char *)fb->buf, fb->len);
            esp_camera_fb_return(fb);
        }
    });

    server.on("/stream", []() {
        Serial.println("streaming");
        char buffer[80];
        WiFiClient* client = new WiFiClient();
        *client = server.client();

        TaskHandle_t taskHandle;
        xTaskCreatePinnedToCore(
            streamTask,
            "stream",
            4 * 1024,
            (void*) client, //(void*) handler,
            2,
            &taskHandle,
            1
        );
    });
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void streamTask(void* param) {
    Serial.println("Starting streaming task");
    char buffer[80];
    WiFiClient* client = (WiFiClient*)param;
    camera_fb_t * fb = esp_camera_fb_get();

    // Send main header
    sprintf(buffer, "HTTP/1.1 200 OK\r\n");
    client->write(buffer, strlen(buffer));
    sprintf(buffer, "Access-Control-Allow-Origin: *\r\n");
    client->write(buffer, strlen(buffer));
    sprintf(buffer, "Content-Type: multipart/x-mixed-replace; boundary=123456789000000000000987654321\r\n");
    client->write(buffer, strlen(buffer));

    // Write first boundary
    client->write(boundary, boundary_len);

    // Stream
    for(int framesSent = 0;client->connected();framesSent++) {
        if(framesSent % 10 == 0) {
            Serial.print("sending frame: ");
            Serial.println(framesSent);
        }
        sprintf(buffer, "Content-Type: image/jpeg\r\n");
        client->write(buffer, strlen(buffer));

        sprintf(buffer, "Content-Length: %d\r\n", fb->len);
        client->write(buffer, strlen(buffer));

        client->write("\r\n", 2);
        client->write((const char *)fb->buf, fb->len);

        client->write(boundary, boundary_len);
        esp_camera_fb_return(fb);
        fb = esp_camera_fb_get();
        taskYIELD();
    }
    Serial.print("client disconnected");
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
