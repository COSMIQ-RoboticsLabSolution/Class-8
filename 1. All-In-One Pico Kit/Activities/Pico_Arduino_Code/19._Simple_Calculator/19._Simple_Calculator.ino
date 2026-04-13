/*
  =========================================================
   RP2040 + ST7789 Touch Display + IR Remote Calculator
  =========================================================

  LIBRARIES USED:
  ---------------------------------------------------------
  - LovyanGFX.hpp  → Display + Touch Control
  - IRremote.hpp   → IR Remote Input Handling
  - <stack>        → Expression Evaluation (Postfix)

  FUNCTION:
  ---------------------------------------------------------
  - Takes input from IR Remote
  - Displays input on TFT screen
  - Evaluates mathematical expressions
  - Shows result on screen

  FEATURES:
  ---------------------------------------------------------
  - Supports +, -, *, /
  - Handles decimal numbers
  - Unary minus support
  - Error handling (NaN / Overflow)
  - Debounced IR input
*/

// ==========================
// LIBRARIES
// ==========================
#include <LovyanGFX.hpp>   // Display control
#include <IRremote.hpp>    // IR remote handling
#include <stack>           // Stack for expression evaluation

// ==========================
// CUSTOM DISPLAY CLASS
// ==========================
class LGFX : public lgfx::LGFX_Device {

  lgfx::Panel_ST7789 _panel_instance; // Display panel
  lgfx::Bus_SPI _bus_instance;        // SPI bus
  lgfx::Touch_FT5x06 _touch_instance; // Touch controller

public:
  LGFX(void) {

    // ==========================
    // SPI BUS CONFIGURATION
    // ==========================
    auto cfg = _bus_instance.config();
    cfg.spi_host = 0;
    cfg.spi_mode = 0;
    cfg.freq_write = 80000000; // Fast write speed
    cfg.freq_read  = 16000000; // Read speed
    cfg.pin_sclk = 6;
    cfg.pin_mosi = 7;
    cfg.pin_miso = -1; // Not used
    cfg.pin_dc   = 16;
    _bus_instance.config(cfg);
    _panel_instance.setBus(&_bus_instance);

    // ==========================
    // DISPLAY PANEL CONFIG
    // ==========================
    auto panel_cfg = _panel_instance.config();
    panel_cfg.pin_cs = 17;
    panel_cfg.pin_rst = -1;
    panel_cfg.memory_width  = 240;
    panel_cfg.memory_height = 320;
    panel_cfg.panel_width   = 240;
    panel_cfg.panel_height  = 320;
    panel_cfg.offset_rotation = 1;
    panel_cfg.invert = true; // Color inversion

    _panel_instance.config(panel_cfg);

    // ==========================
    // TOUCH CONFIGURATION
    // ==========================
    auto touch_cfg = _touch_instance.config();
    touch_cfg.x_min = 0;
    touch_cfg.x_max = 239;
    touch_cfg.y_min = 0;
    touch_cfg.y_max = 319;
    touch_cfg.pin_int = 25;
    touch_cfg.pin_rst = 24;
    touch_cfg.i2c_port = 0;
    touch_cfg.i2c_addr = 0x38;
    touch_cfg.pin_sda = 4;
    touch_cfg.pin_scl = 5;
    touch_cfg.freq = 400000;

    _touch_instance.config(touch_cfg);
    _panel_instance.setTouch(&_touch_instance);

    setPanel(&_panel_instance); // Attach panel
  }
};

// ==========================
// GLOBAL VARIABLES
// ==========================
LGFX gfx;              // Display object
String input = "";     // Stores current expression
bool evaluated = false; // Tracks if result was shown

// ==========================
// DISPLAY FUNCTIONS
// ==========================

// Show input expression
void showInput(const String &text) {
  gfx.fillScreen(TFT_BLACK);
  gfx.setCursor(10, 100);
  gfx.setTextSize(2);
  gfx.setTextColor(TFT_WHITE);
  gfx.print("Input: " + text);
  gfx.flush();
}

// Show result
void showResult(const String &text) {
  gfx.fillScreen(TFT_BLACK);
  gfx.setCursor(10, 100);
  gfx.setTextSize(2);
  gfx.setTextColor(TFT_GREEN);
  gfx.print("Result: " + text);
  gfx.flush();
}

// ==========================
// EXPRESSION UTILITIES
// ==========================

// Operator precedence
int precedence(char op) {
  if (op == '+' || op == '-') return 1;
  if (op == '*' || op == '/') return 2;
  return 0;
}

// Check if character is operator
bool isOperator(char c) {
  return c == '+' || c == '-' || c == '*' || c == '/';
}

