
/*
GLIDERTTIMER
*/
#include <Wire.h>
#include <M5Stack.h> ///TODO JHR 19/1/2021 : src/In_eSPI.h 543
#include <sys/time.h>

#include <Wire.h>
#include "DebugPrint.h"
#include <AsyncUDP.h>

//////////////Seccion wifi/////////////////////
#include <SPIFFS.h> // Include the SPIFFS library

#include "keeper.h"
#include "src/GTdisplay.h"
#include "src/FlightManager.h"
#include "KeeperESP32WIFI.h"
//#include "StructSavedRW.h"
#include "src/CompetitionSavedRW.h"
#include "src/Myudp.h"
#include "src/Gthrow.h"
#include "src/MP3player.h"
#include "src/MCP23017Display.h"
#include "RTC.h" //**
#include <ESPNtpClient.h>

#include "NVSFunctions.h"

////////////////////////////////
// Version
///////////////////////////////
String versionkeeper = String(FW_VERSION);
String fw_ui_version= versionkeeper.substring(7);

/////////////////////////////////////Variables "config"" ////////////////
// FLTnum debe estar en la lista
int screen_type = 1;
int interrupt_pin = 0; //1 int11; 0->int2; 2 polling
int bsm = 6;           //orden del esmucificado
short shake_thr = 200;
short jerk_thr = 300;
short gzsm_thr = 15; // pongo 20 despues de cambiar gzsmoother +jerk<<3 25 Ago  // pongo 3 24Ago2019, era 10// pongo 25 26Oct
short landing_pressure_thr = 100;
int timelapse = 2000; //miliseconds
//int pinconfig = 15;   // este es el pin D0 del wemos TODO, para que se usa?
int working_axis = 3;
boolean testMode = 0;
//int blink_period = 350;
int firm_ver = 100101;
                                                    
String serialnumber = WiFi.macAddress().substring(6); // Se actualiza al inicializar la wifi 00:00:nn:nn:nn:nn

///////////  ntp  data & RTC data
//////////////////////////////////
char * button_label="  Menu    Scroll   Select";

// const char *ntpServer = "es.pool.ntp.org";
const char *ntpServer = "time.cloudflare.com";
//const char *ntpServer = "pool.ntp.org";
// 	•	Worldwide - pool.ntp.org
// •	Asia - asia.pool.ntp.org
// •	Europe - europe.pool.ntp.org
// •	North America - north-america.pool.ntp.org
// •	Oceania - oceania.pool.ntp.org
// •	South America - south-america.pool.ntp.org

const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;
boolean RTC_available = 0;
boolean RTC_updated = 0;

////////////////////////////////////Variables del Vuelo "last_FLT"
//////firm_ver debe estar en la lista
int FLTnum = 1;
float F5J_height = 0;
long F5J_millis = 0;
int landing_pressure = 0;
unsigned long take_off_millis = 0;
unsigned long takeoff_localsecs = 0;
uint16_t takeoff_localmilisecs = 0;
unsigned long shake_timer = 0;
unsigned long throttle_shut_millis = 0;
unsigned long rest_millis = 0;
unsigned long landing_millis = 0;
float flight_time = 0;
int flight_mins = 0;
int flight_secs = 0;
int eventcount = 0;
bool screwF5JFlight = 0;
//////////////////////////////////////////////
/////////////////////////////////////Variables internas ////////////////

//Variables de control/////
String FLT_filename = "fwSTD__1.26";


uint8_t REC_mode = REC_MODE_THROTTLE; // variable globales para registro y competicion
uint8_t COMP_mode = COMP_MODE_FAI;
boolean EmergencyMotorEnable = true;
boolean faiFirmware = FAI_FIRMWARE;

boolean modoavanzado = 1;

// int N_loops = 0;                    //contador para no sobrecargar al display
// boolean inic_signals_validated = 0; // si el altis esta bien comunicado
boolean throttle_shut = 0; // si se han usado los gases
boolean landing_event = 0;

//int altitude_inic = 0;

unsigned long tik = 0;
unsigned long tak = 0;
boolean throttle_flag = 0; // Si el Valor de Throttle en mayor de un 20% del inicial
int gz_flag = 0;
int num = 0; //puntero de numero de vuelo en filesytem
int blink_time = 0;
int blink_counts = 0;
int blink_mode = 3;

int error = 0;

// int record_num = 0;
int last_saved_rec = 0;
int read_pointer = 0;
int save_pointer = 0;
int gzsmoother = 0;
short jerk = 0;
boolean recording = 0;
boolean blink_ontime_flag=0; //If true, pin 5 will blink on odd seconds
int gpio_pin = 5; //pinout


int gpio_in_pin =2;// mp3 monitor
boolean busy_mp3=0;
boolean check_for_mp3=0;
uint32_t millis_from_cmd=0;
byte display_mode = COMMON_CATODE;

int update_counter = 0; ///para que funcione la libreria updater con la mod de GK
boolean RTC_drift_flag=0;  /// drift logFile.csv to be created
boolean RTC_drift_record_flag=0;  ///start recording once NTP enabled
boolean RTC_drfit_file_flag=0; // flag to bypass ConfigLocalTimeFromNTP in subsequent
//StructSaved vuelosFlash;
//timer_flight_data current_fltdata;
uint32_t GLOBAL_DURATION = 0; //esto se actualiza al iniciar el vuelo, y se usa para mostrar el tiempo total del vuelo en la pantalla de resultados, aunque el vuelo se haya abortado antes de ese tiempo
GTimer GT;                    //maneja el menu y los resultados de la competiion
GTdisplay GD;                 //accesos a display
CompetitionSaved competition; //acessos a flash
Timer_flight_data FLTdata;    //recibido por wifi
Flight_Manager flight;
Myudp myudp;
//Gthrow gth; //TODO quitando Gthrow
AsyncUDP udp;
RTC local_RTC;
MP3player MP3;
bool button_pressed = 0;
bool buttonA = 0;
bool buttonB = 0;
bool buttonC = 0;
int8_t bat_level =0;

#define buff_len 160
#define Debug Serial

//struct datastore myevent;
//String incoming_package;
//char incoming_Pack [100]={0};
volatile bool packet_received;
// Variables goblales que representras las tareas que se ejecutan en bucle las CPU0 y CPU1

TaskHandle_t taskLoopCPU0, taskLoopCPU1;

struct datastore datossensores; //esto viene del antiguo loop1
struct datastore myDecimated;
void scan_wifi();

String get_last_FLTdata();
bool send_single_tfd();
bool send_loop_tfd();
int connect_to_AP(int selection);
int download_scheduled_rounds(int file_num, bool main_sta);

// void loopCPU1(void *parameter)
// {
bool udp_in_progress = 0;
uint32_t timeforpwm = 0;
uint32_t timefordisp = 0;
int FLT_mode = PATATA; ////borrado aqui al migrar al  LOOP
boolean sinWifi = 1;
int page_to_show = MENU_PAGE;

// while (true)
// {
int nloops;
NVSClass nvs;
// #include "AudioFileSourceSPIFFS.h"
// #include "AudioFileSourceID3.h"
// #include "AudioGeneratorMP3.h"
// #include "AudioOutputI2S.h"

// AudioGeneratorMP3 *mp3;
// AudioFileSourceSPIFFS *file;
// AudioOutputI2S *out;
// AudioFileSourceID3 *id3;