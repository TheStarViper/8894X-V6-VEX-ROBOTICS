#include "main.h"
#include "lemlib/api.hpp" // IWYU pragma: keep
#include "configuration.hpp"
#include "graphics/pages.hpp"
#include "liblvgl/lvgl.h"
#include "pros/abstract_motor.hpp"
#include "pros/misc.h"
#include "pros/adi.hpp"
#include "pros/misc.hpp"
#include "pros/rtos.hpp"
bool debugmode = false; // set to true to enable debug features

void initialize() {
  
  chassis.calibrate(); // calibrate sensors
  chassis.setPose(0,0,0);
  //init_sorter_sensor();
  controller.rumble("."); // main systems calibrated
  //init_sorter_sensor();
  

  if (debugmode) {
    pros::lcd::initialize();
    pros::delay(500);
    //pros::Task poseDebug(poseDebugTask, nullptr, "Pose Debug Task");
    //pros::Task brainAutonButton(brainAutonButtonTask, nullptr, "Brain Auton Button");
  } else{
    pros::delay(500);
    brain_menu();
    pros::Task lvgl_handler(lvgl_task, NULL, "LVGL Handler");
  }
  controller.rumble(".-."); // gui operational
  pros::delay(20); // update every 20 ms
}



void disabled() {}

void competition_initialize() {}

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
