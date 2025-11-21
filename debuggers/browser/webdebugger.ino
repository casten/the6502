#include <TimerEvent.h>
#include <SimpleWebSerial.h>

#define CLOCK_PIN 31
#define LED_PIN 13
#define RW_PIN 23
#define RESET_PIN 25

SimpleWebSerial WebSerial;

double hz = 0;
bool process_event = false;
bool clock_state = false;
bool clock_running = false;

int addr_pins[] = {22,24,26,28,30,32,34,36,
                   38,40,42,44,46,48,50,52};

int data_pins[] = {53,51,49,47, 
                   45,43,41,39};

TimerEvent clock_timer_event;
bool disable_logging = false;

void print_pins() {
  if (disable_logging) {
    return;
  }
  uint16_t address=0;
  uint8_t data=0;
  char str_out[64] = {0};
  String s = "";
  for (int i=0;i<16;i++) {
    uint8_t bit = digitalRead(addr_pins[i])?1:0;
    address = (address << 1) + bit;
    s+=bit?"1":"0";
  }
  s+="   ";
  for (int i=0;i<8;i++) {
    uint8_t bit = digitalRead(data_pins[i])?1:0;
    data = (data << 1) + bit;
    s+=bit?"1":"0";
  }
  s+="   ";
  uint8_t rw = digitalRead(RW_PIN);
  uint8_t reset = digitalRead(RESET_PIN);
  sprintf(str_out, "%04x %02x %c %c", address, data, rw?'r':'W', reset?'.':'R');
  s+=str_out;
  WebSerial.send("log", s);
}

void step() {
    if (!clock_running) {
      digitalWrite(CLOCK_PIN, HIGH);
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(CLOCK_PIN, LOW);      
      digitalWrite(LED_PIN, LOW);
      delay(100);
      print_pins();
    }
}

void clock_cb() {
  if (clock_running) {
    clock_state = !clock_state;
    digitalWrite(CLOCK_PIN, clock_state?HIGH:LOW);
    digitalWrite(LED_PIN, clock_state?HIGH:LOW);
    if (clock_state) {
        print_pins();
    }
  }
  else {
    digitalWrite(CLOCK_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }
}

void init_io_pins() {
  for (int i=0;i<16;i++) {
    pinMode(addr_pins[i], INPUT);    
  }
  for (int i=0;i<8;i++) {
    pinMode(data_pins[i], INPUT);    
  }
  pinMode(RW_PIN, INPUT);
}

void handleSWSMsg(JSONVar parameter) {
  if (parameter["action"] == String("start")) {
    clock_running = true;
    hz = parameter["hz"];
  }
  else 
  if (parameter["action"] == String("stop")) {
    clock_running = false;
    digitalWrite(CLOCK_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }
  else
  if (parameter["action"] == String("pulse")) {
      if (clock_running) {
        WebSerial.send("log", "Can't pulse while clock is running!");
        return;
      }
      step();
  }
  else
  if (parameter["action"] == String("set_logging")) {
    disable_logging = parameter["disabled"];
  }
  process_event = true;
}

void setup() {
  Serial.begin(115200);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  clock_timer_event.set((int)(hz/500),clock_cb);
  clock_timer_event.disable();
  init_io_pins();
  Serial.println("\nDebugger Initialized.");
  WebSerial.on("debugger", handleSWSMsg);
}

void loop() {
  if (process_event) {
    if (clock_running) {
      clock_timer_event.setPeriod(500/hz);
      clock_timer_event.reset();
      clock_timer_event.enable();
    }
    else {
      clock_timer_event.disable();
    }
  }
  process_event = false;
  clock_timer_event.update();
  WebSerial.check();
}
