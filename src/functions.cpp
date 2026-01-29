#include "functions.hpp"
#include "configuration.hpp"

#include <cstdint>

namespace {

struct IntakeTargets {
    int main = 0;
    int score = 0;
    int mid = 0;
};

IntakeTargets lastTargets;

void rememberTargets(int speedmain, int speedscore, int speedmid) {
    lastTargets.main = speedmain;
    lastTargets.score = speedscore;
    lastTargets.mid = speedmid;
}

void runPreset(int speedmain, int speedscore, int speedmid, int duration_ms) {
    intakefunc(speedmain, speedscore, speedmid);
    if (duration_ms > 0) {
        pros::delay(duration_ms);
        stopIntakes();
    }
}

} // namespace

// === Intake helpers ===
void intakefunc(int speedmain, int speedscore, int speedmid) {
    rememberTargets(speedmain, speedscore, speedmid);
    intakeMain.move_velocity(speedmain);
    intakescore.move_velocity(speedscore);
    intakemid.move_velocity(speedmid);
}

int get_intake_target_speed() {
    // Positive means "trying to bring rings inward" (legacy convention)
    return -lastTargets.main;
}

void stopIntakes() { intakefunc(0, 0, 0); }

void runIntakeStore(int duration_ms) { runPreset(-600, 0, -600, duration_ms); }
void runOuttake(int duration_ms)     { runPreset( 600, 600, 600, duration_ms); }
void runlowscore(int duration_ms)    { runPreset( 300, 0, 300, duration_ms); }

void scoreMiddleGoal(int duration_ms) { runPreset(-600, 600, -600, duration_ms); }
void scoreHighGoal(int duration_ms)   { runPreset(-600, -600, -600, duration_ms); }
