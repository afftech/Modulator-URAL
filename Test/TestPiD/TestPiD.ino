
#include "GyverPID.h"
#define Open PB8
#define Close PB9
GyverPID regulator(0.09, 0.5, 0.01, 1000);  // коэф. П, коэф. И, коэф. Д, период дискретизации dt (мс)
                                            // или так:
                                            // GyverPID regulator(0.1, 0.05, 0.01);	// можно П, И, Д, без dt, dt будет по умолч. 100 мс
int temp;                                   // читаем с датчика температуру
long time;

void setup() {
  regulator.setDirection(NORMAL);  // направление регулирования (NORMAL/REVERSE). ПО УМОЛЧАНИЮ СТОИТ NORMAL
  regulator.setLimits(0, 1023);    // пределы (ставим для 8 битного ШИМ). ПО УМОЛЧАНИЮ СТОЯТ 0 И 255
  regulator.setpoint = 500;        // сообщаем регулятору температуру, которую он должен поддерживать
  pinMode(Open, OUTPUT);
  pinMode(Close, OUTPUT);
  pinMode(PA0, OUTPUT);
  pinMode(PA1, INPUT);
  Serial.begin(115200);
}

void loop() {
  regulator.input = temp;  // сообщаем регулятору текущую температуру
  //Print();
  run(regulator.getResultTimer(), regulator.setpoint, temp);
}
void Print() {
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
void run(double R, double P, double F) {
  if (P - 50 > F) {
    Serial.println("Min");
    if (R > P) {
      digitalWrite(Open, true);
      digitalWrite(Close, false);
      Serial.print("Open:");
      Serial.println(F);
    }
  } else if (P + 50 < F) {
    Serial.println("Max");
    if (R < P) {
      digitalWrite(Open, false);
      digitalWrite(Close, true);
      Serial.print("Close");
      Serial.println(F);
    }
  } else {
    digitalWrite(Open, false);
    digitalWrite(Close, false);
    Serial.print("None:");
    Serial.println(F);
  }
}