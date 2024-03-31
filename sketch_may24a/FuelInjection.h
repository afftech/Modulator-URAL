#include "WSerial.h"
#include "wiring_time.h"
#include "wiring_digital.h"

class FuelInjection {
public:
  FuelInjection(int CylinderVolume, double Pressure, char pin) {
    pinMode(pin, OUTPUT);
    _pin = pin;
    pinMode(pin, OUTPUT);
    _CylinderVolume = CylinderVolume;
    _Pressure = Pressure;
  }
  void run() {
    if (DurationOpen != 0) {
      digitalWrite(_pin, true);
      if (micros() - _Tmr >= (DurationOpen * 1000)) {
        digitalWrite(_pin, false);
        //Serial.println(micros() - _Tmr);
        DurationOpen = 0;
      }
    }
  }
  bool AddFuel(double p, double tempA, double load) {  //давление воздуха, температура, загрузка
    int mass;
    AirTempK = tempKelvin(tempA);
    massFuel = CalculationAirM(p, MolM, GasConstant, AirTempK);
    FuelVolume = calculateFuelVolume(_CylinderVolume, massFuel, modulEEPROM.getThrottle());
    DurationOpen = SensorData.getVariableResistor();  //= CalculationInjectTime(FuelVolume, 1, _Pressure);
    _Tmr = micros();
    return true;
  }

private:

  char _pin;
  const double GasConstant = 8.314;
  const double MolM = 28.98;
  double AirTempK, massFuel;  //Температура воздуха в кельвинах
  double DurationOpen;        //Продолжительность открытия форсунок
  double FuelVolume;
  double _Pressure;
  int _CylinderVolume;
  unsigned long _Tmr, _TmrNew;

  double CalculationAirM(double P, double M, double R, double T) {
    return P * M / (R * T);
  }
  /*Рассчет длительности открытия форсунок по:
объему впрыскиваемого топлива,
коэффициент расхода топлива;
давлению в системе впрыска;
Объем у цилиндров;
*/
  double CalculationInjectTime(double FuelVolume, double ExpenseRatio, double SystemPressure /*, double engineDisplacement*/) {
    double InjectTime = (FuelVolume / ExpenseRatio) / (SystemPressure /* * 0.0001875*/);
    return InjectTime;
  }
  /*функция на C++, которая рассчитывает требуемый объем топлива для впрыска в цилиндр:
- `engineDisplacement` - объем цилиндра двигателя в кубических сантиметрах (см³).
- `airMass` - масса воздуха, затянутого в цилиндр за один цикл работы двигателя, в граммах (г).
- `airFuelRatio` - lambda соотношение массы воздуха к массе топлива при смешении воздуха и топлива, безразмерная величина.
*/
  double calculateFuelVolume(double engineDisplacement, double airMass, double airFuelRatio) {
    // Рассчитываем требуемый объем топлива по формуле
    double fuelVolume = (airFuelRatio * airMass) * (1.0 / 14.7) * engineDisplacement;  //(airMass /2) деление чтобы впрыскивать в один цилиндр 2 раза. Было так ( airFuelRatio/airMass).
    return fuelVolume;
  }
  double tempKelvin(double tempCelsius) {
    return tempCelsius + 273.15;
  }
};