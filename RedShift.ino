// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "LittleFS.h"

// Replace with your network credentials
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

int elevation_pins[] = {16,5,4,0};
int azimuth_pins[] = {14,12,13,15};

int x,y, x_dir, y_dir, x_temp, y_temp;
unsigned long current_time;
unsigned long last_x_step_time, last_y_step_time;
int elevation_step, azimuth_step;

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Create a WebSocket object
AsyncWebSocket ws("/ws");
String message = "";

// Motor control function handler
void move_with_speed();
void go_to_position();
void track(); 
void (*move_now)() = &track;
void set_position(int*, int);

void setup()
{  
  Serial.begin(115200);

  // Initialize filesystem.
  if (!LittleFS.begin()) {
    Serial.println("An error has occurred while mounting LittleFS");
  }
  else {
    Serial.println("LittleFS mounted successfully");
  } 

  // Initialize wifi.
  /*IPAddress ip(192,168,1,6); 
  IPAddress gateway(192,168,1,1); 
  IPAddress subnet(255,255,255,0); 
  WiFi.config(ip, gateway, subnet); */ 
  Serial.print("\n");
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  { 
    Serial.print("*"); 
    delay(500); 
  } 
  Serial.print("\n"); 
  Serial.print("Connected to Wi-Fi: "); 
  Serial.println(WiFi.SSID()); 
  delay(100); 

  // Initialize web socket
  ws.onEvent(onEvent); 
  server.addHandler(&ws); 

  // Server responses
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) { 
    request->send(LittleFS, "/index.html", "text/html"); 
  }); 
  Serial.print("\n"); 
  Serial.println("Starting ESP8266 Web Server..."); 
  server.serveStatic("/", LittleFS, "/"); 
  AsyncElegantOTA.begin(&server); 
  server.begin(); /* Start the HTTP web Server */ 
  Serial.println("ESP8266 Web Server Started"); 
  Serial.print("The IP of ESP8266 Web Server is: "); 
  Serial.println(WiFi.localIP()); 
  Serial.print("\n");

  for (int i = 0; i < 4; i++){
    pinMode(elevation_pins[i], OUTPUT);
    pinMode(azimuth_pins[i], OUTPUT);
  }
}

void loop(){
  (*move_now)();
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      //Notify client of motor current state when it first connects
      //notifyClients(direction);
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      message = (char*)data;
      message = message.substring(0, len);
      Serial.println(message);
      if (message.substring(0,2).equals("xy")) {
        // If this is a new xy session, set timers and controll function
        if ((x == 0) && (y == 0)) {
          last_x_step_time = last_y_step_time = micros();
          move_now = &move_with_speed;
        }
        // Parse xy position/speed
        x_temp = message.substring(2, message.indexOf(":")).toInt();
        y_temp = message.substring(message.indexOf(":") + 1).toInt();
        if (x_temp > 0) {
          x_dir = 1;
          if (x_temp > 150) {x_temp = 150;}
          x = x_temp;
        } else {
          x_dir = -1;
          if (x_temp < -150) {x_temp = -150;}
          x = -1 * x_temp;
        }
        if (y_temp > 0) {
          y_dir = 1;
          if (y_temp > 150) {y_temp = 150;}
          y = y_temp;
        } else {
          y_dir = -1;
          if (y_temp < -150) {y_temp = -150;}
          y = -1 * y_temp;
        }
        
        if ((x == 0) && (y == 0)) { // End of xy session
          move_now = &track;
        }
      } else {
        move_now = &track;
      }
      Serial.println(x);
      Serial.println(y);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void move_with_speed() {
  current_time = micros();
  if ((x != 0) && (current_time > last_x_step_time + 150000 / x)) {
    elevation_step += x_dir;
    if (elevation_step == 8) {elevation_step = 0;}
    if (elevation_step == -1){elevation_step = 7;}
    last_x_step_time = current_time;
  }
  if ((y != 0) && (current_time > last_y_step_time + 150000 / y)) {
    azimuth_step += y_dir;
    if (azimuth_step == 8) {azimuth_step = 0;}
    if (azimuth_step == -1){azimuth_step = 7;}
    last_y_step_time = current_time;
  }
  set_position(elevation_pins, elevation_step);
  set_position(azimuth_pins, azimuth_step);
}

void go_to_position() {}

void track(){}

void set_position(int *pin, int Position){
  switch(Position){
    case 0:
      digitalWrite(pin[0], LOW);
      digitalWrite(pin[1], LOW);
      digitalWrite(pin[2], LOW);
      digitalWrite(pin[3], HIGH);
      break;
    case 1:
      digitalWrite(pin[0], LOW);
      digitalWrite(pin[1], LOW);
      digitalWrite(pin[2], HIGH); 
      digitalWrite(pin[3], HIGH);
      break;
    case 2:
      digitalWrite(pin[0], LOW);
      digitalWrite(pin[1], LOW);
      digitalWrite(pin[2], HIGH);
      digitalWrite(pin[3], LOW);
      break;
    case 3:
      digitalWrite(pin[0], LOW);
      digitalWrite(pin[1], HIGH);
      digitalWrite(pin[2], HIGH);
      digitalWrite(pin[3], LOW);
      break;
    case 4:
      digitalWrite(pin[0], LOW);
      digitalWrite(pin[1], HIGH);
      digitalWrite(pin[2], LOW);
      digitalWrite(pin[3], LOW);
      break;
    case 5:
      digitalWrite(pin[0], HIGH);
      digitalWrite(pin[1], HIGH);
      digitalWrite(pin[2], LOW);
      digitalWrite(pin[3], LOW);
      break;
    case 6:
      digitalWrite(pin[0], HIGH);
      digitalWrite(pin[1], LOW);
      digitalWrite(pin[2], LOW);
      digitalWrite(pin[3], LOW);
      break;
    case 7:
      digitalWrite(pin[0], HIGH);
      digitalWrite(pin[1], LOW);
      digitalWrite(pin[2], LOW);
      digitalWrite(pin[3], HIGH);
      break;
    default:
      digitalWrite(pin[0], HIGH);
      digitalWrite(pin[1], LOW);
      digitalWrite(pin[2], LOW);
      digitalWrite(pin[3], LOW);
      break;
  }
}
