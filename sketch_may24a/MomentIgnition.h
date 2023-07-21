#include "wiring_time.h"
#include "wiring_digital.h"

class MomentIgnition {
public:
  MomentIgnition(char Pin) {
    _Pin = Pin;
    pinMode(_Pin, OUTPUT);
  }
  void run(double rpm) {
    if (WithoutAnAngleOn && micros() - WithoutAnAngleTime >= 1000) {
      digitalWrite(_Pin, false);  // Снимаем сигнал пуска
    }
    if (Trigger) {
      if (rpm < 300) {  //Опережение в зависимости от оборотов дв
        if (Moment(0)) {
          Trigger = false;
        }
      }
      if (rpm >= 300 && rpm < 1800) {
        if (Moment(6)) {
          Trigger = false;
        }
      }
      if (rpm >= 1800 && rpm < 2500) {
        if (Moment(10)) {
          Trigger = false;
        }
      }
      if (rpm >= 2500 && rpm < 3000) {
        if (Moment(12)) {
          Trigger = false;
        }
      }
      if (rpm >= 3000 && rpm < 3500) {
        if (Moment(18)) {
          Trigger = false;
        }
      }
      if (rpm >= 3500 && rpm < 4000) {
        if (Moment(26)) {
          Trigger = false;
        }
      }
      if (rpm >= 4000) {
        if (Moment(32)) {
          Trigger = false;
        }
      }
      //Serial.println(rpm);
    }
    if (rpm == 0) {  //сброс состояний если 0 оборотов
      timerMZ = micros();
      Trigger = false;
      TriggerMomentIgnition = false;
    }
  }

  bool Moment(int advance) {
    if (!TriggerMomentIgnition) {
      TriggerMomentIgnition = true;
      _time = ((timerMZ - _time) / 330) * (InitialValueMZ - advance);
      Serial.println(_time);
    }
    if (TriggerMomentIgnition && micros() - timerMZ >= _time) {
      //Serial.println(micros() - _timeVMT);
      digitalWrite(_Pin, true);  // пуск искры
      TriggerMomentIgnition = false;
      //Serial.println(micros() - test);
      InitialValueMZ = 30;
      return true;
    }
    InitialValueMZ = 30;
    return false;
  }
  bool on(long time, bool OnStart) {  //момент зажигания  2mc
    if (OnStart) {
      //test = micros();
      _timeVMT = time;
      _time = time;  //Время почледней позиции в МТ
      Trigger = true;
      timerMZ = micros();
      //Serial.println(timerMZ - _time);
      return true;
    } else {
      WithoutAnAngle();
      return true;
    }
  }

  bool WithoutAnAngle() {
    WithoutAnAngleOn = true;
    WithoutAnAngleTime = micros();
    digitalWrite(_Pin, true);  // пуск искры
    return true;
  }
  long _time, _timeVMT, test;
  bool Trigger;
  long timerMZ;
  long InitialValueMZ = 30;  //опережение зажигание 20 раннее (5-45 гр. до вмт)
  bool TriggerMomentIgnition;
  char _Pin;
};