#include <Arduino.h>
#include <sta.h>

#include <HTTPClient.h>

using namespace sta::literals;
using namespace sta::control;

class Controller : public sta::micro_controller {
public:
  Controller() {
    sta::request<int> req = sta::post_request<int>();
  }

  Controller(const Controller&) = default;

public:
  bool onInit() override {
    sta::pin_mode(LED_BUILTIN, OUTPUT);
    
    return true;
  }

  bool onLoop() override {

    return true;
  }

  bool onUpdate() override {

    return true;
  }

  void onEnd() override {
    
  }
private:
  int counterVar = 0;
  unsigned long long previous = 0_sdp;
};

sta::micro_controller* sta::create_app() {
  return new Controller();
}

