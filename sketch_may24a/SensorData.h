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
Filter FilterMap(0.1);        //чем меньше коффициент тем плавнее регулировка
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
    Map = ReadMap();
    Throttle = ReadThrottle();
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

    if (millis() - TimeGetTempAir >= 5000) {
      ds.reset();
      ds.write(0xCC);
      ds.write(0x44);
      /****************************/
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
  int getRpm() {
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
    }
  }
  double getFuel() {
    bool AddF;
    if (Map > 80) {
      AddF = true;
    }

    return mapValue(Throttle, 0, 100, getVariableResistor() + AddF, 8 + getVariableResistor());
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
    dataResistor = mapValue(FilterResistor.ClearingSignal(analogRead(Resistor)), 0, 1023, 0.1, 2.5);
  }
  double getVariableResistor() {
    return dataResistor;
  }

private:
  double dataResistor;
  long TimeOldDataRpm, TimeGetTempAir, TimeFindMap;
  double maxKpa, minKpa;
  int Rpm;
  char _MapPin, _ThrottlePin, _OptoPint, _TempPin;
  int optoSignal;
  bool optoState = 0;
  int TempAir;
  int DacMinMap = 0, DacMaxMap = 455, FactMinMap = 0, FactMaxMap = 118, Map;
  int DacMinThrottle = 500, DacMaxThrottle = 500 /*800*/, FactMinThrottle = 1, FactMaxThrottle = 100, Throttle;
  int DacMinTemp = 0, DacMaxTemp = 1023, FactMinTemp = -20, FactMaxTemp = 100;

  int ReadMap() {
    int data = analogRead(_MapPin);  //291-74.295-75.309-80
    return FilterMap.ClearingSignal(mapValue(data, DacMinMap, DacMaxMap, FactMinMap, FactMaxMap));
  }
  double ReadThrottle() {
    int data = FilterThrottle.ClearingSignal(analogRead(_ThrottlePin));
    if (data > DacMinThrottle) { DacMinThrottle = data; }
    if (data < DacMaxThrottle) { DacMaxThrottle = data; }
    return mapValue(data, DacMinThrottle, DacMaxThrottle, FactMinThrottle, FactMaxThrottle);
  }
  double ReadTemp() {
    return mapValue(analogRead(_TempPin), DacMinTemp, DacMaxTemp, FactMinTemp, FactMaxTemp);
  }
};