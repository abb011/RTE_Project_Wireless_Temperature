#include "homework4.h"
#include <stdio.h>
#include <math.h>
#include "accelerometers/accelerometers.h"

#define EPSILON 5.0
#define G_1_R 0.0
#define G_1_P 0.0
#define G_2_R 45.0
#define G_2_P 0.0
#define G_3_R 0.0
#define G_3_P 0.0
#define ONE_SECOND 50		//Note 50 calls at 20ms frequency is 1second
#define INDEX_G1 0
#define INDEX_G2 INDEX_G1 +1
#define INDEX_G3 INDEX_G2 +1
#define INDEX_JT_MAX INDEX_G3+1
#define AVG_ARRAY_LEN 5
#define MAX_WRONG 10


extern uint32_t announce_gesture;
extern float roll;
extern float pitch;
extern uint32_t gesture_state;
struct AccelData accel_data_window[AVG_ARRAY_LEN];
uint16_t nw = 0;

void init_gesture_detection(){
	init_accelerometers();
	init_jumpTable();
}

int isGesture1(){
	return((G_1_R+EPSILON> roll)&&(G_1_R-EPSILON<roll))&&((G_1_P+EPSILON>pitch)&&(G_1_P-EPSILON<pitch));
}

int isGesture2(){
	return ((G_2_R+EPSILON> roll)&&(G_2_R-EPSILON<roll))&&((G_2_P+EPSILON>pitch)&&(G_2_P-EPSILON<pitch));
}
int isGesture3(){
	return ((G_3_R+EPSILON> roll)&&(G_3_R-EPSILON<roll))&&((G_3_P+EPSILON>pitch)&&(G_3_P-EPSILON<pitch));
}
void update_state1(){
	static uint32_t counter = 0;
	static uint32_t numWrong = 0;
	if(isGesture1()){
		counter++;
		//Move to the next state and reset counter
	}else if(isGesture2() && counter >= ONE_SECOND){
		numWrong = 0;
		counter = 0;
		gesture_state = INDEX_G2;
		printf("Going to State 2\n");
	}else if(numWrong >= MAX_WRONG){
		numWrong = 0;
		printf("Failed State1: %d\t", counter);
		counter = 0;
		printPitchAndRoll();
	}else{
		numWrong++;
	}
	return;
}

void update_state2(){
	static uint32_t counter = 0;
	static uint32_t numWrong = 0;
	if(isGesture2()){
		counter++;
		//Move to the next state and reset counter
	}else if(isGesture3() && counter >= ONE_SECOND){
		numWrong = 0;
		counter = 0;
		gesture_state = INDEX_G3;
		printf("Going to State3\n");
	}else if(numWrong >= MAX_WRONG){
		numWrong = 0;
		printf("Failed State2: %d\t", counter);
		counter = 0;
		gesture_state = INDEX_G1;
		printPitchAndRoll();
	}else{
		numWrong++;
	}
	return;
}
void update_state3(){
	static uint32_t counter = 0;
	static uint32_t numWrong = 0;
	if(counter >= ONE_SECOND){
		numWrong = 0;
		counter = 0;
		printf("Gesture Detected\n");
		gesture_state = INDEX_G1;
	}else if(isGesture3()){
		counter++;
	}else if(numWrong >= MAX_WRONG){
		printf("Failed State13 %d\t", counter);
		numWrong = 0;
		counter = 0;
		printPitchAndRoll();
		gesture_state = INDEX_G1;
	}else{
		numWrong++;
	}
	return;
}


void (*gesture_jumpTable[INDEX_JT_MAX])();
void init_jumpTable(){
	gesture_jumpTable[INDEX_G1] = update_state1;
	gesture_jumpTable[INDEX_G2] = update_state2;
	gesture_jumpTable[INDEX_G3] = update_state3;
}


float accel_data[3];
void gesture_detection(){
	read_accelerometers(accel_data);
	calc_pitch_roll(accel_data[0], accel_data[1], accel_data[2], &pitch, &roll);
	gesture_jumpTable[gesture_state]();
}

void read_acc(){
	read_accelerometers(accel_data);
}
void gesture_detection_avg(){
	float x,y,z;
	accel_data_window[nw].x = accel_data[0];
	accel_data_window[nw].y = accel_data[1];
	accel_data_window[nw].z = accel_data[2];
	nw = (nw+1)%AVG_ARRAY_LEN;
	calc_average(&x, &y, &z);
	calc_pitch_roll(x,y,z, &pitch, &roll);
	gesture_jumpTable[gesture_state]();
}
void printPitchAndRoll(){
	printf("Pitch = %f, Roll = %f\n", pitch, roll);
}
void __attribute__((weak)) calc_average(float* avg_x, float * avg_y, float * avg_z){
	int i;
	// set them all to 0 to start
	*avg_x = *avg_y = *avg_z =0;
	
	for(i= 0; i<AVG_ARRAY_LEN; i++){
		*avg_x +=accel_data_window[i].x;
		*avg_y +=accel_data_window[i].y;
		*avg_z +=accel_data_window[i].z;
		
	}
	*avg_x = *avg_x/AVG_ARRAY_LEN;
	*avg_y = *avg_y/AVG_ARRAY_LEN;
	*avg_z = *avg_z/AVG_ARRAY_LEN;
}

void calc_pitch_roll(float acc_x, float acc_y, float acc_z, float *pitch, float *roll)
{
	*roll = (180.0/M_PI)*atan2(acc_y, acc_z);
	*pitch = (180.0/M_PI)*atan2(-acc_x, sqrt(acc_y*acc_y+acc_z*acc_z));
}