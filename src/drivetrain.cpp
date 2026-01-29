#include "drivetrain.hpp"
#include "configuration.hpp"

#include <algorithm>
#include <cmath>

// ----- CONSTANTS ----- //
static const float CONTROLLER_THROTTLE_CURVE = 1.1f;
static const float CONTROLLER_THROTTLE_DEADBAND = 2.0f;

static const float CONTROLLER_TURN_CURVE = 1.4f;
static const float CONTROLLER_TURN_DEADBAND = 2.0f;

static const float CURVATHERP_INTERP_START = 2.0f;
static const float CURVATHERP_INTERP_END   = 45.0f;
static const float CURVATHERP_TURN_COEFF   = 1.1f;

// ----- SMALL HELPER FUNCTIONS ----- //
static inline float clip(float x, float lo, float hi) {
    return std::max(lo, std::min(hi, x));
}

static inline int deadband(int value, float band) {
    return (std::abs(value) < band) ? 0 : value;
}

// Nonlinear joystick curve: keeps small inputs gentle, large inputs strong
static float pilonCurve(int input, float maxInput, float exponent) {
    if (input == 0) return 0.0f;
    float normalized = static_cast<float>(input) / maxInput;  // -1..1
    float sign = (normalized >= 0.0f) ? 1.0f : -1.0f;
    float mag  = std::pow(std::fabs(normalized), exponent);
    return sign * mag * maxInput; // back to -max..max
}

// ----- ARCADE MATH ----- //
DriveOutput calc_arcade(int throttle, int turn) {
    // Deadband then curve inputs
    float curved_throttle = pilonCurve(deadband(throttle, CONTROLLER_THROTTLE_DEADBAND),
                                       127.0f, CONTROLLER_THROTTLE_CURVE);
    float curved_turn     = pilonCurve(deadband(turn, CONTROLLER_TURN_DEADBAND),
                                       127.0f, CONTROLLER_TURN_CURVE);

    // Basic arcade
    float left  = curved_throttle + curved_turn;
    float right = curved_throttle - curved_turn;

    // Normalize to [-127, 127]
    float ratio = std::max(std::fabs(left), std::fabs(right)) / 127.0f;
    if (ratio > 1.0f) {
        left  /= ratio;
        right /= ratio;
    }

    return {left, right};
}

// ----- CURVATHERP MATH ----- //
DriveOutput calc_curvatherp(int throttle, int turn) {
    // 1) Apply deadband
    int throttle_db = deadband(throttle, CONTROLLER_THROTTLE_DEADBAND);
    int turn_db     = deadband(turn,     CONTROLLER_TURN_DEADBAND);

    // 2) Curve inputs
    double forward_stick = pilonCurve(throttle_db, 127.0f, CONTROLLER_THROTTLE_CURVE);
    double turn_stick    = pilonCurve(turn_db,     127.0f, CONTROLLER_TURN_CURVE);

    // Compute blend factor based on forward magnitude
    double interpolator =
        (std::fabs(forward_stick) - CURVATHERP_INTERP_START) /
        (CURVATHERP_INTERP_END - CURVATHERP_INTERP_START);
    interpolator = clip(static_cast<float>(interpolator), 0.0f, 1.0f);

    turn_stick *= CURVATHERP_TURN_COEFF;

    double forward_clamped = forward_stick / 127.0;
    double turn_clamped    = turn_stick    / 127.0;

    // Curvature-style output
    double left_curvature =
        forward_clamped + std::fabs(forward_clamped) * turn_clamped;
    double right_curvature =
        forward_clamped - std::fabs(forward_clamped) * turn_clamped;
    double max_curv = std::max(std::fabs(left_curvature), std::fabs(right_curvature));

    if (max_curv > 1.0) {
        left_curvature  /= max_curv;
        right_curvature /= max_curv;
    }

    // Tank-style output
    double left_tank  = clip(static_cast<float>(forward_clamped + turn_clamped), -1.0f, 1.0f);
    double right_tank = clip(static_cast<float>(forward_clamped - turn_clamped), -1.0f, 1.0f);

    // Blend between curvature and tank
    double leftPower =
        left_curvature  * interpolator + left_tank  * (1.0 - interpolator);
    double rightPower =
        right_curvature * interpolator + right_tank * (1.0 - interpolator);

    constexpr double DRIVE_SCALE = 1.0; // tune later when needed

    return { static_cast<float>(leftPower  * 127.0 * DRIVE_SCALE),
             static_cast<float>(rightPower * 127.0 * DRIVE_SCALE) };
}

void drive_distance_inches(double inches, int speed) {
    constexpr double wheel_diam_inches = 3.25;
    constexpr double tolerance_deg = 5.0; // degrees of error allowed
    constexpr double degrees_per_rev = 360.0;
    constexpr double pi = 3.14159265358979323846;

    const double wheel_circumference = wheel_diam_inches * pi;
    const double target_deg = inches / wheel_circumference * degrees_per_rev;

    left_mg.tare_position();
    right_mg.tare_position();

    left_mg.move_relative(target_deg, speed);
    right_mg.move_relative(target_deg, speed);

    pros::delay(20); // let motors start

    while (true) {
        double left_pos  = left_mg.get_position();
        double right_pos = right_mg.get_position();

        bool left_done  = std::fabs(left_pos  - target_deg) < tolerance_deg;
        bool right_done = std::fabs(right_pos - target_deg) < tolerance_deg;

        if (left_done && right_done) break;
        pros::delay(10);
    }
}
