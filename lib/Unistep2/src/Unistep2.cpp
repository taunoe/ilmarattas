/*
  Copyright 2018-2023
  UniStep2.cpp - Non-blocking Library for unipolar steppers.
  Modified by Tauno Erik , 30.09.2023.
  Created by Robert Sanchez, January 28, 2018.
  Based on Unistep by Matthew Jones.
  Released into the public domain.

  This library tracks the phase and step of unipolar 5 wire stepper motors
  (28BYJ-48) in a non blocking way.
*/

#include "Arduino.h"
#include "Unistep2.h"

Unistep2::Unistep2(int _pin1, int _pin2, int _pin3, int _pin4,
                   int _steps, uint64_t _stepdelay) {
  pin1 = _pin1;
  pin2 = _pin2;
  pin3 = _pin3;
  pin4 = _pin4;

  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);

  phase = 0;
  steptime = _stepdelay;
  stepsperrev = _steps;
  currentstep = 0;
  stepstogo = 0;
}

// run() should be computationally cheap if false. Just check if we have to move
// (maybe checking objective vs current step? Or with a flag?) and send to the
// function. Or else return quickly so that we don't stall the loop.
bool Unistep2::run() {
  if (!stepstogo) {  // will be true if zero
    return true;
  } else {
    nextStep();
  }
}

// Called because we still have to move a stepper. Do a time check to see if
// the next step is due and determine the sequence of phases that we need.
void Unistep2::nextStep() {
  uint64_t time = micros();

  if (time - _lastStepTime >= steptime) {
    // clockwise
    if (stepstogo > 0) {
      stepCW();
      stepstogo--;
      if (stepstogo == 0) stop();  // set pins low. Less humm and less energy.
    // counter-clockwise
    } else {
      stepCCW();
      stepstogo++;
      if (stepstogo == 0) stop();  // set pins low. Less humm and less energy.
    }
    _lastStepTime = time;
  }
}

// Setup a movement. Set stepstogo.
void Unistep2::move(int steps) {
  power_up();
  stepstogo = steps;
}

// Setup a movement to position. Calculate and set stepstogo.
void Unistep2::move_to(unsigned int pos) {
  power_up();
  stepstogo = pos - currentstep;
  if (abs(stepstogo) > stepsperrev / 2) {
    stepstogo = (stepstogo > 0 ) ? stepstogo - stepsperrev : stepstogo + stepsperrev;
  }
}

// Inherits phase, calls for clockwise movement phase sequence
void Unistep2::stepCW() {
  // gofromthisphase
  switch (phase) {
    case 0:
      goto7();
      break;
    case 1:
      goto0();
      break;
    case 2:
      goto1();
      break;
    case 3:
      goto2();
      break;
    case 4:
      goto3();
      break;
    case 5:
      goto4();
      break;
    case 6:
      goto5();
      break;
    case 7:
      goto6();
      break;
    default:
      goto0();
      break;
  }

  currentstep++;
  if (currentstep == stepsperrev) {
    currentstep = 0;
  }
}

// Inherits phase, calls for counter-clockwise movement phase sequence
void Unistep2::stepCCW() {
  // gofromthisphase
  switch (phase) {
    case 0:
      goto1();
      break;
    case 1:
      goto2();
      break;
    case 2:
      goto3();
      break;
    case 3:
      goto4();
      break;
    case 4:
      goto5();
      break;
    case 5:
      goto6();
      break;
    case 6:
      goto7();
      break;
    case 7:
      goto0();
      break;
    default:
      goto0();
      break;
  }

  currentstep--;
  if (currentstep < 0) {
    currentstep = stepsperrev - 1;
  }
}

// individual steps
void Unistep2::goto1() {
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, HIGH);
  phase = 1;
}

void Unistep2::goto2() {
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, HIGH);
  digitalWrite(pin4, HIGH);
  phase = 2;
}

void Unistep2::goto3() {
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, HIGH);
  digitalWrite(pin4, LOW);
  phase = 3;
}

void Unistep2::goto4() {
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, HIGH);
  digitalWrite(pin3, HIGH);
  digitalWrite(pin4, LOW);
  phase = 4;
}

void Unistep2::goto5() {
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, HIGH);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
  phase = 5;
}

void Unistep2::goto6() {
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, HIGH);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
  phase = 6;
}

void Unistep2::goto7() {
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
  phase = 7;
}

void Unistep2::goto0() {
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, HIGH);
  phase = 0;
}

int Unistep2::current_position() {
  return currentstep;
}

int Unistep2::steps_to_go() {
  return stepstogo;
}

// Pulls pins low to save power and avoid heat build up when not moving.
void Unistep2::stop() {
  stepstogo = 0;
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  digitalWrite(pin3, LOW);
  digitalWrite(pin4, LOW);
}

// Powers pins up again at current phase to get ready to move. This prevents
// stalling and improves the response to CCW movement, allowing faster speeds
// (lower step delays)
void Unistep2::power_up() {
  switch (phase) {
    case 0:
      goto0();
      break;
    case 1:
      goto1();
      break;
    case 2:
      goto2();
      break;
    case 3:
      goto3();
      break;
    case 4:
      goto4();
      break;
    case 5:
      goto5();
      break;
    case 6:
      goto6();
      break;
    case 7:
      goto7();
      break;
    default:
      goto0();
      break;
  }
}
