// GTimer.h 2021/01/19 compilation of classes of gliderTimer

#ifndef GTimer_h
#define GTimer_h

#include <stdint.h>
#include <Arduino.h>

#include "keeper.h"
//#include "Pilots.h"
#include "contests.h"
#include "GTimer.h"
#include "GTdisplay.h"
#include "FlightManager.h"
#include "Timer_flight_data.h"
#include "CompetitionsavedRW.h"
#include <SPIFFS.h>
#include <M5Stack.h>


typedef struct GROUP_Pilots
{
    // uint32_t pilot_no;
    uint32_t pilot_id;
    uint32_t serialnumber;
    uint32_t serialnumber2;
    uint32_t serialnumber3;
    // char serialnumber[4];
    // char serialnumber2[4];
    // char serialnumber3[4];
    // char serialnumber[12];
    // char serialnumber2[12];
    // char serialnumber3[12];
    // char name[25];
    // char nick_name[5];
};

typedef struct Pilots
{

    // uint32_t pilot_no;
    uint32_t pilot_id;
    char serialnumber[12];
    char serialnumber2[12];
    char serialnumber3[12];
    char name[25];
    char nick_name[5];
};
typedef struct Menulist
{
    uint32_t menulevel;
    char menulabel[18];
    // uint32_t menulabel;
    uint32_t new_menulevel;
    uint32_t action;
};
#define MAX_MENUPOINTER 110
#define ACTION_MENU 1
#define ACTION_LIST_ROUND 2
#define ACTION_SELECT_COMPETITION 3
#define ACTION_LIST_PILOT 4
#define ACTION_RUN_FLIGHT 5
#define ACTION_NEW_COMP 6
#define ACTION_ERASE_COMP 7
#define ACTION_GLIDER_THROW 8
#define ACTION_CLOSE_COMPETITION 9
#define ACTION_SCAN_AP 10
#define ACTION_SELECT_AP 11
#define ACTION_BRIGHTNESS 12
#define ACTION_VOLUME 13
#define ACTION_ABOUT 14
#define ACTION_COLLECTOR_TOGGLE 15
#define ACTION_COLLECTOR_OFF 16
#define ACTION_GROUP_ON 17
#define ACTION_GROUP_TOGGLE 18
#define ACTION_SCHEDULED_TOGGLE 19

#define SELECTION_POINTER_ROUND 1
#define SELECTION_POINTER_COMPETITION 2
#define SELECTION_POINTER_PILOT 3

#define ERROR_AP 14
#define ERROR_DATA 16
#define ERROR_FLASH 18
#define ERROR_SYNC 24

#define MAX_SELECTION_POINTER_LANDING 16
#define MAX_SELECTION_TABLE_PILOTS 64
#define MAX_GROUP_DRAW_DATA 15

#define MAX_PILOT_NUMBER 4
#define TIMEOUT_SECS_CLASSIFICATION 600
#define TIMEOUT_SECS_FLYOFF 900

const uint32_t MAX_PILOTS_IN_COMPETITION = MAX_SELECTION_POINTER_PILOT; // they will be #0 till MAX_PILOTS_IN_COMPETITION-1
// const uint32_t PILOT_NOT_FOUND_FLAG = MAX_PILOTS_IN_COMPETITION + 1;
const uint32_t PILOT_NOT_FOUND_FLAG = 999;

