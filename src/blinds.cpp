#include "Arduino.h"
#include "blinds.h"

Blinds::Blinds() {}

Blinds::Blinds(String name, uint8_t servo_pin, uint8_t initial_state, uint8_t step_time)
{
  this->name = name;
  this->servo.attach(servo_pin);
  this->servo.write(90);
  Serial.println("");
  Serial.printf("[%s] Attach to servo on pin%d\n", this->name.c_str(), servo_pin);
  this->state = initial_state;
  this->step_time = step_time;
}

Blinds::~Blinds()
{
  this->servo.detach();
}

int Blinds::getState()
{
  return this->state;
}

void Blinds::calibrate(uint8_t percentage)
{
  Serial.printf("[%s] Calibrate to %d%%\n", this->name.c_str(), percentage);
  this->state = percentage;
}

void Blinds::moveTo(uint8_t percentage)
{
  percentage = constrain(percentage, 0, 100);
  Serial.printf("[%s] Moving from %d%% to %d%%\n", this->name.c_str(), this->state, percentage);

  int motion = percentage - this->state;
  if (motion > 0) // down
  {
    this->servo.write(180);
  }
  else if (motion < 0) // up
  {
    this->servo.write(0);
  }
  Serial.println(abs(motion) * this->step_time);
  delay(abs(motion) * this->step_time);
  this->servo.write(90);
  this->state = percentage;
}
