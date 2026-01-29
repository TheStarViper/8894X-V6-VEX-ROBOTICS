#pragma once
#include "pros/rtos.hpp"

// Raw helper to command the three intake motors at once.
void intakefunc(int speedmain, int speedscore, int speedmid);

// Optional: returns last "main intake target" convention (legacy sign behavior)
int get_intake_target_speed();

// Utility helpers for common presets. duration_ms <= 0 runs until stopped.
void stopIntakes();
void runIntakeStore(int duration_ms = 0);
void runOuttake(int duration_ms = 0);
void runlowscore(int duration_ms = 0);
void scoreMiddleGoal(int duration_ms = 0);
void scoreHighGoal(int duration_ms = 0);
