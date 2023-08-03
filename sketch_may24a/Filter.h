class Filter {
public:
  Filter(double Ratio) {
    _Ratio = Ratio;
  }

  int ClearingSignal(int data) {
    input = data;
    filtVal += (data - filtVal) * _Ratio;
    output = data;
    return data;
  }
  void log() {
    if (millis() - TimeLog >= 4) {
      TimeLog = millis();
      Serial.print("MAX:");
      Serial.print(1023);
      Serial.print(",");
      Serial.print("input:");
      Serial.print(input);
      Serial.print(",");
      Serial.print("filter:");
      Serial.print(output);
      Serial.print(",");
      Serial.print("Zero:");
      Serial.println(0);
    }
  }
private:
  int input, output;
  float filtVal = 0;
  long TimeLog;
  double _Ratio;
};