/*
  Rui Santos
  Complete project details at our blog: https://RandomNerdTutorials.com/esp32-esp8266-firebase-bme280-rtdb/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <Wire.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "Fito"
#define WIFI_PASSWORD "hellothere"

// Insert Firebase project API Key
#define API_KEY "AIzaSyAJv351-MvKLacSnJZS4I4F6KZCUhFXTaA"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "Osvaldo.C@gmail.com"
#define USER_PASSWORD "GigaChad69420"

// Insert RTDB URLefine the RTDB URL
#define DATABASE_URL "https://smart-parking-system-fa518-default-rtdb.asia-southeast1.firebasedatabase.app/"

// Define Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable to save USER UID
String uid;

// Variables to save database paths
String databasePath;
String disPath;
String reservePath;

// LEDs

int ledRed = 26;
int ledGreen = 25;
int ledBlue = 27;
int db_distValue;

// Ultrasonic sensor

int TRIG_PIN = 13; // trigger pin
int ECHO_PIN = 12; // echo pin
long duration;
int distance;

// Timer variables (send new readings every three minutes)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 600;



// Initialize WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Write int values to the database
void sendInt(String path, int value){
  if (Firebase.RTDB.setInt(&fbdo, path.c_str(), value)){
    Serial.print("Writing value: ");
    Serial.print (value);
    Serial.print(" on the following path: ");
    Serial.println(path);
    Serial.println("PASSED");
    Serial.println("PATH: " + fbdo.dataPath());
    Serial.println("TYPE: " + fbdo.dataType());
  }
  else {
    Serial.println("FAILED");
    Serial.println("REASON: " + fbdo.errorReason());
  }
}

void setup(){
  Serial.begin(115200);
  initWiFi();
   
   pinMode(TRIG_PIN, OUTPUT);
   pinMode(ECHO_PIN, INPUT);
   pinMode(ledRed, OUTPUT);
   pinMode(ledBlue, OUTPUT);
   pinMode(ledGreen, OUTPUT);
  
  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Assign the maximum retry of token generation
  config.max_token_generation_retry = 5;

  // Initialize the library with the Firebase authen and config
  Firebase.begin(&config, &auth);

  // Getting the user UID might take a few seconds
  Serial.println("Getting User UID");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Print user UID
  uid = auth.token.uid.c_str();
  Serial.print("User UID: ");
  Serial.println(uid);

  // Update database path
  databasePath = "/UsersData/" + uid;

  // Update database path for sensor readings
  disPath = databasePath + "/distance"; // --> UsersData/<user_uid>/distance
  reservePath = databasePath + "/reservation_form"; // --> UsersData/<user_uid>/reservation_form
}

void loop(){
  // Send new readings to database
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    // Get latest sensor readings
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;

    // Send readings to database:
    sendInt(disPath, distance);
  
    // Read the current database readings;
    if (Firebase.RTDB.getInt(&fbdo, disPath)) {
      if (fbdo.dataType() == "int") {
        db_distValue = fbdo.intData();
    }
  }
  if (Firebase.RTDB.getJSON(&fbdo, reservePath) && (db_distValue >= 9)) {
    if (fbdo.dataType() == "json") {
      digitalWrite(ledBlue, HIGH);
      digitalWrite(ledGreen, LOW);
      digitalWrite(ledRed, LOW);
    }
   }
   else if (db_distValue < 9) {
      digitalWrite(ledBlue, LOW);
      digitalWrite(ledGreen, LOW);
      digitalWrite(ledRed, HIGH);
      Firebase.RTDB.deleteNode(&fbdo, reservePath);
    }
    
    else {
    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, HIGH);
    digitalWrite(ledBlue, LOW);
      }
  }
}
