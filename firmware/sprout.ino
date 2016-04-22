// This #include statement was automatically added by the Particle IDE.
#include "spark-dallas-temperature/spark-dallas-temperature.h"

// This #include statement was automatically added by the Particle IDE.
#include "OneWire/OneWire.h"

// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_DHT/Adafruit_DHT.h"

#define BAUD_RATE 57600
#define TIME_DELAY_MILLISECONDS 5000

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
float air_humidity;
float air_temperature;

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
// GET https://api.particle.io/v1/devices/{DEVICE_ID}/{VARIABLE}?access_token=token
// Up to 10 cloud variables may be defined and each variable name is limited to a maximum of 12 characters

double var_air_temperature;
double var_air_humidity;
double var_soil_temperature;
int var_soil_moisture;
int var_light;

bool isFirstRun = true;
bool isDeviceActive = false;

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
    Particle.function("execute", executeServerCommand);
}

void register_variables() {
    Particle.variable("airTemp", var_air_temperature);
    Particle.variable("airRH", var_air_humidity);
    Particle.variable("soilTemp", var_soil_temperature);
    Particle.variable("soilMoisture", var_soil_moisture);
    Particle.variable("light", var_light);
}

void loop() {
    if (isDeviceActive) {
        read_data();
        send_data();
        check_push_buttons_state();
        update_variables();
    } else {
        display_error();
    }
    delay(TIME_DELAY_MILLISECONDS);
}

void update_variables() {
    if (isFirstRun) {
        isFirstRun = false;
        var_air_temperature = getAirTemp();
        var_air_humidity = getAirMoisture();
        var_soil_temperature = getSoilTemp();
        var_soil_moisture = getSoilMoisture();
        var_light = getLight();
    } else {
        double temp_air_temp = getAirTemp();
        if (abs(var_air_temperature - temp_air_temp) >= 1.0 ) {
            var_air_temperature = temp_air_temp;
        }
        double temp_air_humidity = getAirMoisture();
        if (abs(var_air_humidity - temp_air_humidity) >= 1.0) {
            var_air_humidity = temp_air_humidity;
        }
        double temp_soil_temp = getSoilTemp();
        if (abs(var_soil_temperature - temp_soil_temp) >= 1.0) {
            var_soil_temperature = temp_soil_temp;
        }
        int temp_soil_moisture = getSoilMoisture();
        if (abs(var_soil_moisture - temp_soil_moisture) >= 25) {
            var_soil_moisture = temp_soil_moisture;
        }
        int temp_light = getLight();
        if (abs(var_light - temp_light) >= 41) {
            var_light = temp_light;
        }
    }
}

void read_data() {
    read_air_sensor();
}

void send_data() {
    String data = "{" \
    "\"deviceId\":\"" + System.deviceID() + "\"," \
    "\"soilTemperature\":" + String(getSoilTemp()) + "," \
    "\"soilMoisture\":" + String(getSoilMoisture()) + "," \
    "\"airTemperature\":" + String(air_temperature) + "," \
    "\"airMoisture\":" + String(air_humidity) + "," \
    "\"light\":" + String(getLight()) + \
    "}";
    Particle.publish("sprout-data", data, 60, PRIVATE);
}

void read_air_sensor() {
  air_humidity = getAirMoisture();
  air_temperature = getAirTemp();
  if (isnan(air_humidity) || isnan(air_temperature)) {
      air_temperature = -500.0;
      air_humidity = -1;
  }
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
        Particle.publish("button-pressed", "ok");
        // TODO: turn off button led
    }
    if (pb_great_pressed) {
        pb_great_pressed = false;
        Particle.publish("button-pressed", "great");
        // TODO: turn off button led
    }
    if (pb_not_good_pressed) {
        pb_not_good_pressed = false;
        Particle.publish("button-pressed", "not-good");
        // TODO: turn off button led
    }
}

void display_too_wet() {
    // TODO
}

void display_water_needed() {
    // TODO
}

void display_portal() {
    // TODO
}

void display_ok() {
    // TODO
}

void display_error() {
    // TODO
}

float getSoilTemp() {
    sensors.requestTemperatures();
    return 9.0/5.0 * sensors.getTempCByIndex(0) + 32;
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
    if(command == "sprout") {
        send_data();
        return 0;
    } else if (command == "portal") {
        display_portal();
        return 1;
    } else if (command == "water") {
        display_water_needed();
        return 2;
    } else if (command == "activate") { 
        isDeviceActive = true;
        return 3;
    } else if (command == "deactivate") { 
        isDeviceActive = false;
        return 4;
    } else if (command == "dry") {
        display_too_wet();
        return 5;
    } else {
        return -1;
    }
}
