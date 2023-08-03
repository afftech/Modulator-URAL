#include "wiring_time.h"
#include "wiring_digital.h"
#include "GyverPID.h"
GyverPID regulator(0.09, 0.5, 0.01, 1000);  // коэф. П, коэф. И, коэф. Д, период дискретизации dt (мс)
#define Open PB8
#define Close PB9

class PxxPid {
public:
  PxxPid(double SetPoint) {
    regulator.setpoint = SetPoint;  // сообщаем регулятору температуру, которую он должен поддерживать
  }
  void begin() {
    regulator.setDirection(NORMAL);  // направление регулирования (NORMAL/REVERSE). ПО УМОЛЧАНИЮ СТОИТ NORMAL
    regulator.setLimits(0, 1023);    // пределы (ставим для 8 битного ШИМ). ПО УМОЛЧАНИЮ СТОЯТ 0 И 255
    pinMode(Open, OUTPUT);
    pinMode(Close, OUTPUT);
    pinMode(PA0, OUTPUT);
    pinMode(PA1, INPUT);
  }
  void run(double rpm) {
    regulator.input = rpm;
    control(regulator.getResultTimer(), regulator.setpoint, rpm);
  }
  void control(double R, double P, double F) {
    if (P - 30 > F) {
      //Serial.println("Min");
      if (R > P) {
        digitalWrite(Open, true);
        digitalWrite(Close, false);
        /*Serial.print("Open:");
        Serial.println(F);*/
      }
    } else if (P + 30 < F) {
      //Serial.println("Max");
      if (R < P) {
        digitalWrite(Open, false);
        digitalWrite(Close, true);
        /*Serial.print("Close");
        Serial.println(F);*/
      }
    } else {
      digitalWrite(Open, false);
      digitalWrite(Close, false);
      /*Serial.print("None:");
      Serial.println(F);*/
    }
  }
  void log() {
    if (millis() - time >= 50) {
      time = millis();
      Serial.print("SetPoint:");
      Serial.print(regulator.setpoint);
      Serial.print(",");
      Serial.print("InPut:");
      Serial.print(regulator.input);
      Serial.print(",");
      Serial.print("OutPut:");
      Serial.print(regulator.getResultTimer());
      Serial.print(",");
      Serial.print("Max:");
      Serial.print(1023);
      Serial.print(",");
      Serial.print("Min:");
      Serial.println("0");
    }
  }
private:
  long time;
};