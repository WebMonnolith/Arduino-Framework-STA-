#include <Arduino.h>
#include <sta.h>

sta::pair<sta::function<int()> , sta::function<int()>> closure(int x) {
  auto fn1 = [x]() mutable {
    x++;
    return x;
  };

  auto fn2 = [x]() mutable {
    x--;
    return x;
  };

  return sta::pair<sta::function<int()> , sta::function<int()>>(fn1, fn2);
}

template <typename T>
void foo(T i) {
  Serial.println(i);
}

using namespace sta::literals;
using namespace sta::control;

class Controller : public sta::micro_controller {
public:
  Controller() {
    
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
    // sta::logln(closure_fn.first());
    foo("Hello world");
    foo(7);
    return true;
  }

  void onEnd() override {
    
  }
private:
  int counterVar = 0;
  unsigned long long previous = 0_sdp;

  sta::pair<sta::function<int()> , sta::function<int()>> closure_fn = closure(8);

};

sta::micro_controller* sta::create_app() {
  return new Controller();
}

