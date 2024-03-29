// Import required libraries
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include "LittleFS.h"

// Replace with your network credentials
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

int altitude_pins[] = {16,5,4,0};
int azimuth_pins[] = {14,12,13,15};
int laser_pin = 9;

bool is_laser_on = false;

// Maximum number of stepper steps per second
const int maximum_frequency = 1000;

int x,y, x_dir, y_dir, x_temp, y_temp, x_period_us, y_period_us;
unsigned long current_time;
unsigned long last_x_step_time, last_y_step_time;
int altitude_step_no, azimuth_step_no;

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
// Create a WebSocket object
AsyncWebSocket ws("/ws");
String message = "";

// Motor control function handler
void moveWithSpeed();
void goToPosition();
void track(); 
void (*move_now)() = &track;

void handleCommand(String);
void setPosition(int*, int);

void setup()
{ 
  pinMode(laser_pin, OUTPUT);
  digitalWrite(laser_pin, LOW);
   
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
  WiFi.hostname("redshift");
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
    pinMode(altitude_pins[i], OUTPUT);
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
      handleCommand(message);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void handleCommand(String command){
  if (command.substring(0,2).equals("xy")) {
    // If this is a new xy session, set times and controll function
    if ((x == 0) && (y == 0)) {
      last_x_step_time = last_y_step_time = micros();
      move_now = &move_with_speed;
    }
    // Parse xy speed
    // Speed is converted to time between consecutive steps and the direction of steps
    x_temp = command.substring(2, command.indexOf(":")).toInt();
    y_temp = command.substring(command.indexOf(":") + 1).toInt();
    x_dir = x_temp > 0 ? 1 : -1;
    x = abs(x_temp);
    y_dir = y_temp > 0 ? 1 : -1;
    y = abs(y_temp);
    
    if ((x == 0) && (y == 0)) { // End of xy session
      move_now = &track;
    } else {  // Set stepping period
      if (x > 0) {x_period_us = (100 / x) * (1000000 / maximum_frequency);}
      if (y > 0) {y_period_us = (100 / y) * (1000000 / maximum_frequency);}
    }
  } else if (command.substring(0,2).equals("ll")) {
    is_laser_on = !is_laser_on;
    if (is_laser_on) {
      digitalWrite(laser_pin, HIGH);
    } else {
      digitalWrite(laser_pin, LOW);
    }
    
  } else {
    move_now = &track;
  }
}

void move_with_speed() {
  current_time = micros();
  // Move x motor one step further if it's time
  if ((x != 0) && (current_time > last_x_step_time + x_period_us)) {
    altitude_step_no += x_dir;
    altitude_step_no = (altitude_step_no + 8) % 8;
    last_x_step_time = current_time;
    set_pin_output(altitude_pins, altitude_step_no);
  }
  // Move y motor one step further if it's time
  if ((y != 0) && (current_time > last_y_step_time + y_period_us)) {
    azimuth_step_no += y_dir;
    azimuth_step_no = (azimuth_step_no + 8) % 8;
    last_y_step_time = current_time;
    set_pin_output(azimuth_pins, azimuth_step_no);
  }
}

void go_to_position() {}

void track(){}

void set_pin_output(int *pin, int StepNo){
  // Map step number (0-8) to pin values
  switch(StepNo){
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
