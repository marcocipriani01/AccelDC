#include "AccelDC.h"

AccelDC::AccelDC(uint8_t input1, uint8_t input2, uint8_t enable) {
    in1 = input1;
    in2 = input2;
    en = enable;
    accel = 0;
    minSpd = currentSpd = targetSpd = 0;
    moveForMillisVal = 0;
    lastRunTime = 0;
    invertDir = false;
}

void AccelDC::begin() {
    pinMode(in1, OUTPUT);
    analogWrite(in1, 0);
    pinMode(in2, OUTPUT);
    analogWrite(in2, 0);
    pinMode(en, OUTPUT);
    digitalWrite(en, LOW);
}

void AccelDC::setBackwards(bool backwards) {
    invertDir = backwards;
}

void AccelDC::setEnabled(bool enabled) {
    digitalWrite(en, enabled);
}

int AccelDC::write(float mps) {
    currentSpd = constrain(mps, 0, targetSpd);
    if (invertDir) {
        analogWrite(in1, 0);
        analogWrite(in2, MPS_TO_PWM(currentSpd));
    } else {
        analogWrite(in2, 0);
        analogWrite(in1, MPS_TO_PWM(currentSpd));
    }
}

void AccelDC::setConfig(float minSpeed, float acceleration) {
    accel = acceleration;
    minSpd = minSpeed;
}

void AccelDC::setTargetSpeed(float speed) {
    targetSpd = speed;
}

void AccelDC::moveForMillis(unsigned long t) {
    moveForMillisVal = t;
    moveForMillisCurrent = 0;
}

bool AccelDC::run() {
    unsigned long time = millis();
    unsigned long dt = time - lastRunTime;
    if (dt < TIME_RESOLUTION_MS) return;
    lastRunTime = time;

    // Time-based acceleration curve
    if (moveForMillisVal != 0) {
        if (moveForMillisCurrent == 0) {
            write(minSpd);
        } else if (moveForMillisCurrent < (moveForMillisVal / 2)) {
            write(currentSpd + (accel * dt));
        } else if (moveForMillisCurrent < moveForMillisVal) {
            write(currentSpd - (accel * dt));
        } else {
            write(0);
            moveForMillisVal = moveForMillisCurrent = 0;
            return false;
        }
        moveForMillisCurrent += dt;
        return true;
    }
}