class GTimer
{
public:
    struct Menulist GTmenu[MAX_MENUPOINTER] = {
        // menulevel; menulabel;new_menulevel; Action ;menupointer
        {0, "GliderTimer", 1, 0},
        {1, "Run Flight", 116, ACTION_MENU},
        {1, "Pick data", 118, ACTION_SCAN_AP},
        {1, "Show Scores", 11, ACTION_MENU},    // 1
        {1, "Management", 12, ACTION_MENU},     // 2
        {11, "Select Round", 111, ACTION_MENU}, // 3
        {11, "Select Pilot", 113, ACTION_MENU}, // 5
        {11, "Sel. Compet.", 112, ACTION_MENU}, // 4
        //{12, "Run Flight",      121, ACTION_RUN_FLIGHT},
        //{12, "Pick GK data",    118, ACTION_SCAN_AP}, //6
        {12, "New Compet.", 114, ACTION_MENU},
        {12, "Settings", 20, ACTION_MENU},    // 7
        {12, "Group Mngmt", 25, ACTION_MENU}, // 8
        {12, "Format Data", 30, ACTION_MENU}, // 8
        {13, "Ap Error", 14, ACTION_MENU},
        {14, "  ", 1, ACTION_MENU},
        {14, "Conectn. Fail", 1, ACTION_MENU},
        {14, "  ", 1, ACTION_MENU},
        {14, "Press Select", 1, ACTION_MENU},
        {15, "Data error", 16, ACTION_MENU},
        {16, "  ", 1, ACTION_MENU},
        {16, "not a flight", 1, ACTION_MENU},
        {16, "  ", 1, ACTION_MENU},
        {16, "Press Select", 1, ACTION_MENU},
        {17, "Database", 16, ACTION_MENU},
        {18, "Failed  ", 1, ACTION_MENU},
        {18, "pls, Format", 1, ACTION_MENU},
        {18, "  ", 1, ACTION_MENU},
        {18, "Press Select", 1, ACTION_MENU},
        {20, "Volume", 21, ACTION_MENU},
        {20, "Brightness", 22, ACTION_MENU},
        //{20, "Group Mngmt", 25, ACTION_MENU},
        {20, "About GTmini", 1, ACTION_ABOUT},
        {21, "Volume: 0", 0, ACTION_VOLUME},
        {21, "Volume: 2", 2, ACTION_VOLUME},
        {21, "Volume: 4", 4, ACTION_VOLUME},
        {21, "Volume: 6", 6, ACTION_VOLUME},
        {21, "Volume: 8", 8, ACTION_VOLUME},
        {22, "Brigthness: 1", 50, ACTION_BRIGHTNESS},
        {22, "Brigthness: 2", 100, ACTION_BRIGHTNESS},
        {22, "Brigthness: 3", 150, ACTION_BRIGHTNESS},
        {22, "Brigthness: 4", 200, ACTION_BRIGHTNESS},
        {22, "Brigthness: 5", 250, ACTION_BRIGHTNESS},
        {23, "Sync Failed", 16, ACTION_MENU},
        {24, " ", 1, ACTION_MENU},
        {24, "pls, retry", 1, ACTION_MENU},
        {24, "  ", 1, ACTION_MENU},
        {24, "Press Select", 1, ACTION_ERASE_COMP},
        {25, "ONE group", 1, ACTION_GROUP_TOGGLE},
        {25, "Collector ON", 1, ACTION_COLLECTOR_TOGGLE},
        {25, "Scheduled ON", 1, ACTION_SCHEDULED_TOGGLE},
        {30, "Data about to", 1, ACTION_ERASE_COMP},
        {30, "Erase. Press", 1, ACTION_ERASE_COMP},
        {30, "Select: Confirm", 1, ACTION_ERASE_COMP},
        {111, "Final Score", 0, ACTION_LIST_ROUND},
        {111, "Round #1", 1, ACTION_LIST_ROUND}, // 111
        {111, "Round #2", 2, ACTION_LIST_ROUND}, // 12
        {111, "Round #3", 3, ACTION_LIST_ROUND}, // 13
        {111, "Round #4", 4, ACTION_LIST_ROUND}, // 14
        {111, "Round #5", 5, ACTION_LIST_ROUND}, // 14
        {111, "Round #6", 6, ACTION_LIST_ROUND},
        {112, "Comp. #1", 1, ACTION_SELECT_COMPETITION}, // 111
        {112, "Comp. #2", 2, ACTION_SELECT_COMPETITION}, // 12
        {112, "Comp. #3", 3, ACTION_SELECT_COMPETITION}, // 13
        {112, "Comp. #4", 4, ACTION_SELECT_COMPETITION}, // 14
        {112, "Comp. #5", 5, ACTION_SELECT_COMPETITION}, // 14
        {112, "Comp. #6", 6, ACTION_SELECT_COMPETITION},
        {112, "Comp. #7", 7, ACTION_SELECT_COMPETITION},
        {112, "Comp. #8", 8, ACTION_SELECT_COMPETITION},
        {113, "Pilot #1", 1, ACTION_LIST_PILOT}, // 111
        {113, "Pilot #2", 2, ACTION_LIST_PILOT}, // 12
        {113, "Pilot #3", 3, ACTION_LIST_PILOT}, // 13
        {113, "Pilot #4", 4, ACTION_LIST_PILOT}, // 14
        {113, "Pilot #5", 5, ACTION_LIST_PILOT}, // 14
        {113, "Pilot #6", 6, ACTION_LIST_PILOT},     
        {114, "9 pilots", 1179, ACTION_MENU},
        {114, "8 pilots", 1178, ACTION_MENU}, // 14
        {114, "7 pilots", 1177, ACTION_MENU},  // 12
        {114, "6 pilots", 1176, ACTION_MENU},    // 12
        {114, "5 pilots", 1175, ACTION_MENU},    // 12
        {114, "4 pilots", 1174, ACTION_MENU},    // 12
        {114, "3 pilots", 1173, ACTION_MENU},    // 13
        {114, "2 pilots", 1172, ACTION_MENU},
        {114, "1 pilot_", 1171, ACTION_MENU},
        {115, "6 rounds", 6, ACTION_NEW_COMP},
        {115, "5 rounds", 5, ACTION_NEW_COMP},
        {115, "4 rounds", 4, ACTION_NEW_COMP},
        {115, "3 rounds", 3, ACTION_NEW_COMP},
        {115, "2 rounds", 2, ACTION_NEW_COMP},
        {115, "1 round_", 1, ACTION_NEW_COMP},
        {116, "in 1 min.", 1, ACTION_RUN_FLIGHT},
        {116, "in 2 min.", 2, ACTION_RUN_FLIGHT},
        {116, "in 3 min.", 3, ACTION_RUN_FLIGHT},
        {116, "in 4 min.", 4, ACTION_RUN_FLIGHT},
        {116, "in 5 min.", 5, ACTION_RUN_FLIGHT},
        {117, "10 min durat.", 11510, ACTION_MENU}, // 12
        {117, "15 min durat.", 11515, ACTION_MENU}, // 13
        {117, " 2 min durat.", 11502, ACTION_MENU},
        {117, " 5 min durat.", 11505, ACTION_MENU},
        {118, "scanning", 0, ACTION_SELECT_AP},
        {118, "...", 1, ACTION_SELECT_AP},
        {118, "...", 2, ACTION_SELECT_AP},
        {118, "Choose one id.", 3, ACTION_SELECT_AP},
        {333, "NOP", 33, 0} // 15
    };

