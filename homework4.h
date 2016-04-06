#ifndef HOMEWORK4_H
#define HOMEWORK4_H


void init_gesture_detection();

void update_state1();

void update_state2();

void update_state3();

void gesture_detection();
void gesture_detection_avg();

void init_jumpTable();

void calc_pitch_roll(float acc_x, float acc_y, float acc_z, float *pitch, float *roll);

void __attribute__((weak)) calc_average(float* avg_x, float * avg_y, float * avg_z);
void read_acc();

void printPitchAndRoll();

struct AccelData{
	float x;
	float y;
	float z;
	
};

#endif