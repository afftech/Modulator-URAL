class Error {
public:
  void run() {
    log();
  }
  void SkippingIgnition() {
    SkippingIgnitionOn = true;
  }
private:
  bool SkippingIgnitionOn;
  long TimeLog;
  void log() {
    if (millis() - TimeLog >= 2000) {
      TimeLog = millis();
      if (SkippingIgnitionOn) {
        Serial.print("SkippingIgnition;");
      }
    }
  }
};