    int management_pointer = 0;

    uint32_t flt_duration = 10; // these 3 to store values while setting the competition
    bool groups_enabled = 0;
    uint32_t collector = 0;
    bool scheduled = 0;
    uint32_t comp_step = 1;
    uint32_t strict_timing =0;    // TODO 4.15
    uint32_t groups; /// from settings
    uint32_t stored_competition_no;
    uint32_t stored_round_no;
    uint32_t stored_group_no;
    uint32_t max_round;
    uint32_t max_pilot;
    uint32_t max_group;

    uint32_t stored_record_num;
    bool stored_competition_closed;

    uint max_group_in_file = 0; // the result of scanning the draw file
    uint max_round_in_file = 0;
    uint max_pilot_number_in_file = 0;

    uint32_t current_record_num;
    uint32_t current_competition_no = -1;
    uint32_t current_round_no;
    uint32_t current_group_no = 0;
    uint32_t round_number = 1;
    uint32_t pilot_number = 1;

    uint32_t menulevel = 1;
    uint32_t menupointer = 1;
    uint32_t pick_pointer;
    uint32_t selectionAP;
    String ssid_found[3];
    bool firstime = 1;

    uint32_t selected_pilots = 0; // number of pilot selected in menu for creation
    // uint32_t flt_duration = 10;
    // The following struct pivots Pilot# at competition to Its identification
    //  pilot_id_contest[piloti] is the number in Pilots_table of piloti  (the registered pilot in this competition) =0 if not found
    //  serialnum_contest[piloti] is the Serial number of the unit of piloti which must be the same in pilots_table number in Pilots_table[pilot_id_contest[piloti]]

    // uint8_t pilot_id_contest[MAX_SELECTION_POINTER_PILOT];
    // String serialnum_contest[MAX_SELECTION_POINTER_PILOT];
    

