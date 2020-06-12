#include "Arduino.h"
#include "blinds.h"

blinds::blinds() {}

blinds::blinds(String name, int servo_pin)
{
  this->name = name;
  this->servo.attach(servo_pin);
  Serial.println("");
  Serial.print("Attach to servo on pin");
  Serial.println(servo_pin);
  this->state = 0;
}

blinds::~blinds()
{
  this->servo.detach();
}

void blinds::calibrate(int percentage)
{
  Serial.printf("[blindS] Calibrate to %d%%\n", percentage);
  this->state = percentage;
}

void blinds::move_to(int percentage)
{
  percentage = constrain(percentage, 0, 100);
  Serial.printf("[blindS] %s moving from %d%% to %d%%\n", this->name.c_str(), this->state, percentage);

  int motion = percentage - this->state;
  if (motion > 0) // down
  {
    this->servo.write(180);
  }
  else if (motion < 0) // up
  {
    this->servo.write(0);
  }
  else
  {
    Serial.print("Already at ");
    Serial.println(percentage);
    return;
  }
  Serial.println(abs(motion) * this->step_time);
  delay(abs(motion) * this->step_time);
  this->servo.write(90);
  this->state = percentage;
}
