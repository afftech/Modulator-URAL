#define AB PB9
#define BA PB8
#define CD PB7
#define DC PB6

class ControlPxx {
public:
  void begin() {
    pinMode(AB, OUTPUT);
    pinMode(BA, OUTPUT);
    pinMode(CD, OUTPUT);
    pinMode(DC, OUTPUT);
  }

  void run(double rpm) {
    if (rpm < 10) {

    } else if (rpm > 1000) {


    } else {
    }
  }
private:
  long TimerOpen, TimerClose;

  bool Open(int step) {
    if (millis() - TimerOpen >= 500) {
      TimerOpen = millis();
      if (stepOpen == 0) {
        lock = true;
        digitalMode(AB, true);
        digitalMode(BA, false);
        digitalMode(CD, false);
        digitalMode(DC, false);
      } else if (stepOpen == 1) {
        lock = true;
        digitalMode(AB, false);
        digitalMode(BA, false);
        digitalMode(CD, true);
        digitalMode(DC, false);

      } else if (stepOpen == 2) {
        lock = true;
        digitalMode(AB, false);
        digitalMode(BA, true);
        digitalMode(CD, false);
        digitalMode(DC, false);

      } else if (stepOpen == 3) {
        lock = true;
        digitalMode(AB, false);
        digitalMode(BA, false);
        digitalMode(CD, false);
        digitalMode(DC, true);
      } else if (stepOpen == 4) {
        lock = false;
        digitalMode(AB, false);
        digitalMode(BA, false);
        digitalMode(CD, false);
        digitalMode(DC, false);
        stepOpen = 0;
        return true;
      }
      stepOpen++;
      return false;
    }
  }
  bool Close(int step) {
    if (millis() - TimerClose >= 500) {
      TimerClose = millis();
      if (stepClose == 0) {
        lock = true;
        digitalMode(AB, false);
        digitalMode(BA, false);
        digitalMode(CD, false);
        digitalMode(DC, true);
      } else if (stepClose == 1) {
        lock = true;
        digitalMode(AB, false);
        digitalMode(BA, true);
        digitalMode(CD, false);
        digitalMode(DC, false);

      } else if (stepClose == 2) {
        lock = true;
        digitalMode(AB, false);
        digitalMode(BA, false);
        digitalMode(CD, true);
        digitalMode(DC, false);

      } else if (stepClose == 3) {
        lock = true;
        digitalMode(AB, true);
        digitalMode(BA, false);
        digitalMode(CD, false);
        digitalMode(DC, false);
      } else if (stepClose == 4) {
        lock = false;
        digitalMode(AB, false);
        digitalMode(BA, false);
        digitalMode(CD, false);
        digitalMode(DC, false);
        stepClose = 0;
        return true;
      }
      stepClose++;
      return false;
    }
  }
};