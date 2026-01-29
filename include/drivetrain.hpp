#pragma once
#include "main.h"

struct DriveOutput {
    float left;
    float right;
};

// Curvatherp blend: curvature + tank based on forward magnitude
// Inputs are joystick values in [-127, 127]
DriveOutput calc_curvatherp(int throttle, int turn);

// Basic arcade (curved joystick response)
DriveOutput calc_arcade(int throttle, int turn);

// Drive a straight-line distance (in inches) using only drivetrain motor encoders
void drive_distance_inches(double inches, int speed = 100);
