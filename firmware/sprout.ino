// This #include statement was automatically added by the Particle IDE.
#include "HttpClient/HttpClient.h"

// This #include statement was automatically added by the Particle IDE.
#include "Adafruit_DHT/Adafruit_DHT.h"

#define BAUD_RATE 57600

#define TIME_DELAY 15000
#define WAIT delay(TIME_DELAY);

// Soil moisture sensor
#define PIN_SOIL_MOISTURE A0
#define READ_SOIL_MOISTURE analogRead(PIN_SOIL_MOISTURE)
#define SOIL_MOISTURE String(READ_SOIL_MOISTURE)

// Air temperature and moisture sensor
// Values are converted.
// Temperature is in degrees Farenheit.
// Moisture is given as Relative humidity (accurate between )
#define DHT_TYPE DHT11
#define DHT_PIN D0 
#define INIT_AIR_SENSOR dht.begin();
#define READ_AIR_HUMIDITY dht.getHumidity();
#define READ_AIR_TEMPERATURE dht.getTempFarenheit();

// Light sensor
// Raw value will be between 0 (low light) and 4095 (more light)
#define PIN_LIGHT A1
#define READ_LIGHT analogRead(PIN_LIGHT)
#define LIGHT String(READ_LIGHT)

// Push buttons
#define CHECK_PUSH_BUTTONS check_push_buttons_state();
volatile bool pb_ok_pressed = false;

float air_humidity;
float air_temperature;

DHT dht(DHT_PIN, DHT_TYPE); 

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
   pinMode(D1, INPUT);
   attachInterrupt(D1, push_button_ok_isr, CHANGE);
   request.hostname = "45.33.89.143";
   request.port = 8080;
   request.path = "/sprout/data";
}

void loop() {
    read_air_sensor();

    request.body = "{" \
    "\"deviceId\":\"1f0031000347343337373737\"," \
    "\"soilTemperature\":" + String(0) + "," \
    "\"soilMoisture\":" + SOIL_MOISTURE + "," \
    "\"airTemperature\":" + String(air_temperature) + "," \
    "\"airMoisture\":" + String(air_humidity) + "," \
    "\"light\":" + LIGHT + \
    "}";
    http.post(request, response, headers);
    Particle.publish("Http response status", String(response.status));
    CHECK_PUSH_BUTTONS
    WAIT
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

void check_push_buttons_state() {
    if (pb_ok_pressed) {
        pb_ok_pressed = false;
        // TODO: Call push button endpoint
    }
}
