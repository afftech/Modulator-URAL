#include "wiring_time.h"
#include "wiring_digital.h"

class SensorData {
public:
  SensorData(char MapPin, char TempPin, char ThrottlePin) {
    _MapPin = MapPin;
    _ThrottlePin = ThrottlePin;
    _TempPin = TempPin;
  }
  void run(bool _engineState, bool begin) {
    if (!_engineState && !engineStateOn) {
      delay(200);
      maxKpa = ReadMap();
      minKpa = maxKpa;
      engineStateOn = true;
    }
    if (ReadThrottle() == 100) {
      maxKpa = ReadMap();
      minKpa = maxKpa;
    } else {
      if (begin) {
        if (minKpa = maxKpa) {
          _begin = false;
        }
        if (ReadMap() < minKpa) {
          minKpa = ReadMap();
        }
      } else {
        if (!_begin) {
          OldMinKpa = minKpa;
          _begin = true;
        }
        minKpa = maxKpa;
      }
    }
  }
  double getTemp() {
    double value = -10;  //ReadTemp();
    return value;
  }
  double getTempEngine() {
    double value = -10;  //ReadTemp();
    return value;
  }
  double getThrottle() {
    double value = 15;  //ReadThrottle();
    return value;
  }
  double getMap() {
    double value = 100;
    //return OldMinKpa;
    return value;
  }
  double mapValue(double value, double min1, double max1, double min2, double max2) {
    double scale = (max2 - min2) / (max1 - min1);
    return (value - min1) * scale + min2;
  }

private:

  double maxKpa, minKpa;
  bool engineStateOn;
  bool _begin;
  char _MapPin, _ThrottlePin, _TempPin;
  int DacMinMap = 0, DacMaxMap = 4048, FactMinMap = 10, FactMaxMap = 118;
  int DacMinThrottle = 0, DacMaxThrottle = 4048, FactMinThrottle = 0, FactMaxThrottle = 100;
  int DacMinTemp = 0, DacMaxTemp = 4048, FactMinTemp = -20, FactMaxTemp = 100;
  int OldMinKpa;

  double ReadMap() {
    return mapValue(analogRead(_MapPin), DacMinMap, DacMaxMap, FactMinMap, FactMaxMap);
  }
  double ReadThrottle() {
    return mapValue(analogRead(_ThrottlePin), DacMinThrottle, DacMaxThrottle, FactMinThrottle, FactMaxThrottle);
  }
  double ReadTemp() {
    return mapValue(analogRead(_TempPin), DacMinTemp, DacMaxTemp, FactMinTemp, FactMaxTemp);
  }
};