#ifndef BLINDS_H
#define BLINDS_H
#include <Arduino.h>
#include <Servo.h>

class Blinds
{
private:
  static const int step_time = 70;
  //    int state_list[5] = {-2, -1, 0, 1, 2};
  String name;
  Servo servo;

public:
  int state;
  Blinds();
  Blinds(String name, int servo_pin);
  ~Blinds();
  void moveTo(int desired_state);
  void calibrate(int percentage);
};
#endif
