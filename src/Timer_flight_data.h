
/* Generated from GenMyModel */

#ifndef DEF_TIMER_FLIGHT_DATA
#define DEF_TIMER_FLIGHT_DATA

//#include <iostream>
#include <Arduino.h>
#include <stdint.h>

#include <sys/time.h>
#include "../keeper.h"
#include "GTimer.h"
#include "Timer_flight_data.h"

#define SYNCHRO_ALLOWANCE_MICROS 200000 //0.1 secs
#define LANDING_ALLOWANCE_MICROS 200000 //0.1 secs

#define SECONDS_2021 1612137600  //Date and time (GMT):> Monday, 1 February 2021 0:00:00
#define SECONDS_2038 2147483648  //Date and time (GMT):> , 19 enero 2038 0:00:00
 

#define LINK_TRAG "http://gliderscore.com/scoringdatamanage.aspx?ACTION=ImportScore&"
//#define SCHED_TRAG "http://GliderScore.com/ScoringDataManage.aspx?ACTION=DownloadGTSchedule&ID=814ebb8af9a"
// #define SCHED_TRAG "http://GliderScore.com/ScoringDataManage.aspx?ACTION=DownloadGTSchedule&ID=58B6a016f0bf"
#define SCHED_TRAG "http://GliderScore.com/ScoringDataManage.aspx?ACTION=DownloadGTSchedule&ID="
//#define LINK_TRAG "http://gs.gliderlink.net/api/addScore?"
#define TIMER_TRAG "http://192.168.4.1/timer?"

typedef struct Splitted
{String split;
String cacho;
String numero;};


typedef struct timeshift{
	int tv_sec;
	uint32_t tv_usec;
};
class Timer_flight_data
{
public:
	struct datastore tfd;    
	struct datastore next_tfd;
	char name_in_keeper[12];
	char nick_in_keeper[5];
	struct Splitted split;
	struct timeshift timer_to_keeper;
	struct timeshift landing_shift;   
    struct timeshift landing_allowance;
    struct timeval keeper_takeoff_time;
    struct timeval keeper_launch_time; //time the horn sounds in keeper time
    struct timeval keeper_landing_time;
    struct timeval keeper_transmit_time;
    struct timeval keeper_landing_timeout;
    bool bothRTC = 0;
    bool keeperRTC = 0;
    bool timerRTC = 0;
	uint8_t keeperIP[4];
	String TC;
	void begin();
	void set_keeperIP(String arg);
	void set_FLTnum(String arg);
	void set_flight_mins(String arg);
	void set_flight_secs(String arg);
	void set_F5J_millis(String arg);
	void set_F5J_height(String arg);
	void set_take_off_millis(String arg);
	void set_throttle_shut_millis(String arg);
	void set_landing_millis(String arg);
	void set_takeoff_local_secs(String argsecs);
	void set_takeoff_local_microsecs(String argmillisecs);
	void set_transmit_local_secs(String argmillisecs);
	void set_transmit_local_microsecs(String argmillisecs);
	void set_rest_millis(String arg);
	void set_Firm_ver(String arg);
	void set_serialnumber(String arg);
	void set_emergency_used(String arg);
	void set_RTCtimed(String arg);
	void set_from_NVS(String arg);
	void set_boolpad(String arg);
	void set_timedout(bool timedout);
	void set_landing_dist(uint distance);
	void set_name_in_keeper(String arg);
	void set_nick_in_keeper(String arg);
	uint32_t assign_pilot_and_round();
	uint32_t check_landing_moment();
	bool check_sychro_needed();
	void datastore2String(struct datastore mytfd , bool separator, char* mypayload);
	String last_launch_payload();
	void print_tfd(struct datastore tfd);
	void sprint_tfd(struct datastore tfd, char *payload);
	timeshift compare_times(timeval before, timeval after);
	timeval shift_time(timeval origin, timeshift shift);
	timeshift shift_time(timeshift origin, timeshift shift);
	Splitted splitPL(String payload);
	bool str_splitPL(String payload);
	bool payload2tfd (String payload);
	void create_dummy_group_data(int round, int group);
	

};

#endif
