#include <math.h>
#include <TimerOne.h>

/* pins */
int trigpwm = 9;

float rate = 32*120.0; // Hz
unsigned int pulse_time = 4; // microseconds
unsigned int period = 1000000.0 / rate; // microseconds
unsigned int duty = ceil(1023.0 * ((float)pulse_time / (float)period));

void setup() {
    pinMode(trigpwm, OUTPUT);

    digitalWrite(trigpwm, LOW);

    Timer1.initialize();
    Timer1.pwm(trigpwm, duty, period);
    
//    Serial.begin(9600);
}

void loop() {
//  Serial.println(period);
}
