/*
  Copyright 2018-2023
  UniStep2.h - Non-blocking Library for unipolar steppers.
  Modified by Tauno Erik , 30.09.2023.
  Created by Robert Sanchez, January 28, 2018.
  Based on Unistep by Matthew Jones.
  Released into the public domain.

  This library tracks the phase and step of unipolar 5 wire stepper motors
  (28BYJ-48) in a non blocking way.
*/

#ifndef LIB_UNISTEP2_SRC_UNISTEP2_H_
#define Unistep2_h

#include "Arduino.h"

class Unistep2 {
 public:
  // Constructor. We define pins for In1, In2, In3 and In4 on a ULN2003AN driver
  // plus the number of steps per rev (which is usually 4096) and the delay
  // between steps (900 works well).
  Unistep2(int _pin1, int _pin2, int _pin3, int _pin4,
           int _steps, uint64_t _stepdelay);

  // Must call this as frequently as possible,
  // but at least once per minimum step interval,
  // preferably in your main loop.
  // Return true if the motor is at the target position.
  bool run();

  // Move function. We define the number of steps (int) and set stepstogo
  // accordingly.
  // steps > 0 is clockwise
  // steps < 0 is counterclockwise
  void move(int steps);

  // We define the absolute position (between 0 and _steps)
  // and set stepstogo to get there. Will choose shortest path, so direction
  // will depend on current position.
  void move_to(unsigned int pos);

  // Return the current step of the motor.
  int current_position();

  // Return the number of steps between the current position to the target
  // position. Positive is clockwise. If 0 the motor is not moving.
  int steps_to_go();

  // Sets a new target position that causes the stepper to stop as quickly as
  // possible and pulls pins low.
  void stop();

 private:
  int phase;
  int pin1;
  int pin2;
  int pin3;
  int pin4;
  int stepsperrev;         // the number of steps in one ref zero indexed.
  int currentstep;         // the current step number, zero indexed.
  int stepstogo;           // the remaining steps to complete the curret movement.
  uint64_t steptime;       // the delay time between steps
  uint64_t _lastStepTime;  // the last step time in microseconds

  void nextStep();   // Called if there are stepstogo (!= 0)
  void stepCW();
  void stepCCW();
  void goto1();
  void goto2();
  void goto3();
  void goto4();
  void goto5();
  void goto6();
  void goto7();
  void goto0();
  void power_up();  // powers pins at current step to get ready for move
};

#endif  // LIB_UNISTEP2_SRC_UNISTEP2_H_
