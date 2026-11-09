#ifndef GTdisplay_h
#define GTdisplay_h

#include <stdint.h>
#include <Arduino.h>

//#include "Pilots.h"
#include "contests.h"
#include "GTimer.h"
#include "Timer_flight_data.h"
#include "FlightManager.h"
#include "Gthrow.h"

#include <WiFi.h>
#include <WiFiClient.h>
//#include <WebServer.h>
#include <ESPmDNS.h>
#include <DNSServer.h> //captive
//update
#include <Update.h>
///OTA

#include <WiFiUdp.h>
//#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include <SPIFFS.h>
#include <M5Stack.h>
#include "Free_Fonts.h"
#include "RTC.h"
#include "MP3player.h"
#define NO_OFF_FORMATS 8
#define NO_OFF_LINES 10


#define FLASH_SIZE 1376256
#define SPIFFS_SIZE 1114112
struct format_table
{
    uint32_t format_no;
    uint32_t text_size0;
    uint32_t text_size1;
    uint32_t xoffset0;
    uint32_t yoffset00;
    uint32_t yoffset0;
    uint32_t xoffset1;
    uint32_t yoffset1;
    uint32_t max_lines;
};

struct offsettable
{
    uint32_t format_no;
    uint32_t line;
    uint32_t text_size = 2;
    uint32_t xoffset = 1;
    uint32_t yoffset = 10;
};

class GTdisplay
{
public:
    uint32_t background_colorx = TFT_DARKGREEN;
    //992;
    uint32_t white_colorx = TFT_WHITE;

    uint32_t black_colorx = BLACK;
    uint32_t red = TFT_WHITE;
    uint32_t green = TFT_WHITE;
    uint32_t green1 = TFT_WHITE;
    uint32_t blue = TFT_WHITE;
    uint32_t dark_blue=TFT_WHITE;
    uint32_t grey = TFT_WHITE;
    uint32_t yellow= TFT_WHITE;

    uint32_t xpos = 0;
    uint32_t ypos = 0;
    uint32_t line = 0;
    //uint32_t format = 0;
    //uint32_t last_time_millis;
    uint32_t last_secs_to_land;
    char tc[25];
    char c_str_minutes[3];
    char c_str_seconds[3];
    char timetodisp[8];
    // String str_minutes;
    // String str_seconds;
    //String timetodisp;
    bool primeravez = 1;
    bool last_prenotice = 1;
    bool firsttime_settings =1;
    bool firsttime_run=1;
    uint32_t sched_adv_millis_for_partial=0;
    bool sched_adv_first_time=1;

    uint32_t xapos = 0;
    uint32_t yapos = 210;
    String alabel = "";
    uint32_t xbpos = 35;
    uint32_t ybpos = 238;
    String blabel = "";
    uint32_t zoom_level = 0;
    bool updating_flash=0;

    //String label0 = "";
    String label1;
    String label2;
    String label3;
    String label4;
    String label5;
    String label6;

    // String label1_v;
    // String label2_v;
    //String label3_v;

    char vlabel1[7];
    char vlabel2[7];
    char vlabel3[7];
   // String prueba = "35";
    // String label4_v;
    // String label5_v;
    // String label6_v;

    format_table page_format[NO_OFF_FORMATS] =
        //  format_no;
        //     text_size0 ;
        //        text_size1 ;
        //            xoffset0 ;
        //                yoffset00 ;
        //                   yoffset0 ;
        //                       xoffset1 ;
        //                           yoffset1 ;
        //                               max_lines ;
        {
            {0, 4, 3, 30, 40, 5, 15, 39, 5},
            {1, 4, 3, 30, 40, 5, 30, 30, 5},
            {2, 4, 3, 5, 40, 8, 40, 30, 5},
            {3, 4, 3, 5, 40, 8, 40, 30, 5},
            {4, 3, 2, 5, 30, 0, 5,  26, 9},
            {5, 4, 3, 5, 40, 3, 5,  30, 9},
            {6, 2, 5, 5, 30, 0, 5,  26, 9},
            {7, 6, 2, 5, 30, 0, 5,  26, 9}};

    offsettable offsets[NO_OFF_FORMATS][NO_OFF_LINES];

    void begin();
    void clear_display();
    void cursor(uint32_t line_no, uint32_t format_no);
    void GDsetTextSize(int size);
    void string_to_disp(String TC, uint32_t line, uint32_t format);
    void string_to_disp(char TC[25], uint32_t line, uint32_t format);
    void displaymenu();
    void display_fltdata(struct datastore FLTdata);
    void display_toggle_name_in_keeper();
    void refresh_disp(String msg);
    void displaywelcomescreen();
    void displayStatus();
    void displayUpdate();
    void STA_disp();
    bool file_disp(int file_num, String payload);
    void update_screen();
    void menu_displayUpdate();
    void menu_displayUpdate(uint32_t menupointer);
    void menu_display_final_round(uint32_t round_number);
    void menu_display_list_round(uint32_t round_number);
    void menu_display_list_pilot(uint32_t pilot_number);
    void displayUpdateError(String msg);
    void displayIPscreen(bool STA_connected);
    void displayameasurementscreen();
    String status_label();
    void partial_flt();
    void clean_partial_flt();
    void print_batt();
    void display_status();

    void manage_timer_display();
    void display_timer();
    void erasetimer();
    void about_screen();
    void confirm_run_flight();


    void displayThrow();
    void clear_thdisplay();
    void draw_angleview_template();
    void erase_angleview();
    void draw_angleview();
    void draw_throwview_template();
    void draw_chord1_template();
    void erase_chord1();
    void draw_chord1();
    void draw_chord2_template();
    void erase_chord2();
    void draw_chord2();
    void draw_angleview3_template();
    void draw_throwview3_template();
    void draw_chord3_template();
    void draw_chord4_template();
    void draw_angleview4_template();
    void draw_angleview4();
    void draw_throwview4_template();
    //void partial_flt(uint32_t timedout);

private:
    //     void display_timer();
    //    void erasetimer();
};
//extern M5stack M5;
//extern GTimer GT;
extern int8_t bat_level;
extern MP3player MP3;
extern int update_counter;
extern String fw_ui_version;

#endif