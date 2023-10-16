// Sensor Data Management.ino
// Can currently only handle 1 sensor.
// Will add support for multiple sensors soon.
#include <Arduino.h>


// Sensors
String sensor_name = "insert_name";    // Sensor label
const int sensor_pin = A0;             // Sensor pin
int sensor_min = 1023;                 // minimum sensor value
int sensor_max = 0;                    // maximum sensor value

// Array
int num_readings = 10;
int* readings = nullptr;
static int read_index = 0;

// Intervals, Frequency
const unsigned long calibrate_duration = 10000;  // Calibration run-time in milliseconds
const unsigned long reading_duration = 60000;    // Reading run-time in milliseconds
const unsigned long reading_frequency = 100;     // Reading frequency, reads/millisecond

// Tracking variables
unsigned long calibration_start_time = 0;        // Keeps track of calibration run-time
unsigned long reading_start_time = 0;            // Keeps track of reading run-time
static unsigned long last_reading_time = 0;      // Keeps track of last read

// Functions
void calculate_moving_average(int sensor_pin, int* readings, int num_readings, String sensor_name);
void calibrate(int sensor_pin, unsigned long calibrate_duration, int &sensor_max, int &sensor_min);
void read_sensor(int sensor_pin, int &sensor_min, int &sensor_max);
void cleanup();

// States
enum State {
    CALIBRATION, // Runs the 'calibrate' function
    READING,     // Runs the 'read_sensor' and 'calulate_moving_average' functions
    CLEANUP,     // Runs the 'cleanup' function and wipes the sensor data array
};
State current_state = CALIBRATION; // Initial state



void setup() {
    Serial.begin(9600);                // Initialise serial communication
    readings = new int[num_readings];  // Create readings array from pointer
}

void loop() {
    unsigned long current_time = millis();

    switch (current_state) {
        case CALIBRATION:
            calibrate(sensor_pin, calibrate_duration, sensor_max, sensor_min);           // Calibrate the sensor
            if (current_time - calibration_start_time >= calibrate_duration) {           // Change state after specified calibration time has passed
                current_state = READING;
                reading_start_time = current_time;
            }
            break;

        case READING:
            read_sensor(sensor_pin, sensor_min, sensor_max);                             // Read the sensor
            calculate_moving_average(sensor_pin, readings, num_readings, sensor_name);   // Calculate the moving average
            if (current_time - reading_start_time >= reading_duration) {                 // Change state after specified reading time has passed
                current_state = CLEANUP;
            }
            break;

        case CLEANUP:
            cleanup();
            current_state = CALIBRATION;
            break;
    }
}

void calculate_moving_average(int sensor_pin, int* readings, int num_readings, String sensor_name) {
    unsigned long current_time = millis(); // Added current_time variable
    int total = 0;

    if (current_time - last_reading_time >= reading_frequency) {
        total = total - readings[read_index];
        readings[read_index] = analogRead(sensor_pin);
        total += readings[read_index];

        read_index = (read_index + 1) % num_readings;

        float moving_average = (float)total / num_readings;

        Serial.print(sensor_name);
        Serial.print(" moving average: ");
        Serial.println(moving_average);

        last_reading_time = current_time;
    }
}

void read_sensor(int sensor_pin, int &sensor_min, int &sensor_max) {
    int sensor_value = analogRead(sensor_pin);
    sensor_value = constrain(sensor_value, sensor_min, sensor_max);
    int mapped_value = map(sensor_value, sensor_min, sensor_max, 0, 1000);
}

void calibrate(int sensor_pin, int &sensor_max, int &sensor_min) {
    int sensor_value = analogRead(sensor_pin);

    if (sensor_value > sensor_max) {
        sensor_max = sensor_value;
    }
    if (sensor_value < sensor_min) {
        sensor_min = sensor_value;
    }
}

void cleanup() {
    delete[] readings; // Deletes the array
}
