

#ifndef DEF_FLIGHT_MNGR
#define DEF_FLIGHT_MNGR

#define NOTE_D0 -1
#define NOTE_D1 294
#define NOTE_D2 330
#define NOTE_D3 350
#define NOTE_D4 393
#define NOTE_D5 441
#define NOTE_D6 495
#define NOTE_D7 556

#define NOTE_DL1 147
#define NOTE_DL2 165
#define NOTE_DL3 175
#define NOTE_DL4 196
#define NOTE_DL5 221
#define NOTE_DL6 248
#define NOTE_DL7 278

#define NOTE_DH1 589
#define NOTE_DH2 661
#define NOTE_DH3 700
#define NOTE_DH4 786
#define NOTE_DH5 882
#define NOTE_DH6 990
#define NOTE_DH7 112

#define MAX_BEEP_POINTS 80
#define ARCH 0
#define BIP_ 1
#define BEEP 2
#define BOP_ 3
#define BOOP 4
#define BPBP 5
#define PLAY 6
#define STOP 7
#define _ADV 8
#define LAUNCH 1
#define LANDING 2

#define RXD1 5
#define TXD1 2
#define SECOND_DIVIDER 250000
#define MP3_LAG 180000 // microseconds time from start of uart cmd to real noise generated
#define SCHEDULED_PRE_NOTICE 310
#define SCHEDULED_PRE_FLT_SEPARATION 120
#define NO_PANEL 0
#define I2C_IF 1
#define STANDARD_SERIAL_IF 2
#define EXTENDED_SERIAL_IF 3
#define TIMER_STATE_ST 0
#define TIMER_STATE_PT 1
#define TIMER_STATE_WT 2
#define TIMER_STATE_LT 3

#include <Arduino.h>
#include <stdint.h>

#include <sys/time.h>
#include <SPIFFS.h>
#include <M5Stack.h>
#include "GTimer.h"
#include "CompetitionSavedRW.h"
#include "MCP23017Display.h"
typedef struct alarm_table
{
	// uint32_t index;
	uint32_t event;
	int32_t seconds_to_event;
	uint32_t beep_type;
	uint32_t millis_for_beep;
	struct timeval local_secs_for_beep;
	uint32_t volume;
	int file_number;
};

class Flight_Manager
{
public:
	struct timeval timesent;
	struct timeval timereceived;
	uint32_t duration = 120;
	uint32_t pre_notice_minutes;
	struct datastore next_tfd;
	uint32_t next_launch_epoch;

	uint32_t local_launch_sec;
	uint32_t local_landing_timeout_sec;
	uint32_t valid_local_launch_sec;
	uint32_t valid_local_landing_timeout_sec;
	
	int32_t secs_to_launch;
	int32_t secs_to_land;
	int32_t last_panel_secs;
	bool pre_notice_status;
	bool in_flight_status;
	int timer_state = TIMER_STATE_ST;

	int32_t table_index;
	bool min_oclock_beep_done = 0;
	bool min_oclock_play_done = 0;
	bool background_music_on = 0;
	int fraction_in_second;
	int last_fraction_in_second = 0;
	uint32_t millis_sent;
	uint32_t millis_landing;
	uint32_t millis_launch;
	uint32_t volume = 6;
	uint32_t brightness = 250;
	bool displayon = true;
	bool showcounter = true;
	char last_label[16] = "";
	uint32_t standby_recipe_counter = -3;
	uint32_t standby_recipe_step_period = 500; // milliseconds
	uint32_t last_step_millis = 0;
	int file_to_play = 0;

	int panel_type = NO_PANEL;
	bool panel_enabled = 1;
	// uint16_t baud_rate1=9600;
	uint16_t baud_rate1 = 19200;

	bool start_betty_deamon = 0;
	uint32_t betty_period=250;
	uint32_t last_betty_millis=0;
	uint32_t betty_step = 0;
	uint32_t wait_for_round = 2910;
	uint32_t wait_for_group = 3800;
	uint32_t adv_millis = 0;

	 // #define NO_PANEL  0
	// #define I2C_IF  1
	// #define STANDARD_SERIAL_IF  2
	// #define EXTENDED_SERIAL_IF  3

