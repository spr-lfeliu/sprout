#include "spark-dallas-temperature/spark-dallas-temperature.h" // Used for soil temperature sensor
#include "OneWire/OneWire.h" // Used for soil temperature sensor
#include "Adafruit_DHT/Adafruit_DHT.h" // Used for air sensor (air temperature and moisture)

#define BAUD_RATE 57600
#define TIME_DELAY_MILLISECONDS 1000

#define PIN_RGB_LED_RED B0
#define PIN_RGB_LED_GREEN B1
#define PIN_RGB_LED_BLUE B2

// Soil moisture sensor
// The moisture sensor raw data values vary between 0 and about 2500. 
// Low values mean less moisture while higher values mean more moisture.
#define PIN_SOIL_MOISTURE A0

// Soil temperature
// Usable temperature range: -55 to 125°C (-67°F to +257°F)
OneWire  oneWire(D2);
DallasTemperature sensors(&oneWire);

// Air temperature and moisture sensor
// Values are converted.
// Temperature is in degrees Farenheit.
// Moisture is given as Relative humidity (accurate between 20 and 80 %)
#define DHT_TYPE DHT11
#define DHT_PIN D0

DHT dht(DHT_PIN, DHT_TYPE); 

// Light sensor
// Raw value will be between 0 (low light) and 4095 (more light)
#define PIN_LIGHT A1

// Push buttons
volatile bool pb_ok_pressed = false;
volatile bool pb_great_pressed = false;
volatile bool pb_not_good_pressed = false;

// Variables
// GET https://api.particle.io/v1/devices/{DEVICE_ID}/{VARIABLE}?format=raw&access_token=token
// Up to 10 cloud variables may be defined and each variable name is limited to a maximum of 12 characters

double var_air_temperature;
double var_air_humidity;
double var_soil_temperature;
int var_soil_moisture;
int var_light;

volatile bool isDeviceActivated = false;
enum Color { red, green, blue, yellow };
volatile Color rgbState;
volatile bool isRgbOn = true;
String lastCommand = "";

// Functions
// POST /v1/devices/{DEVICE_ID}/{FUNCTION}
// Up to 4 different cloud functions are supported
// Max of 12 characters for function name/key

int executeServerCommand(String command);

// Timers
Timer rgbLedTimer(1000, blinkRgb);

void setup() {
   Serial.begin(BAUD_RATE);
   init_air_sensor();
   init_soil_temp_sensor();
   setup_push_buttons();
   register_variables();
   register_functions();
   setup_rgb_led();
   // TODO: need to call endpoint to check if the device has been activated
}

void init_air_sensor() {
    dht.begin();
}

void init_soil_temp_sensor() {
    sensors.begin();
}

void setup_push_buttons() {
   // PB OK 
   pinMode(D5, INPUT_PULLUP);
   attachInterrupt(D5, push_button_ok_isr, RISING);
   // PB GREAT
   pinMode(D6, INPUT_PULLUP);
   attachInterrupt(D6, push_button_great_isr, RISING);
   // PB NOT GOOD
   pinMode(D7, INPUT_PULLUP);
   attachInterrupt(D7, push_button_not_good_isr, RISING);
}

void register_functions() {
    Particle.function("x", executeServerCommand);
}

void register_variables() {
    Particle.variable("aT", var_air_temperature);
    Particle.variable("aH", var_air_humidity);
    Particle.variable("sT", var_soil_temperature);
    Particle.variable("sM", var_soil_moisture);
    Particle.variable("L", var_light);
}

void setup_rgb_led() {
    pinMode(PIN_RGB_LED_RED, OUTPUT);
    pinMode(PIN_RGB_LED_GREEN, OUTPUT);
    pinMode(PIN_RGB_LED_BLUE, OUTPUT);  
}

void loop() {
    if (isDeviceActivated) {
        update_variables();
        check_push_buttons_state();
        check_errors();
    } else {
        display_inactive();
    }
    delay(TIME_DELAY_MILLISECONDS);
}

void setColor(int red, int green, int blue)
{
  analogWrite(PIN_RGB_LED_RED, red);
  analogWrite(PIN_RGB_LED_GREEN, green);
  analogWrite(PIN_RGB_LED_BLUE, blue);  
}

void turnRed() {
    rgbState = red;
    isRgbOn = true;
    setColor(255, 0, 0);
}

