/*
  Copyright 2023 Tauno Erik
  main.cpp
  "Ilmaratas" -meditatsioono masin!
*/
#include <Arduino.h>
#include "Unistep2.h"        // Stepper motor
#include "Radar_MR24HPC1.h"  // Radar

// If Raspberry Pi Pico
#if defined Serial1
  // TX GP0
  // RX GP1
  Radar_MR24HPC1 radar = Radar_MR24HPC1(&Serial1);
#else
// Arduino Nano
  #include <SoftwareSerial.h>
  // Choose any two pins that can be used with SoftwareSerial to RX & TX
  #define RX_Pin A3
  #define TX_Pin A2
  SoftwareSerial mySerial = SoftwareSerial(RX_Pin, TX_Pin);
  Radar_MR24HPC1 radar = Radar_MR24HPC1(&mySerial);
#endif

// Timing
uint64_t prev_millis = 0;
const int RADAR_INTERVAL =  400;  // ms
bool ask_radar = false;

// STEPPER MOTOR
const int IN1_Pin = 5;
const int IN2_Pin = 4;
const int IN3_Pin = 3;
const int IN4_Pin = 2;

const int STEPS_PER_REV = 4096;

// Pins for IN1, IN2, IN3, IN4, steps per rev, step delay(in micros)
Unistep2 stepper(IN1_Pin, IN2_Pin, IN3_Pin, IN4_Pin, STEPS_PER_REV, 900);
int step = 0;  // pos, neg or 0

// Radar
const int MOTION_ENERGY_THRESHOLD = 20;
const int STATIC_ENERGY_THRESHOLD = 140;
const int STATIC_DISTANSE_THRESHOLD = 150;

void setup() {
  Serial.begin(115200);   // Serial print

  // Radar
  #if defined Serial1
    Serial1.begin(115200);
    while (!Serial1) {
      Serial.println("Radar disconnected");
      delay(100);
    }
  #else
    mySerial.begin(115200);
    while (!mySerial) {
      Serial.println("Radar disconnected");
      delay(100);
    }
  #endif

  Serial.println("Radar ready");

  // radar.set_mode(SIMPLE);
  radar.set_mode(ADVANCED);
  // radar.set_static_limit(RANGE_300_CM);
}


void loop() {
  uint64_t current_millis = millis();

  stepper.run();
  // radar.run(VERBAL);
  radar.run(NONVERBAL);

  if ((current_millis - prev_millis) >= RADAR_INTERVAL) {
    ask_radar = true;
    prev_millis = current_millis;
  }

  stepper.move(step);

  if (ask_radar) {
    ask_radar = false;

    int motion_energy = radar.get_motion_energy();
    int static_energy = radar.get_static_energy();
    int static_distance = radar.get_static_distance();
    int motion_distance = radar.get_motion_distance();

    // Serial.print(counter);
    Serial.print("motion ");
    Serial.print(motion_energy);
    Serial.print(" distance ");
    Serial.print(motion_distance);
    Serial.print("; static ");
    Serial.print(static_energy);
    Serial.print(" distance ");
    Serial.print(static_distance);
    Serial.println();

    // TODO: kaugus!
    if (static_energy > STATIC_ENERGY_THRESHOLD) {
      if (motion_energy < MOTION_ENERGY_THRESHOLD) {
        if (static_distance <= STATIC_DISTANSE_THRESHOLD) {
          step = 1;  // move
        }
      }
    } else {
      step = 0;
    }
  }
}

void loop1() {
  // core2
}
