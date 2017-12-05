#ifndef LAB_15_H
#define LAB_15_H

#include <Wire.h>
#include <WiFi.h>
#include <aREST.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

//  debugging
#define dbg(b, s) if(b) Serial.print(s)
#define dln(b, s) if(b) Serial.println(s)
#define printSerial  true
bool printedOTAMessage = false;

//  server
# define LISTEN_PORT    80

//  motor controls
# define leftMotorPort  3
# define rightMotorPort 4 
int speed = 50;

const char* ssid =      "James Phone AP";
const char* password =  "jameswebb";

//  ultrasonic sensors
const int echoPinSide = 34;
const int trigPinSide = 25;
const int echoPinFront = 39;
const int trigPinFront = 26;
long durationSide, durationFront;
int distanceSide, distanceFront;

//  Motor Functions
int stop(String message);
int forward(String message);
int right(String message);
int left(String message);
int backward(String message);

#endif

