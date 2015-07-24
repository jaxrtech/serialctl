#pragma once

/*
 * This file is where robot specific hardware code should be defined.
 * For example, below is the code to interface with the pneumatics and
 * Open Source Motor Controller (OSMC) on Goliath
 *
 */

#define P_SENSOR A2
#define CURRENT_OFFSET -2.5
#define PRESSURE_OFFSET 161
#define MIN_PRESS 80
#define MAX_PRESS 100
#define CURRENT_RIGHT A0
#define CURRENT_LEFT A1

#define PSI(x) ((analogRead(x)-(float)PRESSURE_OFFSET)/2.048)

void init_pins();
void print_data();
void osmc_init();
void fast_pwm();
void drive_osmc(int rawpower, unsigned short brake, unsigned short ali, unsigned short bli, unsigned short ahi, unsigned short bhi);
int get_arm_interlock();
