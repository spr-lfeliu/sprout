#include "notifications.h"
#include "feedback.h"

#include "spark-dallas-temperature/spark-dallas-temperature.h" // Used for soil temperature sensor
#include "OneWire/OneWire.h" // Used for soil temperature sensor
#include "Adafruit_DHT/Adafruit_DHT.h" // Used for air sensor (air temperature and moisture)

#define BAUD_RATE 57600
#define TIME_DELAY_MILLISECONDS 100

#define PIN_RGB_LED_RED B0
#define PIN_RGB_LED_GREEN B1
#define PIN_RGB_LED_BLUE B2
#define PIN_BUTTON_LED_NEUTRAL C0
#define PIN_BUTTON_LED_POSITIVE C1
#define PIN_BUTTON_LED_NEGATIVE C2

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

// Variables
// GET https://api.particle.io/v1/devices/{DEVICE_ID}/{VARIABLE}?format=raw&access_token=token
// Up to 10 cloud variables may be defined and each variable name is limited to a maximum of 12 characters

double var_air_temperature;
double var_air_humidity;
double var_soil_temperature;
int var_soil_moisture;
int var_light;

Feedback feedback = Feedback(PIN_BUTTON_LED_NEUTRAL, PIN_BUTTON_LED_POSITIVE, PIN_BUTTON_LED_NEGATIVE);
Notifications notifications = Notifications(PIN_RGB_LED_RED, PIN_RGB_LED_GREEN, PIN_RGB_LED_BLUE);
Timer ledTimer(3060, breatheLed);
volatile bool isCycleLed = true;

volatile bool isDeviceActivated = false;
String lastCommand = "";

// Functions
// POST /v1/devices/{DEVICE_ID}/{FUNCTION}
// Up to 4 different cloud functions are supported
// Max of 12 characters for function name/key

int executeServerCommand(String command);

void setup() {
   Serial.begin(BAUD_RATE);
   init_air_sensor();
   init_soil_temp_sensor();
   setup_push_buttons();
   register_variables();
   register_functions();
   ledTimer.start();
   // TODO: need to call endpoint to check if the device has been activated
}

void init_air_sensor() {
    dht.begin();
}

void init_soil_temp_sensor() {
    sensors.begin();
}

void setup_push_buttons() {
   // PB NEUTRAL
   pinMode(D5, INPUT_PULLUP);
   attachInterrupt(D5, push_button_neutral_isr, RISING);
   // PB POSITIVE
   pinMode(D6, INPUT_PULLUP);
   attachInterrupt(D6, push_button_positive_isr, RISING);
   // PB NEGATIVE
   pinMode(D7, INPUT_PULLUP);
   attachInterrupt(D7, push_button_negative_isr, RISING);
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

void loop() {
    if (isDeviceActivated) {
        update_variables();
        check_push_buttons_state(); // TODO: Move to feedback
        check_errors();
    } else {
        display_error();
    }
    if (isCycleLed) {
        isCycleLed = false;
        notifications.DisplayLed();
    }
    delay(TIME_DELAY_MILLISECONDS);
}

void update_variables() {
    var_air_temperature = getAirTemp();
    var_air_humidity = getAirMoisture();
    var_soil_temperature = getSoilTemp();
    var_soil_moisture = getSoilMoisture();
    var_light = getLight();
}

void send_data() {
    update_variables();
    String data = "{" \
    "\"sT\":" + String(var_soil_temperature) + "," \
    "\"sM\":" + String(var_soil_moisture) + "," \
    "\"aT\":" + String(var_air_temperature) + "," \
    "\"aH\":" + String(var_air_humidity) + "," \
    "\"L\":" + String(var_light) + \
    "}";
    Particle.publish("d", data, 60, PRIVATE);
}

void push_button_neutral_isr() {
    if(!feedback.isNeutral()) {
        feedback.ButtonPressed(Feedback::Button::neutral);
    }
}

void push_button_positive_isr() {
    if(!feedback.isPositive()) {
        feedback.ButtonPressed(Feedback::Button::positive);
    }
}

void push_button_negative_isr() {
    if(!feedback.isNegative()) {
        feedback.ButtonPressed(Feedback::Button::negative);
    }
}

void check_push_buttons_state() {
    Feedback::Button state = feedback.GetButtonPressed();
    switch(state) {
        case Feedback::neutral:
            Particle.publish("f", "n");
            break;
        case Feedback::positive:
            Particle.publish("f", "+");
            break;
        case Feedback::negative:
            Particle.publish("f", "-");
            break;
    }
    feedback.Reset();
}

void display_error() {
    notifications.SetNotification(Notifications::Notification::failure);
}

void check_errors() {// TODO: add more checks (i.e., disconnected sensors based on pin value, low battery/charge, faults, etc.)
    if (!Cellular.ready()) {
      display_error();
    } else {
        if (lastCommand != "data") {
            executeServerCommand(lastCommand);
        }
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

void breatheLed() {
    isCycleLed = true;
}

int executeServerCommand(String command) {
    lastCommand = command;
    int return_value = -1;
    if(command == "d") {
        send_data();
        return_value = 0;
    } else if (command == "p") {
        notifications.SetNotification(Notifications::Notification::portal);
        return_value = 1;
    } else if (command == "w-") {
        notifications.SetNotification(Notifications::Notification::water_deficit);
        return_value = 2;
    } else if (command == "a") {
        isDeviceActivated = true;
        return_value = 3;
    } else if (command == "a-") {
        isDeviceActivated = false;
        return_value = 4;
    } else if (command == "w+") {
        notifications.SetNotification(Notifications::Notification::water_excess);
        return_value = 5;
    } else if (command == "k") {
        notifications.SetNotification(Notifications::Notification::all_ok);
        return_value = 6;
    } else if (command == "e") {
        notifications.SetNotification(Notifications::Notification::failure);
        return_value = 7;
    } else if (command == "w0") {
        notifications.SetNotification(Notifications::Notification::water_none);
        return_value = 8;
    }
    return return_value;
}