	struct alarm_table alarm[MAX_BEEP_POINTS] = {
		//  index;event; secondstoland;BIPTYPE;0;0;vol;filenam
		//  event; secondstoevent; BIPTYPE;millis_for_beep;timeval(local_secs_for_beep) ;vol;int filenumber
		{LANDING, 60, BOOP, 0, {0, 0}, 0, 0},
		{LANDING, 59, BIP_, 0, {0, 0}, 0, 0},
		{LANDING, 58, BIP_, 0, {0, 0}, 0, 0},
		{LANDING, 57, BIP_, 0, {0, 0}, 0, 0},
		{LANDING, 56, BIP_, 0, {0, 0}, 0, 0},
		{LANDING, 55, BIP_, 0, {0, 0}, 0, 0},
		{LANDING, 54, BPBP, 0, {0, 0}, 0, 0},
		{LANDING, 53, BPBP, 0, {0, 0}, 0, 0},
		{LANDING, 52, BPBP, 0, {0, 0}, 0, 0},
		{LANDING, 51, BPBP, 0, {0, 0}, 0, 0},
		{LANDING, 50, BPBP, 0, {0, 0}, 0, 0},
		{LANDING, 49, PLAY, 0, {0, 0}, 0, 1},
		{LANDING, 40, BPBP, 0, {0, 0}, 0, 0},
		{LANDING, 40, PLAY, 0, {0, 0}, 0, 20},
		{LANDING, 30, BPBP, 0, {0, 0}, 0, 0},
		{LANDING, 30, PLAY, 0, {0, 0}, 0, 30},
		{LANDING, 20, PLAY, 0, {0, 0}, 0, 40},
		{LANDING, 10, PLAY, 0, {0, 0}, 0, 50},
		{LANDING, 0, BOOP, 0, {0, 0}, 0, 0},
		{LANDING, -1, BIP_, 0, {0, 0}, 0, 0},
		{LANDING, -2, BIP_, 0, {0, 0}, 0, 0},
		{LANDING, -3, BIP_, 0, {0, 0}, 0, 0},
		{LANDING, -4, BIP_, 0, {0, 0}, 0, 0},
		{LANDING, -5, BIP_, 0, {0, 0}, 0, 0},
		{LANDING, -6, BPBP, 0, {0, 0}, 0, 0},
		{LANDING, -7, BPBP, 0, {0, 0}, 0, 0},
		{LANDING, -8, BPBP, 0, {0, 0}, 0, 0},
		{LANDING, -9, BPBP, 0, {0, 0}, 0, 0},
		{LANDING, -10, BPBP, 0, {0, 0}, 0, 0},
		{LANDING, -11, PLAY, 0, {0, 0}, 0, 1},
		{LANDING, -20, BPBP, 0, {0, 0}, 0, 0},
		{LANDING, -20, PLAY, 0, {0, 0}, 0, 20},
		{LANDING, -30, BPBP, 0, {0, 0}, 0, 0},
		{LANDING, -30, PLAY, 0, {0, 0}, 0, 30},
		{LANDING, -40, PLAY, 0, {0, 0}, 0, 40},
		{LANDING, -50, PLAY, 0, {0, 0}, 0, 50},
		{LAUNCH, 30, BIP_, 0, {0, 0}, 0, 0},
		{LAUNCH, 30, PLAY, 0, {0, 0}, 0, 30},
		{LAUNCH, 20, BIP_, 0, {0, 0}, 0, 0},
		{LAUNCH, 20, PLAY, 0, {0, 0}, 0, 20},
		{LAUNCH, 3, BIP_, 0, {0, 0}, 0, 0},
		{LAUNCH, 0, BOOP, 0, {0, 0}, 0, 0},
		{LAUNCH, -1, BIP_, 0, {0, 0}, 0, 0},
		{LAUNCH, -2, BIP_, 0, {0, 0}, 0, 0},
		{LAUNCH, -3, BIP_, 0, {0, 0}, 0, 0},
		{LAUNCH, -4, BIP_, 0, {0, 0}, 0, 0},
		{LAUNCH, -5, BIP_, 0, {0, 0}, 0, 0},
		{LAUNCH, -6, BPBP, 0, {0, 0}, 0, 0},
		{LAUNCH, -7, BPBP, 0, {0, 0}, 0, 0},
		{LAUNCH, -8, BPBP, 0, {0, 0}, 0, 0},
		{LAUNCH, -9, BPBP, 0, {0, 0}, 0, 0},
		{LAUNCH, -10, BPBP, 0, {0, 0}, 0, 0},
		{LAUNCH, -11, PLAY, 0, {0, 0}, 0, 11},
		{LAUNCH, -20, BPBP, 0, {0, 0}, 0, 0},
		{LAUNCH, -20, PLAY, 0, {0, 0}, 0, 20},
		{LAUNCH, -30, BPBP, 0, {0, 0}, 0, 0},
		{LAUNCH, -30, PLAY, 0, {0, 0}, 0, 30},
		{LAUNCH, -40, PLAY, 0, {0, 0}, 0, 40},
		{LAUNCH, -50, PLAY, 0, {0, 0}, 0, 50}
		//{LAUNCH, -56, _ADV, 0, {0,0}, 0, 50}
		//{LAUNCH, -57, STOP, 0, {0,0}, 0, 50}

	};

	void init();
	void start(uint32_t duration);
	void manage_flight();
	void manage_panel_in_flight();
	void manage_panel_in_standby();
	void record_launch_data();
	void bip();
	void beep();
	void boop();
	void bpbp();
	void update_volume(uint32_t setting);
	void update_brightness(uint32_t setting);
	void update_collector(bool setting);
	void update_groups(uint32_t setting);
	void update_scheduled(uint32_t setting);
	void read_settings();
	void print_settings();
	void save_settings();
	uint32_t look_for_new_epoch();
	uint32_t look_for_duration_from_file();
	bool check_for_scheduled_start();
	void schedule_competition(uint32_t seconds_for_next_advert);
	void perform_betty_deamon();
	void print_betty_func();
	uint32_t next_round();
	uint32_t next_group();
};

#endif

// #define	timercmp(a, b, CMP) 						      \
//   (((a)->tv_sec == (b)->tv_sec) ? 					      \
//    ((a)->tv_usec CMP (b)->tv_usec) : 					      \
//    ((a)->tv_sec CMP (b)->tv_sec))