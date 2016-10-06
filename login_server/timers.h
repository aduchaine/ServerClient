/*
4/16/16

timers.h
*/

#ifndef TIMERS_H
#define TIMERS_H


#pragma once

#include "stdafx.h"


// there will likely be issues with data type size and "current_time" when the server is up for long periods - will need to address this
class Timers {

public:

	Timers();

	Timers(int in_timer_time, bool restart_on_check);
	
	void StartTimer(int set_timer_time);
	bool CheckTimer(int timer_time);
	void EnableTimer();
	void DisableTimer();
	bool IsEnabled();

	int timer_time;
	int start_time;	
	bool restart;
	bool enabled;

};

#endif
