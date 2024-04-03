#include "WSerial.h"
#include "HardwareSerial.h"
#include "wiring_constants.h"
#include "wiring_analog.h"
#include "wiring_time.h"
#include "wiring_digital.h"

// один датчик лучше читать без адресации, это сильно экономит память
#include <OneWire.h>

OneWire ds(PB5);  // Объект OneWire

#include "Filter.h"
Filter FilterRPM(0.5);        //чем меньше коффициент тем плавнее регулировка
Filter FilterThrottle(0.05);  //чем меньше коффициент тем плавнее регулировка
Filter FilterResistor(0.05);  //чем меньше коффициент тем плавнее регулировка
class SensorData {
public:
  SensorData(char MapPin, char ThrottlePin, char OptoPint) {
    _MapPin = MapPin;
    _ThrottlePin = ThrottlePin;
    _OptoPint = OptoPint;
  }
  void run(bool begin) {
    runVariableResistor();
    runTempAir();
    /*работа с оптодатчиком*/
    runOpto();
    /////////////////////////
    if (!engineStateOn) {
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
    if (millis() - TimeGetTempAir >= 10000) {
      TimeGetTempAir = millis();
      ds.reset();
      ds.write(0xCC);
      ds.write(0xBE);
      data[0] = ds.read();
      data[1] = ds.read();
      // Формируем значение
      TempAir = (data[1] << 8) + data[0];
      TempAir = TempAir >> 4;
      Serial.print("TempAir:");
      Serial.println(TempAir);
    }
  }
  double getTempEngine() {
    double value = -10;  //ReadTemp();
    return value;
  }
  double getThrottle() {
    double value = FilterThrottle.ClearingSignal(ReadThrottle());
    /*Serial.print("ThrottleState:");
    Serial.println(value);*/
    return value;
  }
  int getRpm() {
    /*<<<<<<< HEAD
    if (millis() - (TimeOldDataRpm / 1000) >= 1000) {
=======*/
    if (micros() - TimeOldDataRpm >= 1200000) {  // если меньше 3 оборотов в минуту то обнуляем
      Rpm = 0;
    }
    return Rpm;
  }
  void inputRpm(long TimeNewDataRpm) {
    if (TimeNewDataRpm != TimeOldDataRpm) {  //сколько оборотов в минуту
      int data;
      data = 60000.0 / ((TimeNewDataRpm - TimeOldDataRpm) / 1000);
      Rpm = data;
      TimeOldDataRpm = TimeNewDataRpm;
      engineStateOn = true;
    }
    //Serial.println(Rpm);
  }
  double getMap() {
    double value = 100;
    //return OldMinKpa;
    return value;
  }

  bool getOpto() {
    return optoState;
  }
  void runOpto() {
    optoSignal = analogRead(_OptoPint);
    if (optoSignal > 90) {
      optoState = 0;
    } else if (optoSignal <= 90) {
      optoState = 1;
    }
  }
  double mapValue(double value, double min1, double max1, double min2, double max2) {
    double scale = ((double)max2 - (double)min2) / ((double)max1 - (double)min1);
    return ((double)value - (double)min1) * (double)scale + (double)min2;
  }
  void runVariableResistor() {
    dataResistor = mapValue(FilterResistor.ClearingSignal(analogRead(Resistor)), 0, 1023, 0.3, 8);
    //dataResistor = mapValue(FilterResistor.ClearingSignal(analogRead(Resistor)), 0, 1023, 0.1, 2.5);
  }
  double getVariableResistor() {
    return dataResistor;
  }

private:
  double dataResistor;
  long TimeOldDataRpm, TimeGetTempAir;
  double maxKpa, minKpa;
  int Rpm;
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