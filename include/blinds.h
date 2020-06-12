#ifndef BLINDS_H
#define BLINDS_H
#include <Arduino.h>
#include <Servo.h>

class blinds
{
private:
  static const int step_time = 70;
  //    int state_list[5] = {-2, -1, 0, 1, 2};
  String name;
  Servo servo;

public:
  int state;
  blinds();
  blinds(String name, int servo_pin);
  ~blinds();
  void move_to(int desired_state);
  void calibrate(int percentage);
};
#endif
