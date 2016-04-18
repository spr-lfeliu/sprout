// This #include statement was automatically added by the Particle IDE.
#include "spark-dallas-temperature/spark-dallas-temperature.h"

// This #include statement was automatically added by the Particle IDE.
#include "OneWire/OneWire.h"

// This #include statement was automatically added by the Particle IDE.
#include "HttpClient/HttpClient.h"

// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_DHT/Adafruit_DHT.h"

#define BAUD_RATE 57600

#define TIME_DELAY 14400000
#define WAIT delay(TIME_DELAY);

#define READ_DATA read_data();
#define SEND_DATA send_data();

// Soil moisture sensor
// The moisture sensor raw data values vary between 0 and about 2500. 
// Low values mean less moisture while higher values mean more moisture.
#define PIN_SOIL_MOISTURE A0
#define READ_SOIL_MOISTURE analogRead(PIN_SOIL_MOISTURE)
#define SOIL_MOISTURE String(READ_SOIL_MOISTURE)

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
#define INIT_AIR_SENSOR dht.begin();
#define READ_AIR_HUMIDITY dht.getHumidity();
#define READ_AIR_TEMPERATURE dht.getTempFarenheit();
#define AIR_TEMPERATURE String(air_temperature)
#define AIR_HUMIDITY String(air_humidity)

DHT dht(DHT_PIN, DHT_TYPE); 

// Light sensor
// Raw value will be between 0 (low light) and 4095 (more light)
#define PIN_LIGHT A1
#define READ_LIGHT analogRead(PIN_LIGHT)
#define LIGHT String(READ_LIGHT)

// Push buttons
#define CHECK_PUSH_BUTTONS check_push_buttons_state();
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
String VAR_AIR_TEMP = "airTemp";
String VAR_AIR_RH = "airRH";
String VAR_SOIL_TEMP = "soilTemp";
String VAR_SOIL_MOISTURE = "soilMoisture";
String VAR_LIGHT = "light";

bool isFirstRun = true;

// Functions
// POST /v1/devices/{DEVICE_ID}/{FUNCTION}
// Up to 4 different cloud functions are supported
// Max of 12 characters for function name/key

int executeServerCommand(String command);

// Http
HttpClient http;

http_header_t headers[] = {
    { "Content-Type", "application/json" },
    { NULL, NULL }
};

http_request_t request;
http_response_t response;

void setup() {
   Serial.begin(BAUD_RATE);
   INIT_AIR_SENSOR
   setup_push_buttons();
   setup_http_request();
   register_variables();
   register_functions();
   sensors.begin(); // soil temp sensor
}

void register_functions() {
    Particle.function("execute", executeServerCommand);
}

void register_variables() {
    Particle.variable(VAR_AIR_TEMP, var_air_temperature);
    Particle.variable(VAR_AIR_RH, var_air_humidity);
    Particle.variable(VAR_SOIL_TEMP, var_soil_temperature);
    Particle.variable(VAR_SOIL_MOISTURE, var_soil_moisture);
    Particle.variable(VAR_LIGHT, var_light);
}

void setup_http_request() {
   request.hostname = "45.33.89.143";
   request.port = 8080;
   request.path = "/sprout/data";
}

void setup_push_buttons() {
   // PB OK 
   pinMode(D1, INPUT);
   attachInterrupt(D1, push_button_ok_isr, CHANGE);
   // PB GREAT
   pinMode(D2, INPUT);
   attachInterrupt(D2, push_button_great_isr, CHANGE);
   // PB NOT GOOD
   pinMode(D3, INPUT);
   attachInterrupt(D3, push_button_not_good_isr, CHANGE);
}

void loop() {
    READ_DATA
    SEND_DATA
    CHECK_PUSH_BUTTONS
    update_variables();
    WAIT
}

void update_variables() {
    if (isFirstRun) {
        isFirstRun = false;
        var_air_temperature = READ_AIR_TEMPERATURE
        var_air_humidity = READ_AIR_HUMIDITY
        var_soil_temperature = getSoilTemp();
        var_soil_moisture = READ_SOIL_MOISTURE;
        var_light = READ_LIGHT;
    } else {
        double temp_air_temp = READ_AIR_TEMPERATURE
        if (abs(var_air_temperature - temp_air_temp) >= 1.0 ) {
            var_air_temperature = temp_air_temp;
        }
        double temp_air_humidity = READ_AIR_HUMIDITY
        if (abs(var_air_humidity - temp_air_humidity) >= 1.0) {
            var_air_humidity = temp_air_humidity;
        }
        double temp_soil_temp = getSoilTemp();
        if (abs(var_soil_temperature - temp_soil_temp) >= 1.0) {
            var_soil_temperature = temp_soil_temp;
        }
        int temp_soil_moisture = READ_SOIL_MOISTURE;
        if (abs(var_soil_moisture - temp_soil_moisture) >= 25) {
            var_soil_moisture = temp_soil_moisture;
        }
        int temp_light = READ_LIGHT;
        if (abs(var_light - temp_light) >= 41) {
            var_light = temp_light;
        }
    }
}

void read_data() {
    read_air_sensor();
}

void send_data() {
    request.body = "{" \
    "\"deviceId\":\"" + System.deviceID() + "\"," \
    "\"soilTemperature\":" + getSoilTemp() + "," \
    "\"soilMoisture\":" + SOIL_MOISTURE + "," \
    "\"airTemperature\":" + AIR_TEMPERATURE + "," \
    "\"airMoisture\":" + AIR_HUMIDITY + "," \
    "\"light\":" + LIGHT + \
    "}";
    http.post(request, response, headers);
    Particle.publish("sprout-data-test", request.body, 60, PRIVATE);
}

void read_air_sensor() {
  air_humidity = READ_AIR_HUMIDITY
  air_temperature = READ_AIR_TEMPERATURE
  if (isnan(air_humidity) || isnan(air_temperature)) {
      air_temperature = -500.0;
      air_humidity = -1;
  }
}

void push_button_ok_isr() {
    pb_ok_pressed = true;
}

void push_button_great_isr() {
    pb_great_pressed = true;
}

void push_button_not_good_isr() {
    pb_not_good_pressed = true;
}

void check_push_buttons_state() {
    if (pb_ok_pressed) {
        pb_ok_pressed = false;
        // TODO: Call push button endpoint
    }
    if (pb_great_pressed) {
        pb_great_pressed = false;
        // TODO: Call push button endpoint
    }
    if (pb_not_good_pressed) {
        pb_not_good_pressed = false;
        // TODO: Call push button endpoint
    }
}

void display_too_wet() {
    // TODO
}

void display_ok() {
    // TODO
}

float getSoilTemp() {
    sensors.requestTemperatures();
    return 9.0/5.0 * sensors.getTempCByIndex(0) + 32;
}

int executeServerCommand(String command) {
    if(command == "sprout") {
        send_data();
        return 0;
    } else {
        return -1;
    }
}