    uint32_t registered_pilots = 0; // number of pilots that upto the moment uploaded in a running
                                    //  mustbe equal or smaller than head.number_of_pilots
                                    //  serialnum_contest[MAX_SELECTION_POINTER_PILOT] must be filled from 1 and sequentially
    bool new_pilot_added = 0;
    uint16_t group_draw_data_pointer=0;

    bool receiving_working_time = 0;
    bool received_from_GTimer = 0;
    int tfd_received = 0;
    int tfd_sent_no = 0;
    int tfd_fail_no = 0;

    struct GROUP_Pilots group_pilots_table[GROUP_MAX_SELECTION_POINTER_PILOT];
    struct Pilots pilots_table[16];
    struct Contests mycompetition;
    struct Epoch_table epoch_table[GROUP_MAX_ROUND][GROUP_MAX_GROUP];

    struct Draw_list group_draw_list[GROUP_MAX_SELECTION_POINTER_ROUND][GROUP_MAX_SELECTION_POINTER_PILOT];

    struct datastore group_draw_data[MAX_GROUP_DRAW_DATA];
    struct datastore draw_data[MAX_SELECTION_POINTER_ROUND][MAX_SELECTION_POINTER_PILOT];

    //uint32_t rank_list[MAX_SELECTION_POINTER_ROUND][2][MAX_SELECTION_POINTER_PILOT];
    uint32_t rank_list[MAX_SELECTION_POINTER_ROUND][MAX_SELECTION_POINTER_GROUP][MAX_SELECTION_POINTER_PILOT];
    // Group is left to "group=1" -> only !groups_enabled, for future improvements
    // only internal GliderTimer competitions can be handled
    ////////////////////
    void begin();
    void load_stored_competition(uint32_t competition_no);
    void load_draw_data(uint32_t competition_no);
    void load_pilots_table();
    void load_pilot_master_file();
    void load_draw_from_file();
    uint32_t find_sn_in_pilots_table(char serialnumber[12]);
    uint32_t find_sn_in_draw_list(char serialnumber[12]);
    void close_competition();
    void clear_mycompetition();
    void load_comp(uint32_t competition_no);

    void manage_show_round_data();
    void manage_show_pilot_data();
    void manage_show_error_data();
    void manage_error(int error_code);
    void manage_management();
    void manage_new_data();
    void perform_menu_action(uint32_t action_code);
    void button_update();
    void clear_buttons();
    void new_competition();
    void clear_draw_list();
    void clear_draw_list_scores();
    void clear_contest_pilot_list();

    void create_dummy_draw_list();
    void create_new_competition_from_menu();
    void create_competition_from_file();
    void manage_wifi_input();
    void save_record_and_update_competition(int landing_case);
    void save_record();
    void save_working_time();
    void calculate_raw_score(int landing_case);
    uint32_t assign_pilot_and_round(struct datastore tfd);
    uint32_t assign_pilot_and_round(struct datastore tfd, uint32_t group_no);
    uint32_t find_pilot_in_table(char serial_num[12]);
    int32_t find_pilot_ID_in_table(uint32_t checkd_id);
    uint32_t add_pilot_in_table(uint16_t pilot_id); // returns the pilot_number that corresponds to this id
    uint32_t new_pilot_in_pilots_table();

    void check_for_end_round();
    void back_to_prev_status();
    void print_draw_data();
    void print_draw_list();
    void clear_draw_data();
    void report_error(int error_code); /// to be deprecated
    bool all_scores_in_round();
    uint32_t esplit(String line, String delim); // will retrun the number of field read
    String textxtract(String line, String delim, int pos);
    String desplit(uint32_t fields[20]);
    bool isValidNumber(String str);
    uint32_t fields[20];
    uint32_t delims[20];
    int confirm_for_upload_or_new_launch();
    bool send_loop_tfd();
    bool send_single_tfd();
    void update_setting_labels();
    void set_competition_max_values();
    uint8_t GetHexVal(char hex);
    uint32_t char2_SN(char SerialNumber[12]);
    uint32_t find_pilot_ID_in_nongroup_competion(uint32_t new_candidate_id);
    int fill_pilot_data(String line, int curr_pilot);
};
extern bool button_pressed;
extern bool buttonA;
extern bool buttonB;
extern bool buttonC;
extern char *button_label;
// extern M5stack M5;

#endif