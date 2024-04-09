#include "wiring_time.h"
class Button {
public:
  Button(char pin, int BtnMode, int SizeTimeH) {
    _pin = pin;
    _BtnMode = BtnMode;
    pinMode(_pin, INPUT);
    _SizeTimeH = SizeTimeH;
  }
  void check() {
    int dataBtn = digitalRead(_pin);
    if (dataBtn) {
      _flag = true;
    } else {
      _flag = false;
    }
  }
  bool click() {
    if (_BtnMode) {
      if (!holdStop && _flag && !BtnState && millis() - TimerClick1 >= BtnGroupTime1) {
        BtnState = true;
        TimerClick1 = millis();
        holdOn = true;
      }
      if (!holdStop && _flag && BtnState && millis() - TimerClick1 >= BtnGroupTime3) {
        TimerClick1 = millis();
        holdClick2 = true;
        return false;
      }
      if (!_flag && BtnState) {
        if (holdOn && millis() - TimerClick1 < BtnGroupTime2) {
          holdOn = false;
          BtnState = false;
          return true;
        }
        if (holdOn && millis() - TimerClick1 >= BtnGroupTime2) {
          holdClick1 = true;
          BtnState = false;
          return false;
        }
        if (!_flag) {
          holdStop = false;
        }
        BtnState = false;
        TimerClick1 = millis();
        return false;
      }
      return false;
    } else {
      if (!SensorState && _flag && millis() - TimerHold1 >= _SizeTimeH) {
        TimerHold1 = millis();
        SensorState = true;
      } else if (!_flag) {
        TimerHold1 = millis();
      }
      if (SensorState && !_flag && millis() - TimerHold2 >= _SizeTimeH) {
        TimerHold2 = millis();
        SensorState = false;
      } else if (_flag) {
        TimerHold2 = millis();
      }
      if (SensorState) {
        return true;
      }
      return false;
    }
  }

  bool
  hold1() {
    if (holdClick1) {
      holdClick1 = false;
      holdOn = false;
      return true;
    }
    return false;
  }
  bool hold2() {
    if (holdClick2 && !holdStop) {
      holdStop = true;
      holdClick2 = false;
      holdOn = false;
      return true;
    }
    return false;
  }

  bool stateBtn() {
    if (_flag) {
      return true;
    }
    return false;
  }
private:
  bool holdOn, holdClick1, holdClick2, holdStop;
  bool _flag, BtnState, SensorState;
  int _BtnMode;
  char _pin;
  int _SizeTimeH;
  unsigned long TimerClick1, TimerSensor, TimerHold1, TimerHold2;
};