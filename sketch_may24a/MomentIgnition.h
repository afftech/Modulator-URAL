#include "wiring_time.h"
#include "wiring_digital.h"

class MomentIgnition {
public:
  MomentIgnition(char Pin) {
    _Pin = Pin;
    pinMode(_Pin, OUTPUT);
  }
  void run(double rpm) {
    if (Trigger) {
      if (Moment(rpm)) {
        Trigger = false;
        //digitalWrite(_Pin, false);
      }
    }
  }
  bool Moment(double rpm ) {
    if (!TriggerMomentIgnition) {
      TriggerMomentIgnition = true;
      
      _time = ((timerMZ - _time) / 330) * InitialValueMZ; 
      //Serial.println(_time);
    }
    if (TriggerMomentIgnition && micros() - timerMZ >= _time) {
      Serial.println(micros() - _timeVMT);
      digitalWrite(_Pin, true);
      TriggerMomentIgnition = false;
      //Serial.println(micros() - test);
      return true;
    }
    return false;
  }
  bool on(long time) {  //момент зажигания  2mc
    test = micros();
    _timeVMT = time;
    _time = time;  //Время почледней позиции в МТ
    Trigger = true;
    timerMZ = micros();
    //Serial.println(timerMZ - _time);

    return true;
  }


  long _time, _timeVMT, test;
  bool Trigger;
  long timerMZ;
  long InitialValueMZ = 30;  //опережение зажигание 20 раннее (5-45 гр. до вмт)
  bool TriggerMomentIgnition;
  char _Pin;
};