#include "main.h"
#include "lemlib/api.hpp" // IWYU pragma: keep
#include "configuration.hpp"
#include "graphics/pages.hpp"

void initialize() {
    //pros::lcd::initialize(); // initialize brain screen
    chassis.calibrate(); // calibrate sensors
    controller.rumble("."); // main systems calibrated


    pros::delay(500);
    brain_menu();
    pros::Task lvgl_handler(lvgl_task, NULL, "LVGL Handler");


    controller.rumble(".-."); // gui operational
    pros::delay(20); // update every 20 ms
}


void disabled() {}

void competition_initialize() {}


ASSET(example_txt);


void autonomous() {

}


void opcontrol() {

    while (true) {
        int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
        int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
        chassis.arcade(leftY, rightX);
        pros::delay(10);
    }
}
