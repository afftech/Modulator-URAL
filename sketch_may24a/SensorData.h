#include "wiring_analog.h"
#include "wiring_time.h"
#include "wiring_digital.h"

// один датчик лучше читать без адресации, это сильно экономит память
#include <OneWire.h>

OneWire ds(PB5);  // Объект OneWire

#include "Filter.h"
Filter FilterRPM(0.05);       //чем меньше коффициент тем плавнее регулировка
Filter FilterThrottle(0.05);  //чем меньше коффициент тем плавнее регулировка
class SensorData {
public:
  SensorData(char MapPin, char ThrottlePin, char OptoPint) {
    _MapPin = MapPin;
    _ThrottlePin = ThrottlePin;
    _OptoPint = OptoPint;
  }
  void run(bool begin) {
    runTempAir();
    /*работа с оптодатчиком*/
    runOpto();
    /////////////////////////
    if (!engineStateOn && !engineStateOn) {
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
  double getTempAir() {
    return TempAir;
  }
  void runTempAir() {
    byte data[2];
    ds.reset();
    ds.write(0xCC);
    ds.write(0x44);
    if (millis() - TimeGetTempAir >= 1000) {
      TimeGetTempAir = millis();
      ds.reset();
      ds.write(0xCC);
      ds.write(0xBE);
      data[0] = ds.read();
      data[1] = ds.read();
      // Формируем значение
      TempAir = (data[1] << 8) + data[0];
      TempAir = TempAir >> 4;
    }
  }
  double getTempEngine() {
    double value = -10;  //ReadTemp();
    return value;
  }
  double getThrottle() {
    double value = FilterThrottle.ClearingSignal(ReadThrottle());
    return value;
  }
  double getRpm() {
    if (millis() - TimeOldDataRpm >= 12000) {
      Rpm = 0;
    }
    Rpm = FilterRPM.ClearingSignal(Rpm);
    return Rpm;
  }
  void inputRpm(long TimeNewDataRpm) {
    if (TimeNewDataRpm != TimeOldDataRpm) {  //сколько оборотов в минуту
      double data;
      data = 60000.0 / ((TimeNewDataRpm - TimeOldDataRpm) / 1000);
      if (data < 100) {  // для того что бы в вмт искру дать
        Rpm = 0;
      } else {
        Rpm = data;
      }
      TimeOldDataRpm = TimeNewDataRpm;
      engineStateOn = true;
    }
    Serial.println(Rpm);
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
  bool getOpto() {
    return optoState;
  }
  void runOpto() {
    optoSignal = analogRead(_OptoPint);
    if (optoSignal > 900) {
      optoState = 1;
    } else if (optoSignal < 100) {
      optoState = 0;
    }
  }

private:
  long TimeOldDataRpm, TimeGetTempAir;
  double maxKpa, minKpa;
  double Rpm;
  bool engineStateOn;
  bool _begin;
  char _MapPin, _ThrottlePin, _OptoPint, _TempPin;
  int optoSignal;
  bool optoState = 0;
  int TempAir;
  int DacMinMap = 0, DacMaxMap = 1023, FactMinMap = 10, FactMaxMap = 118;
  int DacMinThrottle = 0, DacMaxThrottle = 800, FactMinThrottle = 0, FactMaxThrottle = 100;
  int DacMinTemp = 0, DacMaxTemp = 1023, FactMinTemp = -20, FactMaxTemp = 100;
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