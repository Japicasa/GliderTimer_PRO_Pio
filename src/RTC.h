#ifndef RTC_h
#define RTC_h

#include "DebugPrint.h"
#include <sys/time.h>
#include <ESPNtpClient.h>
#include <SPIFFS.h> // Include the SPIFFS library

#define MILLIS_FOR_NTP_SYNC 10000

#define NO_RTC_AVAILABLE 0
#define RTC_AVAILABLE 1
#define RTC_OUT_OF_SYNC_PERIOD 2
#define FAILED_NTP 3
#define LOCAL_TIME_ON_SYNC 4
#define RTC_SET_FAILED 5
#define RTC_ON_SYNC 6
#define RTC_ON_SYNC_AND_CALIBRATED 7
#define RTC_SYNC_DISABLED 8

#define DS3231_TEMPERATURE_ADDR 0x11
#define SECONDS_2021 1612137600 //Date and time (GMT):> Monday, 1 February 2021 0:00:00
#define SECONDS_2038 2147483648  //Date and time (GMT):> , 19 enero 2038 0:00:00
#define RECORD_MILLIS_PERIOD 30000
#define NUMBER_OF_RECORDS 11  
#define ELAPSED_FOR_AGING 43200 ///seconds to elapse from last successful sync for re-aging: 12 hours
#define ELAPSED_FOR_SYNC 21600 //6 hours
#define MAX_AGING 66
#define DOUBLE_SYNC_ERROR 10 //double of miliseconds repetability from NTP

class RTC
{

private:
    // RTCtime es tm del RTC, el año de RTC es de 0 a 99 hay que sumar 100 despues del 2000
    // para convertirlo en una tm
    struct tm RTCtime;
    struct tm last_sync_time;

    struct timeval RTCtimeval;

public:
    //struct RTCdata myRTCdata;
    //void ReadRTC(struct RTCdata *myRTCdata);

    int keeper_Synchro_status = NO_RTC_AVAILABLE;
    struct tm *ReadRTC();
    void read_last_sync();
    struct tm *Read_Until_Sec();
    void ConfigLocalTimeFromNTP(bool start_sig);
    void ConfigLocalTimeFromRTC();
    String SetRTC();
    void ZeroRTC();
    void PrintRTC();
    void PrintRTCval();
    void printLocalTime();
    void PrintLocalTimeval();
    void printLocalTimeMicros();
    void test_rtc();
    long diff_timevals(struct timeval old, struct timeval newt);
    uint8_t read_control_reg();
    void write_status_reg();
    void sleep();
    void wake_up();
    void setAgingOffset(int8_t offset);
    float DS3231_get_treg();
    void record_RTC_drift(bool RTC_drift_record_flag);
    void load_first_set();
    float eval_drift_from_last_sync();
    bool write_aging(int8_t aging);
    int8_t read_aging();
    //void blink_out();

    struct timeval last_sync_val;
    int8_t old_drift=0;
    uint8_t alarm2_date=0;
    bool RTC_corrupted=0;
    bool power_lost=0;
    bool hibernated=0;

    boolean syncEventTriggered = false; // True if a time even has been triggered
    NTPEvent_t ntpEvent;              // Last triggered event
    String my_event_string;
    double offset;
    double timedelay;
    void processSyncEvent (NTPEvent_t ntpEvent);
    char* ntpEvent2str (NTPEvent_t e);
    int event_class =-1;

    float Avg_sec_init;
    float Avg_drift_init;
    float Avg_sec_final;
    float Avg_drift_final;
    bool is_final=0;
};
extern const char *ntpServer;
extern const long gmtOffset_sec;
extern const int daylightOffset_sec;
extern boolean RTC_available;
extern boolean RTC_updated;
extern boolean RTC_drfit_file_flag;
// extern String disp_top_line;

#endif