/*
  Copyright 2023 Tauno Erik
*/
#include <Arduino.h>
#include "Unistep2.h"        // Stepper motor
#include "Radar_MR24HPC1.h"  // Radar

// Timing
uint64_t prev_millis = 0;  // unsigned long
#define RADAR_INTERVAL 400
bool ask_radar = false;

// STEPPER MOTOR
// pins
const int IN1 = 2;
const int IN2 = 3;
const int IN3 = 4;
const int IN4 = 5;

const int STEPS_PER_REV = 4096;

// Pins for IN1, IN2, IN3, IN4, steps per rev, step delay(in micros)
Unistep2 stepper(IN1, IN2, IN3, IN4, STEPS_PER_REV, 1000);

int step = 0;  // pos, neg or 0


// RADAR
Radar_MR24HPC1 radar = Radar_MR24HPC1(&Serial1);

int heartbeat = 0;

void setup() {
  Serial.begin(115200);   // Serial print
  Serial1.begin(115200);  // Radar

  while (!Serial1) {
    Serial.println("Radar disconnected");
    delay(100);
  }
  Serial.println("Radar ready");

  // radar.set_mode(SIMPLE);
  radar.set_mode(ADVANCED);
  // radar.set_static_limit(RANGE_300_CM);
}


void loop() {
  uint64_t current_millis = millis();

  if ((current_millis - prev_millis) >= RADAR_INTERVAL) {
    ask_radar = true;
    prev_millis = current_millis;
  }


  stepper.run();
  stepper.move(step);

  //radar.run(VERBAL);
  radar.run(NONVERBAL);

  if (ask_radar) {
    ask_radar = false;

    Serial.print("Motion energi: ");
    Serial.println(radar.get_motion_energy());

    if (radar.get_motion_energy() > 50) {
      step = 1;
    } else {
      step = 0;
    }


  }
}  // main loop end

void loop1() {
  // core2
}
