//#define PinModulator 3
#define GND PA0
#define PinModulator PA2
#define Resistor PA1
#define LED PA11

bool modulator = true;
unsigned long Timer1, Timer2;

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(PinModulator, OUTPUT);
  pinMode(GND, OUTPUT);
  pinMode(PC13, OUTPUT);
  pinMode(LED, INPUT);
}

void loop() {
  if (digitalRead(LED)) {
    digitalWrite(PC13, false);
  } else {
    digitalWrite(PC13, true);
  }
  int R = analogRead(Resistor);
  double Y = map(R, 0, 1023, 12000, 650000);
  if (Y > 600000) {
    Y = 0;
  } else {
    Y = Y / 360;
    Serial.println(Y);
  }

  // put your main code here, to run repeatedly:
  if (!modulator && micros() - Timer1 >= Y * 330) {  //11999 //183333
    Timer2 = micros();
    modulator = true;
  } else {
    if (modulator && micros() - Timer2 >= Y * 30) {  //999 //16666
      Timer1 = micros();
      modulator = false;
    }
  }

  digitalWrite(PinModulator, modulator);
}