#include <Arduino.h>
#include <SoftwareSerial.h>

#include <sta++>

using namespace sta::literals;
using namespace sta::control;

class Controller : public sta::micro_controller {
public:
  Controller() : sserial(1, 0) {
    this->disp = sta::nextion_serial(sserial, 9600);
  }

  Controller(const Controller&) = default;

public:
  bool onInit() override {
    sta::pin_mode(LED_BUILTIN, OUTPUT);
    this->disp.init();

    return true;
  }

  bool onLoop() override {

    return true;
  }

  bool onUpdate() override {
    this->updateDisplay(counterVar, 1);
    return true;
  }

private:
  void updateDisplay(int& counter, int delayTime) {
    this->disp.setComponentText("t0", String(counter));
    delay(delayTime); 
    counter++;
  }
private:
  SoftwareSerial sserial; // Nextion TX to pin 0 and RX to pin 1 of Arduino
  sta::nextion_serial disp;

  int counterVar = 0;
  unsigned long long previous = 0_sdp;
};

sta::micro_controller* sta::create_app() {
  return new Controller();
}

int sta::set_interval() {
  return 10000;
}

