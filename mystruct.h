#ifndef STRUCTS_H
#define STRUCTS_H
struct TimedTask{
	
	void (*fp_void)();
	uint32_t last_time;
	uint32_t period_ms;
	uint32_t times_called;
};

struct TimedTask timed_tasks[10];
#endif