#define ModulatorR PA1
#define ModulatorF PA0
//#define ModulatorT PA2
#define Led PC13
#define Fire PA2
#define Fuel PA3
#define SensorMap PA4         //в диапазоне от 0 до 4095
#define SensorThrottle PA6    //в диапазоне от 0 до 4095
#define SensorTempEngine PA7  //Температура ДАД в диапазоне от 0 до 4095
#define SensorTempAir PA5     //Температура ДАД в диапазоне от 0 до 4095

#define PumpFuel PB10  //вкл насоса
#define none PB11      //

#define ReleLeft PB0   //вкл насоса
#define ReleRight PB1  //вкл насоса
#define Left PB12      //вкл насоса
#define Right PB13     //вкл насоса


volatile unsigned long TimeMZ, OldTimeMZ, NewTime, OldTimeMZData;
volatile unsigned long TimeOldData, TimeNewData, TimeDataOld;

int i;

#include "FuelInjection.h"
FuelInjection FuelInjection(375, 3.5, Fuel);
volatile bool injectOn;

#include "SensorData.h"
SensorData SensorData(SensorMap, SensorTempAir, SensorThrottle);

#include "MomentIgnition.h"
MomentIgnition MomentIgnition(Fire);


double FactMaxMap = 118;

bool triggerInterrupts1;
bool triggerInterrupts2;

volatile bool VMT;
bool engineStateOn;

// Входные данные

//double tmpAir = 0;      // температура двигателя
double tmp = 0;         // температура двигателя
double rpm;             // об/мин
double load = 0.8;      // относительная нагрузка
double ve = 0.75;       // объем эффективного рабочего цикла, литры
double lambda = 1.0;    // коэффициент избытка воздуха   0.1в обедненная смесь, 0,9 обогащенная смесь
double p_atm = 101325;  // атмосферное давление, Па
double t_atm = 298;     // температура атмосферного воздуха, Кельвинах
double t_cyl = 1000;    // температура в цилиндре, К
double k = 1.4;         // показатель адиабаты

double v_air;   // объем воздуха, литры/мин
double m_air;   // масса воздуха, кг/мин
double m_fuel;  // масса топлива, кг/мин
double q_fuel;  // расход топлива, литры/мин
double t_ign;   // время зажигания, секунды???
double v_cyl;   // объем цилиндра, литры
double p_cyl;   // давление в цилиндре, Па
double t_burn;  // продолжительность горения, секунды
volatile int inn;
void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(ModulatorR, INPUT);
  pinMode(ModulatorF, INPUT);
  pinMode(Led, OUTPUT);
  pinMode(Fire, OUTPUT);
  attachInterrupt(ModulatorR, MZ, RISING);
  attachInterrupt(ModulatorF, VMT1, FALLING);
  //attachInterrupt(ModulatorT, VMT2, FALLING);
}

void loop() {
  //проверяем есть ли обороты обороты
  calculation();
  FuelInjection.run();
  SensorData.run(engineStateOn, VMT);
  MomentIgnition.run(rpm /*, SensorData.getTempEngine(), SensorData.getThrottle()*/);

  //Serial.println(rpm);
  //Serial.println(load);
  //Serial.println(SensorData.getThrottle());
  //Serial.println(SensorData.getMap());
  //Serial.println(SensorData.getTemp());
}

void MZ() {
  if (rpm > 150) {
    MomentIgnition.on(TimeNewData, 1);  //включить рассчет момента зажиганиязажигания
    //Serial.println("MZ");
    //Serial.println(rpm);
  }
  digitalWrite(PC13, false);
  VMT = false;
  inn = 1;
  //Serial.println(micros() - TimeNewData);
}
void VMT1() {

  if (rpm <= 150) {
    MomentIgnition.on(TimeNewData, 0);
    Serial.println("VMT1");
  }
  //MomentIgnition.off(TimeNewData);  //включить рассчет момента зажиганиязажигания
  TimeNewData = micros();

  digitalWrite(PC13, true);
  //digitalWrite(Fire, false);  //отключаем сигнал зажигания
  VMT = true;
  injectOn = true;  //включить рассчет и подачу топлива
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

  if (TimeNewData != TimeOldData) {  //сколько оборотов в минуту
    double data;
    data = 60000.0 / ((TimeNewData - TimeOldData) / 1000);
    if (data < 100) {  // для того что бы в вмт искру дать
      rpm = 0;
    } else {
      rpm = data;
    }
    TimeOldData = TimeNewData;
    engineStateOn = true;
  }
  if (millis() - (TimeOldData / 1000) >= 800) {  // если оборотов нет т.е. больше 800 милисек.
    // Serial.println(TimeOldData);
    rpm = 0;
  }
  if (injectOn) {
    if (FuelInjection.AddFuel(SensorData.getMap(), SensorData.getTemp())) {
      injectOn = false;
    }
  }
}

//расчет относительной нагрузки из оборотов/положения дроселя
double calculateLoad(double _rpm, double throttlePosition) {
  //const double maxThrottle = 100.0;
  _rpm = SensorData.mapValue(_rpm, 0, 5000, 0, 1.0);
  throttlePosition = SensorData.mapValue(throttlePosition, 0, 100, 1, 0.0);
  //double data = (((_rpm / (5000.0 / 100)) + (throttlePosition / (maxThrottle / 100)))) / 2 * 0.01;
  //data = min(data, maxData);
  return _rpm - throttlePosition;
}