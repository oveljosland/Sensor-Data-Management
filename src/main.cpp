//Sensor Data Management.ino
// Planning to add movingavg

#include <Arduino.h>

// Functions that will be used
void smooth(int sensor_pin, int num_readings, String sensor_name);
void calibrate(int sensor_pin, unsigned long calibrate_time, int &sensor_max, int &sensor_min); // Used &sensor_max and &sensor_min to allow for update of the variables.
void read_sensor(int sensor_pin, int sensor_min, int sensor_max);

// Variables
int sensor_min = 1023;  
int sensor_max = 0;
int sensor_pin = A0;
String sensor_name = "Photoresistor";
unsigned long calibrate_time = 10000;

// Readings Array
int num_readings = 10;
int readings[10];
int read_index = 0;

unsigned long last_reading_time = 0;
unsigned long reading_interval = 100;

void setup() {
  Serial.begin(9600);
  calibrate(sensor_pin, calibrate_time, sensor_min, sensor_max);
}

void loop() {
  read_sensor(sensor_pin, sensor_min, sensor_max);
  smooth(sensor_pin, num_readings, sensor_name);
}

void smooth(int sensor_pin, int num_readings, String sensor_name) {
    int total = 0;
    int average = 0;

    for (int current_reading = 0; current_reading < num_readings; current_reading++) {
        readings[current_reading] = 0;
    }

    while (true) {
        unsigned long current_time = millis();

        if (current_time - last_reading_time >= reading_interval) {
            total = total - readings[read_index];
            readings[read_index] = analogRead(sensor_pin);
            total = total + readings[read_index];
            read_index = (read_index + 1) % num_readings;

            average = total / num_readings;
            Serial.print(sensor_name);
            Serial.print(" average: ");
            Serial.println(average);

            last_reading_time = current_time;

            /* 
            Pseudo code
            if (serial input = quit) {
                break;
            */
        }
    }
}

void read_sensor(int sensor_pin, int sensor_min, int sensor_max) {
  int sensor_value = analogRead(sensor_pin);
  sensor_value = constrain(sensor_value, sensor_min, sensor_max);
  int mapped_value = map(sensor_value, sensor_min, sensor_max, 0, 1000);
  Serial.println(mapped_value);
}

void calibrate(int sensor_pin, unsigned long calibrate_time, int &sensor_min, int &sensor_max) {
    int sensor_value = 0;
    unsigned long calibration_start_time = millis();

    while (millis() - calibration_start_time < calibrate_time) {
        sensor_value = analogRead(sensor_pin);
        if (sensor_value > sensor_max) {
            sensor_max = sensor_value;
        }
        if (sensor_value < sensor_min) {
            sensor_min = sensor_value;
        }
    }
}

/* 
Make a sensor-array and a function that iterates over the whole array instead of calling the same function on multiple sensors.

void define_sensors() {
  for(int current_sensor = 0; current_sensor < num_sensors; current_sensor++) {
    current_sensor = output[current_sensor]
  }
}
*/
