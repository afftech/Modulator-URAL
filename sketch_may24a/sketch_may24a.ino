
/*Для теста*/
#define TestPin PB4
#define Resistor PA1
bool modulator = false;
unsigned long Timer1, Timer2;
/*****************************/

#define ModulatorF PA0
//#define ModulatorT PA2
#define Led PC13
#define Fire PA2
#define Fuel PA3
#define SensorMap PA4         //в диапазоне от 0 до 4095
#define SensorThrottle PA6    //в диапазоне от 0 до 4095
#define SensorTempEngine PA7  //Температура ДАД в диапазоне от 0 до 4095
#define SensorTempAir PB5     //Температура ДАД в диапазоне от 0 до 4095
#define PumpFuel PB10         //вкл насоса
#define none PB11             //
#define ReleLeft PB0          //Левый поворотник
#define ReleRight PB1         //Правый поворотник
#define Left PB12             //вкл насоса
#define Right PB13            //вкл насоса


volatile unsigned long TimeMZ, OldTimeMZ, NewTime, OldTimeMZData;
volatile unsigned long TimeOldData, TimeNewDataVMT1, TimeNewDataMZ;
bool Eco;
int i, OldData;
//#include "PxxPid.h"
//PxxPid pxxPid(500);
#include "SensorData.h"
SensorData SensorData(SensorMap, SensorThrottle, ModulatorF);

#include "ModulEEPROM.h"
ModulEEPROM modulEEPROM;

#include "FuelInjection.h"
FuelInjection FuelInjection(375, 3.5, Fuel);
volatile bool injectOn;


#include "MomentIgnition.h"
MomentIgnition MomentIgnition(Fire);


double FactMaxMap = 118;

bool triggerInterrupts1;
bool triggerInterrupts2;

volatile bool VMT;

// Входные данные

//double tmpAir = 0;      // температура двигателя
double tmp = 0;               // температура двигателя
volatile double OldRpm, rpm;  // об/мин
double load = 0.8;            // относительная нагрузка
double ve = 0.75;             // объем эффективного рабочего цикла, литры
double lambda = 1.0;          // коэффициент избытка воздуха   0.1в обедненная смесь, 0,9 обогащенная смесь
double p_atm = 101325;        // атмосферное давление, Па
double t_atm = 298;           // температура атмосферного воздуха, Кельвинах
double t_cyl = 1000;          // температура в цилиндре, К
double k = 1.4;               // показатель адиабаты

double v_air;   // объем воздуха, литры/мин
double m_air;   // масса воздуха, кг/мин
double m_fuel;  // масса топлива, кг/мин
double q_fuel;  // расход топлива, литры/мин
double t_ign;   // время зажигания, секунды???
double v_cyl;   // объем цилиндра, литры
double p_cyl;   // давление в цилиндре, Па
double t_burn;  // продолжительность горения, секунды
bool oldOptoData;

#include "Error.h"
Error error;

void setup() {
  error.run();
  //modulEEPROM.begin();
  //pxxPid.begin();
  Serial.begin(230400);
  // put your setup code here, to run once:

  pinMode(ModulatorF, INPUT);
  pinMode(Led, OUTPUT);
  pinMode(TestPin, OUTPUT);
  pinMode(Fire, OUTPUT);
}

void loop() {
  //modulEEPROM.updateThrottle(rpm);//Временно откл
  //test();
  //проверяем есть ли обороты обороты
  calculation();
  FuelInjection.run();
  SensorData.run(VMT);
  rpm = SensorData.getRpm();
  //pxxPid.run(rpm);
  MomentIgnition.run(rpm /*, SensorData.getTempEngine(), SensorData.getThrottle()*/);

  /*if (rpm != OldRpm) {
    OldRpm = rpm;
    Serial.println(OldRpm);
  }*/

  if (SensorData.getOpto() != oldOptoData) {
    oldOptoData = SensorData.getOpto();
    if (oldOptoData) {
      MZ();
    } else {
      VMT1();
    }
  }
}
void MZ() {
  if (/*micros() - TimeNewDataVMT1 >= 500 &&*/ VMT) {  //срабатывает если прошло 500
                                                       //Serial.println("MZ");
                                                       //if (rpm >= 100) {
    MomentIgnition.on(TimeNewDataVMT1, 1);             //включить рассчет момента зажиганиязажигания прилогая последний момент VMT
    //}
    digitalWrite(PC13, false);
    VMT = false;
    TimeNewDataMZ = micros();
  } else {
    error.SkippingIgnition();
  }
}
void VMT1() {
  /*if (VMT) {
    error.SkippingIgnition();
  }*/
  //if (micros() - TimeNewDataMZ >= 500) */ {
  TimeNewDataVMT1 = micros();
  //MomentIgnition.log(TimeNewDataVMT1);
  SensorData.inputRpm(TimeNewDataVMT1);
  digitalWrite(PC13, true);
  VMT = true;
  if (!Eco) {
    injectOn = true;  //включить рассчет и подачу топлива
  }
  /*if (rpm < 100) {
      MomentIgnition.on(TimeNewDataVMT1, 0);
      //Serial.println("VMT1");
    }*/
}

//расчет параметров
void calculation() {
  v_air = ve * rpm / 2;
  m_air = v_air * p_atm / (287 * t_atm);
  m_fuel = m_air / lambda;
  q_fuel = m_fuel / 720;
  t_ign = 1 / (rpm / 60);
  load = (calculateLoad(rpm, SensorData.getThrottle()));
  v_cyl = ve / load;                            // объем цилиндра, литры
  p_cyl = m_air * k * (t_cyl / t_atm) / v_cyl;  // давление в цилиндре, Па
  t_burn = sqrt(v_cyl / p_cyl);                 // продолжительность горения, секунды
  if (injectOn) {
    if (FuelInjection.AddFuel(/*SensorData.getThrottle()*/ 90 /*SensorData.getMap()*/, SensorData.getTempAir(), load)) {
      injectOn = false;
    }
  }
}

//расчет относительной нагрузки из оборотов/положения дроселя
double calculateLoad(double _rpm, double throttlePosition) {
  if (SensorData.mapValue(_rpm, 0, 5000, 0, 1) > SensorData.mapValue(throttlePosition, 0, 100, 0, 1) + 0.3) {  //отключаем подачу топлива если обороты большие, а ручку газа скинули к нулю
    //Eco = true;
  } else {
    //Eco = false;
  }
  const double maxThrottle = 100.0;
  const double maxLoad = 1.0;
  double load = (_rpm / 5000.0) * (throttlePosition / maxThrottle);
  load = min(load, maxLoad);
  return load;
}
void test() {

  // put your main code here, to run repeatedly:
  if (!modulator && micros() - Timer1 >= 100666) {  //12583333 //261666
    Timer2 = micros();
    modulator = true;
  } else {
    if (modulator && micros() - Timer2 >= 19333) {  //2416666  //48333
      Timer1 = micros();
      modulator = false;
    }
  }
  digitalWrite(TestPin, modulator);
}