void turnBlue() {
    rgbState = blue;
    isRgbOn = true;
    setColor(0, 0, 255);
}

void turnGreen() {
    rgbState = green;
    isRgbOn = true;
    setColor(0, 255, 0);
}

void turnYellow() {
    rgbState = yellow;
    isRgbOn = true;
    setColor(255, 255, 0);
}

void turnOff() {
    isRgbOn = false;
    setColor(0, 0, 0);
}

void blinkRgb() {
    if (isRgbOn) {
        turnOff();
    } else { 
        switch(rgbState) {
            case red:
                turnRed();
                break;
            case green:
                turnGreen();
                break;
            case blue:
                turnBlue();
                break;
            case yellow:
                turnYellow();
                break;
        }
    }
}

void blinkBlue() {
    turnBlue();
    rgbLedTimer.start();
}

void blinkYellow() {
    turnYellow();
    rgbLedTimer.start();
}

void blinkRed() {
    turnRed();
    rgbLedTimer.start();
}

void update_variables() {
    var_air_temperature = getAirTemp();
    var_air_humidity = getAirMoisture();
    var_soil_temperature = getSoilTemp();
    var_soil_moisture = getSoilMoisture();
    var_light = getLight();
}

void send_data() {
    String data = "{" \
    "\"sT\":" + String(var_soil_temperature) + "," \
    "\"sM\":" + String(var_soil_moisture) + "," \
    "\"aT\":" + String(var_air_temperature) + "," \
    "\"aH\":" + String(var_air_humidity) + "," \
    "\"L\":" + String(var_light) + \
    "}";
    Particle.publish("data", data, 60, PRIVATE);
}

void push_button_ok_isr() {
    if(!pb_ok_pressed) {
        pb_ok_pressed = true;
        // TODO: turn on led
    }
}

void push_button_great_isr() {
    if (!pb_great_pressed) {
        pb_great_pressed = true;
        // TODO: turn on led
    }
}

void push_button_not_good_isr() {
    if (pb_not_good_pressed) {
        pb_not_good_pressed = true;
        // TODO: turn on led
    }
}

void check_push_buttons_state() {
    if (pb_ok_pressed) {
        pb_ok_pressed = false;
        Particle.publish("btn", "ok");
        // TODO: turn off button led
    }
    if (pb_great_pressed) {
        pb_great_pressed = false;
        Particle.publish("btn", "grt");
        // TODO: turn off button led
    }
    if (pb_not_good_pressed) {
        pb_not_good_pressed = false;
        Particle.publish("btn", "bad");
        // TODO: turn off button led
    }
}

void display_too_wet() {
    rgbLedTimer.stop();
    turnBlue();
}

void display_water_needed() {
    blinkBlue();
}

void display_portal() {
    blinkYellow();
}

void display_ok() {
    rgbLedTimer.stop();
    turnGreen();
}

void display_error() {
    rgbLedTimer.stop();
    turnRed();
}

void display_inactive() {
    blinkRed();
}

void check_errors() {// TODO: add more checks (i.e., disconnected sensors based on pin value, low battery/charge, faults, etc.)
    if (!Cellular.ready()) {
        turnRed();
    } else {
        executeServerCommand(lastCommand);
    }
}

float getSoilTemp() {
    sensors.requestTemperatures();
    return sensors.getTempFByIndex(0);
}

int getSoilMoisture() {
    return analogRead(PIN_SOIL_MOISTURE);
}

float getAirTemp() {
    return dht.getTempFarenheit();
}

float getAirMoisture() {
    return dht.getHumidity();
}

int getLight() {
    return analogRead(PIN_LIGHT);
}

int executeServerCommand(String command) {
    lastCommand = command;
    if(command == "data") {
        update_variables();
        send_data();
        return 0;
    } else if (command == "portal") {
        display_portal();
        return 1;
    } else if (command == "h2o") {
        display_water_needed();
        return 2;
    } else if (command == "act") { 
        isDeviceActivated = true;
        display_ok();
        return 3;
    } else if (command == "deact") { 
        isDeviceActivated = false;
        return 4;
    } else if (command == "dry") {
        display_too_wet();
        return 5;
    } else if (command == "ok") {
        display_ok();
        return 6;
    } else if (command == "error") {
        display_error();
        return 7;
    } else {
        return -1;
    }
}
