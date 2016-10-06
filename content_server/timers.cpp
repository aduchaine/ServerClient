/*
4/16/16

timers.h
*/

#include "stdafx.h"

#include <time.h>

#include "timers.h"


int current_time = 0;

int end_time = 0; // for logging/testing, not necessary atm


Timers::Timers()
{
	timer_time = 0;
	start_time = current_time;	
	enabled = false;
	restart = false;
}

Timers::Timers(int in_timer_time, bool restart_on_check)
{
	current_time = clock();
	start_time = current_time;
	timer_time = in_timer_time;
	
	if (restart_on_check == true) {
		restart = true;
	}
	else {
		restart = false;
	}
	if (timer_time == 0) {
		enabled = false;
	}
	else {
		enabled = true;
	}
}

void Timers::StartTimer(int set_timer_time)
{
	current_time = clock();
	start_time = current_time;
	end_time = start_time + timer_time;
	enabled = true;
	//std::cout << "\n StartTimer: current:(" << current_time << ") start:(" << start_time << ") timer:(" << timer_time << ") end:(" << end_time << ")" << std::endl; // heavy debugging
}

bool Timers::CheckTimer(int timer_time)
{
	if (enabled == false) {
		return false;
	}
	current_time = clock();

	if (restart == true && current_time - start_time > timer_time) {
		start_time = current_time;
		end_time = start_time + timer_time;
		//std::cout << "\n CheckTimer: restart(true) - current:(" << current_time << ") start:(" << start_time << ") timer:(" << timer_time << ") end:(" << end_time << ")" << std::endl; // heavy debugging
		return true;
	}
	else if (restart == false && current_time - start_time > timer_time) {		
		//std::cout << "\n CheckTimer: restart(false) - current:(" << current_time << ") start:(" << start_time << ") timer:(" << timer_time << ") end:(" << end_time << ")" << std::endl; // heavy debugging
		enabled = false;
		return true;
	}
	else {
		return false;
	}
}

// this could work to disable a timer outside of a function where it is checked, thereby setting conditions for something
void Timers::DisableTimer()
{
	enabled = false;
}

void Timers::EnableTimer()
{
	enabled = true;
}

bool Timers::IsEnabled()
{
	if (enabled == true) {
		return true;
	}
	return false;
}
