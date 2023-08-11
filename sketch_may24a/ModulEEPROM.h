#include <EEPROM.h>
float dataFuelThrottle[] = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };  // каждые 500 к RPM 40 байт
class ModulEEPROM {
public:
  void begin() {  //старт
    EEPROM.get(0, dataFuelThrottle);
    for (int i = 0; i < 10; i++) {
      if (i < 9) {
        Serial.print(dataFuelThrottle[i]);
        Serial.print(",");
      }
      if (i == 9) {
        Serial.println(dataFuelThrottle[i]);
      }
      if (dataFuelThrottle[i] < 0.2 || dataFuelThrottle[i] > 2.5)
        dataFuelThrottle[i] = 1;
    }
  }
  void updateThrottle(double rpm) {  // в цикле изменение
    whereRpm(rpm);
    //timer_Save();//Временно отключем сохранение
    newData = round((float)SensorData.getVariableResistor() * 10) / 10;  //jrheu
    if (newData != oldData) {
      dataFuelThrottle[flag] = newData;
      oldData = newData;
      for (int i = 0; i < 10; i++) {
        if (i < 9) {
          Serial.print(dataFuelThrottle[i]);
          Serial.print(",");
        }
        if (i == 9) {
          Serial.println(dataFuelThrottle[i]);
        }
      }
    }
  }
  void saveThrottle() {  // для сохранения
    float dataEEPROM[10];
    bool err = 0;
    EEPROM.get(0, dataEEPROM);
    for (int i = 0; i < 10; i++) {
      if (!(dataFuelThrottle[i] == dataEEPROM[i])) {
        err = 1;
      }
    }
    if (err) {
      EEPROM.put(0, dataFuelThrottle);
    }
  }

  float getThrottle() {  // для получения установленного значения
    return dataFuelThrottle[flag];
  }
  int whereRpm(float rpm) {                           // для установки флага
    flag = SensorData.mapValue(rpm, 0, 5000, 0, 10);  //от 0 - 9
    if (flag == 10) { flag = 9; }
    return flag;
  }
private:
  int JT = 0;
  float newData, oldData;
  long Timer;
  int flag;
  bool timer_Save() {
    if (millis() - Timer >= 1000) {
      Timer = millis();
      JT++;
      //Serial.println(JT);
      if (JT > 30) {
        saveThrottle();
        JT = 0;
        return true;
      }
    }
    return false;
  }
};