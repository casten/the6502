#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TimerEvent.h>
#include <ButtonDebounce.h>
#include <Rotary.h>
#include <stdint.h>
#include <SimpleWebSerial.h>
#include <JSON.h>

SimpleWebSerial WebSerial;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define CLOCK_PIN 12

#define DISABLE_LOGGING false

unsigned long hz_index = 3;
float hz_table[] = {0.125, 0.25, 0.5, 0, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};
double hz = 0;
bool input_event = false;
bool clock_state = false;
bool clock_running = false;

int addr_pins[] = {53,51,49,47,45,43,41,39, 
                   38,40,42,44,46,48,50,52};

int data_pins[] = {34, 32, 30, 28,
                   29, 31, 33, 35};

int rw_pin = 24;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
TimerEvent clock_timer_event;
TimerEvent serial_timer_event;

ButtonDebounce start_stop_button(19, 100);
ButtonDebounce step_button(18, 100);
Rotary rotary(3,2,4);

void hz_index_to_str(int index, char str[]) {
  if (index <= 2) {
    int frac_hz = (int)(1/hz_table[index]);
    sprintf(str, "1/%d", frac_hz);
  }
  else {
    sprintf(str, "%d", (int)hz_table[index]);
  }
}

void redraw() {
  char str[32];
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.print("Hz");
  display.setCursor(0,20);
  hz_index_to_str(hz_index, str);
  display.setTextSize(4);
  display.setTextColor(SSD1306_WHITE);
  display.print(str);
  display.display();
}

void clock_start_stop_pressed(const int is_up) {
  if (!is_up) {
      if (hz != 0) {
        if (clock_running) {
          clock_state = false;
          digitalWrite(CLOCK_PIN, LOW);
        }
        clock_running = !clock_running;
        Serial.print(clock_running?"Enabling":"Disabling");
        Serial.println(" clock");
      }
      else {
        Serial.println("Press with 0 Hz.  Just reading pins.");
      }
      input_event = true;
  }
}

void print_pins(uint16_t *addr_out=NULL, uint8_t *data_out=NULL, uint8_t *rw_out=NULL) {
  uint16_t address=0;
  uint8_t data=0;
  char str_out[64] = {0};
  String s = "";
  for (int i=0;i<16;i++) {
    uint8_t bit = digitalRead(addr_pins[i])?1:0;
    address = (address << 1) + bit;
    Serial.print(bit);
    s+=bit?"1":"0";
  }
  Serial.print("   ");
  s+="   ";
  for (int i=0;i<8;i++) {
    uint8_t bit = digitalRead(data_pins[i])?1:0;
    data = (data << 1) + bit;
    Serial.print(bit);
    s+=bit?"1":"0";
  }
  Serial.print("   ");
  s+="   ";
  uint8_t rw = digitalRead(rw_pin);
  sprintf(str_out, "%04x %c %02x", address, rw?'r':'W', data);
  Serial.println(str_out);
  s+=str_out;
  if (addr_out)
    *addr_out = address;
  if (data_out)
    *data_out = data;
  if (rw_out)
    *rw_out = rw;
  WebSerial.send("log", s);
}

void step_pressed(const int state) {
    if (!clock_running) {
      if (state) {
        digitalWrite(CLOCK_PIN, HIGH);
        delay(250);
        digitalWrite(CLOCK_PIN, LOW);      
        delay(250);
        print_pins();
      }
    }
    else {
      Serial.println("Won't step while clock running.");      
    }
}

void clock_cb() {
  if (clock_running) {
    clock_state = !clock_state;
    digitalWrite(CLOCK_PIN, clock_state?HIGH:LOW);
    if (clock_state) {
      if (!DISABLE_LOGGING) {
        print_pins();
      }
    }
  }
  else {
    digitalWrite(CLOCK_PIN, LOW);
  }
}

void rotary_change_handler(Rotary &rotary) {
  int delta = (rotary.getDirection() == 1)?1:-1;
  int new_index = hz_index + delta; 
  size_t num_elements = sizeof(hz_table) / sizeof(hz_table[0]);
  if ((new_index >= 0) && (new_index < num_elements)) {
    hz_index = new_index;
    hz = hz_table[hz_index];
  } 
  input_event = true;
}

void init_io_pins() {
  for (int i=0;i<16;i++) {
    pinMode(addr_pins[i], INPUT);    
  }
  for (int i=0;i<8;i++) {
    pinMode(data_pins[i], INPUT);    
  }
  pinMode(rw_pin, INPUT);
}

void handleSWSClock(JSONVar parameter) {
  //WebSerial.send("log", parameter);
  if (parameter["action"] == String("start")) {
    clock_running = true;
    hz = parameter["hz"];
  }
  else 
  if (parameter["action"] == String("stop")) {
    clock_running = false;
  }
  else
  if (parameter["action"] == String("pulse")) {
      if (clock_running) {
        WebSerial.send("log", "Can't pulse while clock is running!");
        return;
      }
      step_pressed(true);
    }
  input_event = true;
}


void setup() {
  Serial.begin(115200);
  Wire.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println("SSD1306 allocation failed");
    for(;;); // Don't proceed, loop forever
  }

  redraw();

  pinMode(CLOCK_PIN, OUTPUT);
  clock_timer_event.set((int)(hz/500),clock_cb);
  clock_timer_event.disable();

  start_stop_button.setCallback(clock_start_stop_pressed);
  step_button.setCallback(step_pressed);
  rotary.setChangedHandler(rotary_change_handler);
  init_io_pins();
  Serial.println("\nDebugger Initialized.");
  WebSerial.on("clock", handleSWSClock);
}


void loop() {
  if (input_event) {
    redraw();
    if (hz == 0) {
//      print_pins();
    }
    if (clock_running) {
      clock_timer_event.setPeriod(500/hz);
      clock_timer_event.reset();
      clock_timer_event.enable();
    }
    else {
      clock_timer_event.disable();
    }
  }
  input_event = false;
  clock_timer_event.update();
  start_stop_button.update();
  step_button.update();
  rotary.loop();
  WebSerial.check();
}
