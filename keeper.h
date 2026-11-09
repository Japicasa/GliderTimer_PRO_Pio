#ifndef keeper_h
#define keeper_h

#include <Arduino.h>
#include "esp32-hal-cpu.h"
#include "time.h" //**


// For set FAi Firmaware

#define FAI_FIRMWARE false   //Set to true for FAI Version
#define FW_VERSION "TimerFw4.15" //MUST be 11 chars

//// for flight mode
#define CONFIG 0
#define TIMED_OUT 1
#define INVALID 2
#define NEW_DATA 3
#define UNKNOWN 4
#define SUCCESS 5
#define PATATA 6
#define RECORD_EXISTS 7
#define PILOT_NOT_FOUND 8
#define REPEATED_RECORD 9
#define COMPETITION_CLOSED 10
#define SCAN_AP 11
#define PICK_DATA 12
#define NEW_KEEPER_TFD 13
#define GLIDERTHROW 14
#define CONNECTED_AS_STA 15
#define ACTION_ABOUT_MODE 16
#define UPDATE_IN_PROGRESS 17
#define RUN_FLIGHT_REQUEST 18
#define CONFIRM_WORKING_TIME 19
#define CONFIRM_TFD_UPLOAD 20
#define NEED_FORMAT 21
#define RECEIVED_FROM_TIMER 22
#define NOT_IN_GROUP 23


#define MENU_PAGE 0
#define ROUND_PAGE 1
#define PILOT_PAGE 2
#define FINAL_PAGE 3
#define ERROR_PAGE 4

#define FAILED_STA 0
#define KEEPER_STA 1
#define HOME_STA 2
#define THROW_STA 3
#define THROW4_STA 4
#define TIMER_STA 5 
#define  MAIN_STA 6






//// for record  mode trigger  REC_mode ver KeeperESp32.ino
#define REC_MODE_ALWAYS 0
#define REC_MODE_THROTTLE 1
#define REC_MODE_ALTITUDE 2

//// for competition COMP_mode
#define COMP_MODE_TRAINING 0    
// #define COMP_MODE_RESTART 1   // Emergency Restart Enable
#define COMP_MODE_ALTITUDE 2  
#define COMP_MODE_TIME 3 
#define COMP_MODE_FAI 4

/// for error reporting
#define err_code_none 0
#define err_code_disp 1
#define err_code_accel 2
#define err_code_spiffs 3
#define err_code_baro 4
//#define err_code_accel 5
#define err_code_pwm 6
#define err_code_nvs 7

// working_axis

#define WA_VERTICAL 2
#define WA_FLAT 3

#define COMP_NO 0
#define RECORD_NUM 1
#define ROUND_NO 2
#define GROUP_NO 3
#define PILOT_NO 4
#define PILOT_ID 5
#define FLTNUM 6
#define TAKE_OFF_MILLIS 7
#define TAKEOFF_LOCAL_SECS 8
#define TAKEOFF_LOCAL_MICROSECS 9
#define THROTTLE_SHUT_MILLIS 10
#define F5J_HEIGHT 11
#define F5J_MILLIS 12
#define LANDING_MILLIS 13
#define LANDING_PRESSURE 14
#define REST_MILLIS 15
#define FLIGHT_MINS 16
#define FLIGHT_SECS 17
#define TRANSMIT_LOCAL_SECS 18
#define TRANSMIT_LOCAL_USECS 19
#define TIMER_TO_KEEPER_SEC 20
#define TIMER_TO_KEEPER_USEC 21
#define EMERGENCY_USED 22
#define FROM_NVS 23	
#define LOCAL_LANDING_TIMEOUT_SEC 24
#define LOCAL_LANDING_TIMEOUT_USEC 25
#define TIMEDOUT 26
#define LANDING_DIST 27
#define PENALTY_PTS 28

#define SERIALNUMBER 0
#define FIRM_VER  1
#define TIMER_SERIALNUMBER 2
#define TIMER_FIRM_VER 3




//#include "src/Draw_list.h"  ///GLIDERTIMER
 typedef struct datastore
{
	uint32_t comp_no;
	uint32_t record_num;	
	uint32_t round_no;
	uint32_t group_no;
	uint32_t pilot_no;
	uint32_t pilot_id;
	uint32_t fltnum;
	uint32_t take_off_millis;
	uint32_t takeoff_local_secs;
	uint32_t takeoff_local_microsecs;
	uint32_t throttle_shut_millis;
	uint32_t F5J_height;
	uint32_t F5J_millis;	
	uint32_t landing_millis;
	// uint32_t landing_pressure;	
	uint32_t rest_millis;
	uint32_t flight_mins;
	uint32_t flight_secs;
	int32_t transmit_local_secs;  //this is now timer to keeper shift
	uint32_t transmit_local_usecs;
	int32_t timer_to_keeper_sec;
	int32_t timer_to_keeper_usec;
	int32_t emergency_used;
	//uint32_t RTCtimed;
	uint32_t from_NVS;
	//uint32_t boolpad;
	//uint32_t landing_spot_no;
	uint32_t local_landing_timeout_sec;
	uint32_t local_landing_timeout_usec;
	uint32_t timedout;
	uint32_t landing_dist;
	uint32_t penalty_pts;
	uint32_t raw_score;
	char serialnumber[12];
	char Firm_ver[12];
	char timer_serialnumber[12];
	char timer_Firm_ver[12];
};






struct gliderkeeperconfig
{
  char WiFiSSID[50];
  char WifiPass[50];
  bool Training;
  bool FAImode;
  bool EmergencyMotorEnable;
  bool ShowFlightOLED;
  bool AltitudeSwitch;
  uint16_t AltitudeTrigger;
  bool TimeSwitch;
  uint16_t TimeTrigger;
  bool GliderLink;
  bool AlwaysOn;
  bool MotorLogging;
  bool AltitudeLogging;
  uint16_t AtitudeLoggingSetting; ///parece se rque esto es el umbral de registro de altitud
  uint8_t GKOrientation;
  char QRreader[150];
  char WiFiSSID1[50];
  char WifiPass1[50];
  uint16_t TimeForConnect;
  uint16_t MillisForReconect;
  uint16_t jerk_thr;
  uint16_t gzsm_thr;
  uint16_t landing_pressure_thr;
};

// extern struct gliderkeeperconfig gkConfig;

boolean buscaoled();
void medirbien();
void medir();
void print_measured_data(struct datastore *);
void displayconnectedscreen();
void report_error(int);

// extern boolean faiFirmware;
// extern short jerk;
// extern int gzsmoother;
// extern unsigned long take_off_millis;
// extern unsigned long tik;
// extern int working_axis;
// extern int bsm;
// extern byte FLT_mode;
// extern int blink_mode;
// //extern int blink_period;
// extern int blink_counts;
// extern boolean throttle_shut;
// extern unsigned long throttle_shut_millis;
// extern uint8_t COMP_mode;
// extern bool EmergencyMotorEnable;

#endif