// ==========================
// INFIX → POSTFIX CONVERSION
// ==========================
bool infixToPostfix(const String &infix, String &postfix) {

  std::stack<char> stack;
  String num = "";

  for (int i = 0; i < infix.length(); ++i) {

    char c = infix[i];

    if (c == ' ') continue;

    // Handle negative numbers
    if (c == '-' && (i == 0 || isOperator(infix[i - 1]))) {
      num += c;
      continue;
    }

    // Build number
    if (isdigit(c) || c == '.') {
      num += c;
    }

    // Handle operator
    else if (isOperator(c)) {

      if (num.length() > 0) {
        postfix += num + " ";
        num = "";
      }

      while (!stack.empty() && precedence(stack.top()) >= precedence(c)) {
        postfix += stack.top();
        postfix += " ";
        stack.pop();
      }

      stack.push(c);
    }

    else {
      return false; // Invalid character
    }
  }

  if (num.length() > 0) postfix += num + " ";

  while (!stack.empty()) {
    postfix += stack.top();
    postfix += " ";
    stack.pop();
  }

  return true;
}

// ==========================
// POSTFIX EVALUATION
// ==========================
float evaluatePostfix(const String &postfix) {

  std::stack<float> stack;
  String token = "";

  for (int i = 0; i <= postfix.length(); ++i) {

    char c = postfix[i];

    if (c == ' ' || c == '\0') {

      if (token.length() > 0) {

        // Operator
        if (token.length() == 1 && isOperator(token[0])) {

          if (stack.size() < 2) return NAN;

          float b = stack.top(); stack.pop();
          float a = stack.top(); stack.pop();

          switch (token[0]) {
            case '+': stack.push(a + b); break;
            case '-': stack.push(a - b); break;
            case '*': stack.push(a * b); break;
            case '/':
              if (b == 0) return NAN; // Divide by zero
              stack.push(a / b);
              break;
          }
        }

        // Number
        else {
          stack.push(token.toFloat());
        }

        token = "";
      }
    }
    else {
      token += c;
    }
  }

  return (stack.size() == 1) ? stack.top() : NAN;
}

// ==========================
// MAIN EVALUATION FUNCTION
// ==========================
float evaluateExpression(const String &expr) {
  String postfix;
  if (!infixToPostfix(expr, postfix)) return NAN;
  return evaluatePostfix(postfix);
}

// ==========================
// SETUP FUNCTION
// ==========================
void setup() {

  Serial.begin(115200);

  gfx.init(); // Initialize display

  pinMode(0, OUTPUT);       // Backlight pin
  digitalWrite(0, HIGH);    // Turn ON backlight

  showInput("");            // Show empty input

  IrReceiver.begin(11, DISABLE_LED_FEEDBACK); // IR receiver pin
}

// ==========================
// LOOP FUNCTION
// ==========================
void loop() {

  if (IrReceiver.decode()) {

    uint8_t code = IrReceiver.decodedIRData.command;

    Serial.print("Received: 0x");
    Serial.println(code, HEX);

    // ==========================
    // DEBOUNCE LOGIC
    // ==========================
    static uint32_t lastKeyCode = 0;
    static unsigned long lastKeyTime = 0;
    const unsigned long debounceDelay = 300;

    if (code == lastKeyCode && (millis() - lastKeyTime) < debounceDelay) {
      IrReceiver.resume();
      return;
    }

    lastKeyCode = code;
    lastKeyTime = millis();

    // Reset input after result
    if (evaluated) {
      input = "";
      evaluated = false;
    }

    // ==========================
    // IR BUTTON MAPPING
    // ==========================
    switch (code) {

      case 0x16: input += "0"; break;
      case 0x0C: input += "1"; break;
      case 0x18: input += "2"; break;
      case 0x5E: input += "3"; break;
      case 0x08: input += "4"; break;
      case 0x1C: input += "5"; break;
      case 0x5A: input += "6"; break;
      case 0x42: input += "7"; break;
      case 0x52: input += "8"; break;
      case 0x4A: input += "9"; break;

      case 0x15: input += "+"; break;
      case 0x07: input += "-"; break;
      case 0x44: input += "/"; break;
      case 0x40: input += "*"; break;

      case 0x09: input = ""; break; // Clear

      // ==========================
      // CALCULATE RESULT
      // ==========================
      case 0x43: {
        float result = evaluateExpression(input);

        if (isnan(result)) {
          showResult("Error");
        } 
        else if (!isfinite(result)) {
          showResult("Overflow");
        } 
        else {
          showResult(String(result, 2));
        }

        evaluated = true;
        IrReceiver.resume();
        return;
      }

      // ==========================
      // DECIMAL HANDLING
      // ==========================
      case 0x19: {
        if (input.length() == 0 || input.endsWith(".") || isOperator(input[input.length() - 1])) break;

        int i = input.length() - 1;
        while (i >= 0 && (isdigit(input[i]) || input[i] == '.')) {
          if (input[i] == '.') break;
          --i;
        }

        if (i >= 0 && input[i] == '.') break;

        input += ".";
        break;
      }

      default:
        Serial.print("Unknown key: 0x");
        Serial.println(code, HEX);
        break;
    }

    showInput(input);
    IrReceiver.resume();
  }
}