#include "wiring_time.h"
#include "wiring_digital.h"

class MomentIgnition {
public:
  MomentIgnition(char Pin) {
    _Pin = Pin;
    pinMode(_Pin, OUTPUT);
  }
  void run(double rpm) {
    if (WithoutAnAngleOn) {  // Снимаем сигнал пуска искры
      if (micros() - WithoutAnAngleTime >= 5000) {
        digitalWrite(_Pin, true);
        WithoutAnAngleOn = false;
      }
    }
    if (Trigger) {
      if (rpm < 300) {  //Опережение в зависимости от оборотов дв
        if (Moment(5)) {
          Trigger = false;
        }
        /*if (Moment(SensorData.getVariableResistor())) {
        Trigger = false;
        Serial.print("Moment(");
        Serial.print(SensorData.getVariableResistor());
        Serial.println(")");
      } */
      }
      if (rpm >= 300 && rpm < 1800) {  //4-9
        if (Moment(10)) {
          Trigger = false;
        }
      }
      if (rpm >= 1800 && rpm < 2500) {  //13
        if (Moment(11)) {
          Trigger = false;
        }
      }
      if (rpm >= 2500 && rpm < 3000) {  //15
        if (Moment(12)) {
          Trigger = false;
        }
      }
      if (rpm >= 3000 && rpm < 3500) {  //20
        if (Moment(14)) {
          Trigger = false;
        }
      }
      if (rpm >= 3500 && rpm < 4000) {
         if (Moment(17)) {
          Trigger = false;
        }
      }
      if (rpm >= 4000) {  //нельзя делать меняше InitialValueMZ //32
        if (Moment(19)) {
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
    advanceNew = advance;
    if (!TriggerMomentIgnition) {
      TriggerMomentIgnition = true;
      //Serial.println(timerMZ - _time);
      _time = ((timerMZ - _time) / 150) * (InitialValueMZ - advance);
      if (_time < 0) {
        _time = 500;
      }
      //Serial.println(_time);
    }
    if (TriggerMomentIgnition && micros() - timerMZ >= _time) {
      //Serial.println(micros() - _timeVMT);
      digitalWrite(_Pin, false);  // пуск искры
      WithoutAnAngleOn = true;
      WithoutAnAngleTime = micros();
      TriggerMomentIgnition = false;
      //Serial.println(micros() - test);
      //InitialValueMZ = 58;
      return true;
    } else if (TriggerMomentIgnition && _time == 0) {
      digitalWrite(_Pin, false);  // пуск искры
      WithoutAnAngleOn = true;
      WithoutAnAngleTime = micros();
      TriggerMomentIgnition = false;
    }
    //InitialValueMZ = 58;
    return false;
  }
  bool on(long time, bool OnStart) {  //момент зажигания  2mc
    if (OnStart) {
      //test = micros();
      _timeVMT = time;
      _time = time;  //Время поcледней позиции в МТ
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
    digitalWrite(_Pin, false);  // пуск искры
    return true;
  }
  void log(long time) {
    long test1, test2;
    test1 = (time - _timeVMT) / 360;                //фактическре время прохождения 360
    test1 = test1 * (InitialValueMZ - advanceNew);  //здесь мы узнали правильный момент угла
    test2 = _time;                                  //теоритическое время прохождения 360
    if (test1 > test2) {
      Serial.print("AngleFactMore:");
      Serial.println((test1 - test2));
      Serial.println(test1);
      Serial.println(test2);
      //correct = test1 / test2;
    } else if (test1 < test2) {
      Serial.print("AngleTeorMore:");
      Serial.println((test2 - test1));
    }
    //Serial.println("Angle:");
  }
  int advanceNew;
  long _time, _timeVMT, test;
  bool Trigger;
  long timerMZ, WithoutAnAngleTime, WithoutAnAngleOn;
  int InitialValueMZ = 30;  //опережение зажигание 20 раннее (5-20 гр. до вмт)
  bool TriggerMomentIgnition;
  char _Pin;
  double correct = 1;
};