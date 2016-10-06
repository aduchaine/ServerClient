/*
4/16/16

timers.h
*/

#ifndef TIMERS_H
#define TIMERS_H


#pragma once

#include "stdafx.h"


class Timers {

public:

	Timers();

	Timers(int in_timer_time, bool restart_on_check);
	
	void StartTimer(int set_timer_time = 0);
	bool CheckTimer(int timer_time);
	void EnableTimer();
	void DisableTimer();

	int timer_time;
	int start_time;	
	bool restart;
	bool enabled;

};

#endif
