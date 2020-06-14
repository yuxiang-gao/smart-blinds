#ifndef BLINDS_H
#define BLINDS_H
#include <Arduino.h>
#include <Servo.h>

class Blinds
{
private:
  uint8_t step_time; // time to move 1%
  String name;
  Servo servo;
  uint8_t state; // percentage

public:
  Blinds();
  Blinds(String name, uint8_t servo_pin, uint8_t initial_state = 0, uint8_t step_time = 60);
  ~Blinds();
  int getState();
  void moveTo(uint8_t desired_state);
  void calibrate(uint8_t state);
};
#endif
