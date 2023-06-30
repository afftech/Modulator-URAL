#define PinModulator 3

bool modulator = true;
unsigned long Timer1, Timer2;
void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(PinModulator, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!modulator && micros() - Timer1 >= 183333) {  //11999 //183333
    Timer2 = micros();
    modulator = true;
  } else {
    if (modulator && micros() - Timer2 >= 16666) {  //999 //16666
      Timer1 = micros();
      modulator = false;
    }
  }
  //Serial.println(modulator);
  digitalWrite(PinModulator, modulator);
}