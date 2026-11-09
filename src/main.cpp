#include <Arduino.h>


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
#include "GTdisplay.h"
#include "FlightManager.h"
#include "KeeperESP32WIFI.h"
//#include "StructSavedRW.h"
#include "CompetitionSavedRW.h"
#include "Myudp.h"
#include "Gthrow.h"
#include "MP3player.h"
#include "MCP23017Display.h"
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


//#include "QueueRegister.h"
//#include "StructSavedRW.h"
#include "CompetitionSavedRW.h"
#include "DebugPrint.h"

void setup()
{
  M5.begin(true, false, true, true);

  // M5Stack::begin(bool LCDEnable, bool SDEnable, bool SerialEnable, bool I2CEnable)

  Serial.printf("WW-> empieza setup : %d \n", millis());
  struct Contests *fltdata = NULL;
  int maxpriority = configMAX_PRIORITIES;
  TaskHandle_t Task0, Task1;

  // Serial.begin(115200); // comentar esto si queremos que no salga nada por el usb

  //.... D_funciones_Display Linea 10 y 12
  Wire.begin(GPIO_NUM_21, GPIO_NUM_22); // M%STACK uses std wire /I2C pins
  // Wire.begin(GPIO_NUM_16, GPIO_NUM_17); // SDA, SCL Usado por el barometro y el OLED  del KEEPER
 Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  wifiDisconnect();
  /*
    Power chip connected to gpio21, gpio22, I2C device
    Set battery charging voltage and current
    If used battery, please call this function in your project
  */
  M5.Power.begin();
  M5.Power.setPowerBoostOnOff(1);

  bat_level = M5.Power.getBatteryLevel();
  Serial.printf("Battery Level = %d Can control %d  \n", bat_level, M5.Power.canControl());

  // M5.Lcd.setBrightness(250);
  M5.Speaker.setVolume(0);
  M5.Speaker.update();
  //Serial.printf("WW-> Fin de M5 : %d \n", millis());
  M5.Lcd.setFreeFont(FMB9);
  M5.lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(0, 20);
  M5.Lcd.println("Glider Timer " + versionkeeper);
  M5.Lcd.println();

  if (!SPIFFS.begin())
  {
    // report_error(err_code_spiffs);
  } // Start the SPI Flash Files System
  // Serial.println();

  Wire.begin(GPIO_NUM_21, GPIO_NUM_22);
  readGliderKeeperConfig();
  flight.init(); // se leen los settings
  GLOBAL_DURATION=flight.look_for_duration_from_file();
  Serial.printf("duration from file %d \n", GLOBAL_DURATION);

  // FLTnum = competition.init(); // El último vuelo está aquí

  GT.stored_competition_no = competition.init(); //// competition number that was opened
  Serial.print("competition inited ");
  Serial.println(GT.stored_competition_no);
  M5.Lcd.println("Loading index table");

  if (GT.stored_competition_no > 0)
  {
    fltdata = competition.getFlightData(GT.stored_competition_no);

    Serial.println("---------------------------");
    Serial.printf("competion was: %d Data Version was: %d FwData version is %d \n", fltdata->head.contestNum, fltdata->head.data_version, DATA_VERSION);
    Serial.println("---------------------------");
    if (fltdata->head.data_version != DATA_VERSION)
    {
      M5.Lcd.println("MISMATCH DATA");
      M5.Lcd.println("  Formatting...");
      M5.Lcd.println("Unit will reset");
      bool exito = competition.format();
      Serial.printf("<<<<<<<<<<<<<<<<<<<format completed exito %d \n", exito);
      ESP.restart();
    }
    else
    {
      M5.Lcd.println("Dataformat:OK");
      Serial.println("Stored COMPETITION  :");
      Serial.print("Conmp:no ");
      Serial.println(GT.stored_competition_no);
      Serial.print("Rounds: ");
      Serial.println(fltdata->head.number_of_rounds);
      Serial.print("Groups: ");
      Serial.println(fltdata->head.number_of_groups);
      Serial.print("number_of_pilots: ");
      Serial.println(fltdata->head.number_of_pilots);
    }
  }
  else
  {
    M5.Lcd.println("Dataformat:BLANK");
  }
  M5.Lcd.println();  

  set_server_functions();
  M5.Lcd.println("loading fonts");

  GD.begin(); 
  M5.Lcd.println("loading competition");
  M5.Lcd.println();

  GT.begin();
  //Serial.printf("WW-> fin de GT.begin : %d \n", millis());
  M5.Lcd.println("looking for MP3 player");
  M5.Lcd.println();

  MP3.setup();

  //Serial.printf("WW-> fin de MP3.setup : %d \n", millis());

  GT.update_setting_labels();
  FLTdata.begin();

  //Serial.printf("WW-> fin de setting labels : %d \n", millis());

 

  //   //M5.Lcd.drawBmpFile(SPIFFS, "/LogoKeeper2.bmp", 0, 0);
  //   M5.Lcd.setTextColor(BLUE, WHITE);
  //   M5.Lcd.setCursor(55, 197);
  //   //M5.Lcd.setTextSize(2);

  //   M5.Lcd.setFreeFont(FSS9);
  //   //GDsetTextSize(2);

  //   M5.Lcd.setTextSize(1);
  //   M5.Lcd.print("Altimeter");

  //   M5.Lcd.print(" & ");
  //   //M5.Lcd.setTextSize(2);
  //   M5.Lcd.print("Timekeeper");
  //   M5.Lcd.setCursor(21, 236);
  //   M5.Lcd.setFreeFont(FMB9);
  //   //M5.Lcd.setTextSize(2);
  //  // M5.Lcd.print("     Menu         Scroll          Select");
  //   M5.Lcd.print(button_label);

  pinMode(gpio_in_pin, INPUT_PULLUP);
  pinMode(gpio_pin, OUTPUT);
  M5.Lcd.println("Updating Clock");
  M5.Lcd.println();
  //Serial.printf("WW-> principio de reloj : %d \n", millis());

  local_RTC.ReadRTC();
  if (local_RTC.keeper_Synchro_status == RTC_AVAILABLE)
  {
    local_RTC.ConfigLocalTimeFromRTC();
    local_RTC.read_last_sync();
    Serial.println("RTC_AVAILABLE ++++++++++++++++++++++");
  }
  
 // Serial.printf("WW-> fin reloj : %d \n", millis());

  MP3.volume(0);
  // MP3.play_nothing();
  if (flight.panel_type == I2C_IF)
  {
    initMCPs();
  }


  M5.Lcd.println("Setting peripherals");
  M5.Lcd.println();
  M5.Lcd.println("Setting WiFi");
  M5.Lcd.println();
  Serial.printf("WW-> fin setup : %d \n", millis());
  //Serial.printf("WW-> empieza dibujo : %d \n", millis());
  M5.Lcd.drawJpgFile(SPIFFS, "/LogoKeeper2.jpg", 0, 0);
 // Serial.printf("WW-> fin sdibujo : %d \n", millis());
  // M5.Lcd.drawBmpFile(SPIFFS, "/LogoKeeper2.bmp", 0, 0);
  M5.Lcd.setTextColor(BLUE, WHITE);
  M5.Lcd.setCursor(55, 197);
  // M5.Lcd.setTextSize(2);
  M5.Lcd.setFreeFont(FSS9);
  // GDsetTextSize(2);
  M5.Lcd.setTextSize(1);
  M5.Lcd.print("Altimeter");
  M5.Lcd.print(" & ");
  // M5.Lcd.setTextSize(2);
  M5.Lcd.print("Timekeeper");
  M5.Lcd.setCursor(21, 236);
  M5.Lcd.setFreeFont(FMB9);
  // M5.Lcd.setTextSize(2);
  // M5.Lcd.print("     Menu         Scroll          Select");
  M5.Lcd.print(button_label);


}

void blink_ontime(bool blink_ontime_flag) ////Blink ontime shloug only be used in PATATA
{
  // if (blink_ontime_flag)
  // {
  //   struct timeval xx;
  //   gettimeofday(&xx, NULL);
  //   digitalWrite(gpio_pin, (bool)(xx.tv_sec % 2));
  //   //Serial.printf(" RTC status %d \n", local_RTC.keeper_Synchro_status);
  // }
  // if (check_for_mp3)
  // {
  //   busy_mp3 = !digitalRead(gpio_in_pin);
  //   if (busy_mp3)
  //   {
  //     uint32_t mp3_lag = millis() - millis_from_cmd;
  //     //Serial.printf("LAG to receive a BUSY Signal %d \n",mp3_lag);
  //     check_for_mp3 = 0;
  //   }
  // }
}

void beginUdp()
{
  // clear_Myudp();

  // tc.toCharArray(replyPacket, 50);

  // Udp.begin(localUdpPort);
  if (udp.listen(myudp.localUdpPort))
  {
    Serial.print("UDP Listening on IP: ");
    Serial.println(WiFi.localIP());
    udp.onPacket([](AsyncUDPPacket packet)
                 {
                   //handleUdp(packet);
                   // Serial.print("UDP Packet Type: ");
                   // Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast"
                   //                                                                        : "Unicast");
                   // Serial.print(", From: ");
                   // Serial.print(packet.remoteIP());
                   // Serial.print(":");
                   // Serial.print(packet.remotePort());
                   // Serial.print(", To: ");
                   // Serial.print(packet.localIP());
                   // Serial.print(":");
                   // Serial.print(packet.localPort());
                   // Serial.print(", Length: ");
                   // Serial.print(packet.length());
                   //Serial.print(",raw  Data: ");
                   //Serial.write(packet.data(), packet.length());
                   // Serial.println();
                   //reply to the client
                   //packet.printf("Got %u bytes of data", packet.length());
                   // memcpy(incomingPacket, packet.data(), packet.length());
                   //extern char incoming_Pack [100];
                   extern volatile bool packet_received;
                   extern Myudp myudp;
                   myudp.millis_in = millis();
                   //Serial.println(millis());
                   // incoming_package = packet.readString();
                   // incoming_package += ";"+String(millis());

                   memcpy(&myudp.incomingPacket, packet.data(), packet.length());
                   myudp.incomingPacket[packet.length()] = 0; //For string termination
                   myudp.incoming_package = String(myudp.incomingPacket);
                   packet_received = 1;
                   // Serial.print("myudp.incomingPacket ");
                   // Serial.println(myudp.incomingPacket);
                   packet.flush();

                   //sprintf(&incomingPacket, const char *format-string, argument-list); });
    Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), myudp.localUdpPort); });
  }
}

void loop()
{

  static bool STA_connected = 0;
  static uint last_loop_millis = 0;
  static uint loop_counter = 0;
 

  if ((millis() - last_loop_millis) > 10000)
  {
    // Serial.println(digitalRead(gpio_in_pin));
    // Serial.print((millis()-last_loop_millis));
    // Serial.print("Average loop time, msec: ");
    float xx = (float)((millis() - last_loop_millis));
    xx = xx / loop_counter;
    // Serial.println(xx,3);
    loop_counter = 0;
    last_loop_millis = millis();
  }
  else
  {
    loop_counter++;
  }
  if (sinWifi)
  {
    // GD.displaywelcomescreen();
    //  M5.Lcd.println("Setting WiFi");
    //  M5.Lcd.println();
    STA_connected = connecttowifi();
    sinWifi = 0;
    // server.begin();

    if (STA_connected)
    {
      // Blue_led.led_blink(BLINK_NORMAL);
      Debug.println("BLINK NORMAL");
      uint8_t byte3 = WiFi.localIP()[2];
      if (byte3 == 4)
      {
        myudp.begin();
        beginUdp();
      }
      else
      {
        local_RTC.ConfigLocalTimeFromNTP(1);
      }
    }
    else
    {
      // Blue_led.led_blink(BLINK_LONG);
     // Debug.print("BLINK LONG");
      //Serial.printf(" stored : %d\n", GT.stored_record_num);
    }
  }
  else
  {
    // static unsigned long last_wifimillis = 0;
    // //Debug.println("wifi Looop :"+String((millis()-last_wifimillis)));
    // last_wifimillis = millis();
    if (STA_connected)
    {
      // ArduinoOTA.handle();
    }
    else
    {
    }
    if (udp_in_progress)
    {
      // myudp.handleUdp();
    }
    else
    {
      //////////////// TODO quitado captive 
      // if (flight.panel_type == NO_PANEL) /// only no panel units will have DNS requests capability  JHR 08 may 2022
      // {
      //   dnsServer.processNextRequest();
      // }
    }
    // server.handleClient();
    //  myudp.handleUdp();
  }

  GT.button_update();

  switch (FLT_mode)
  {
  case PATATA:
  {
    if (flight.in_flight_status)
    {
      // Serial.print(".");
    }
    else
    {
      switch (page_to_show)
      {
      case MENU_PAGE:
        GT.manage_management();
        break;
      case ROUND_PAGE:
        GT.manage_show_round_data();
        break;
      case PILOT_PAGE:
        GT.manage_show_pilot_data();
        break;
      case ERROR_PAGE:
        GT.manage_show_error_data();
        break;
      }
    }
  }
  break;
  case SCAN_AP:
  {
    scan_wifi();
    FLT_mode = PATATA;
  }
  break;
  case PICK_DATA:
  {
    switch (connect_to_AP(GT.selectionAP)) // with connection result
    {
    case MAIN_STA:
    {
      if (GT.scheduled)
      {
        GT.menupointer = 1;
        GT.menulevel = 1;
        FLT_mode = CONNECTED_AS_STA;  ///3rd file main_sta=1

      }
      else
      {
        GT.receiving_working_time = 1; ////TODO not clear this is the place...
        delay(100);
        Serial.println("PICK_DATA");
        if (FLTdata.str_splitPL(get_last_FLTdata()))
        {
          STA_connected = 1;

          FLT_mode = NEW_KEEPER_TFD;
          packet_received = 0;
          Serial.flush();
          delay(19);
          M5.Speaker.tone(NOTE_DH2, 100);
          delay(100);
          M5.Speaker.tone(NOTE_DL5, 50);
          delay(100); // to allow the bell???
          myudp.syncro_status = SUCCESS_SYCHRONISING;
        }
        else
        {
          Serial.println("Data not a flight");
          GD.alabel = "FAIL: Reset, pls";
          GD.partial_flt();
          FLT_mode = CONFIRM_TFD_UPLOAD;
          GT.firstime = 0;
        }
      }
    }
    break;
    case TIMER_STA:
    {
      GT.receiving_working_time = 1; ////TODO not clear this is the place...
      delay(100);
      Serial.println("TIMER_STA");
      if (FLTdata.str_splitPL(get_last_FLTdata()))
      {
        STA_connected = 1;

        FLT_mode = NEW_KEEPER_TFD;
        packet_received = 0;
        Serial.flush();
        delay(19);
        M5.Speaker.tone(NOTE_DH2, 100);
        delay(100);
        M5.Speaker.tone(NOTE_DL5, 50);
        delay(100); // to allow the bell???
        myudp.syncro_status = SUCCESS_SYCHRONISING;
      }
      else
      {
        Serial.println("Data not a flight");
        GD.alabel = "FAIL: Reset, pls";
        GD.partial_flt();
        FLT_mode = CONFIRM_TFD_UPLOAD;
        GT.firstime = 0;
      }
    }
    break;
    case KEEPER_STA:
    {
      delay(100);
      Serial.println("KEEPER_STA");
      if (FLTdata.str_splitPL(get_last_FLTdata()))
      {
        STA_connected = 1;
        FLT_mode = NEW_KEEPER_TFD;
        packet_received = 0;
        myudp.clear_Myudp();
        M5.Speaker.tone(NOTE_DH2, 100);
        delay(100);
        M5.Speaker.tone(NOTE_DL5, 50);
        if (!FLTdata.check_sychro_needed())
        { // will jump to a mode where no need for synchro
          myudp.syncro_status = SUCCESS_SYCHRONISING;
          delay(100); /// to allow the bell
        }
        else
        {
          myudp.syncro_status = SYCHRONISING;
        }
      }
      else
      {
        // Serial.println("Data not a flight");
        GT.manage_error(ERROR_DATA);
      }
    }
    break;
    case HOME_STA:
    {
      GT.menupointer = 1;
      GT.menulevel = 1;
      // page_to_show=MENU_PAGE;
      // FLT_mode = PATATA;
      // GD.menu_displayUpdate(menupointer);
      // page_to_show = ERROR_PAGE;

      FLT_mode = CONNECTED_AS_STA;
    }
    break;
    case THROW_STA:
    {
      FLT_mode = GLIDERTHROW;
    }
    break;
    case THROW4_STA:
    {
      FLT_mode = GLIDERTHROW;
    }
    break;
    default:
    {
      // Serial.println("timeout of connection");
      GT.manage_error(ERROR_AP);
    }
    break; /// break de selection AP
    }
    break; /// break de FLTmode
  }
  case NEW_KEEPER_TFD: /// Ha llegado algo desde /timer
  {
    udp_in_progress = 1;
    switch (myudp.syncro_status)
    {
    case SYCHRONISING:
      delay(90);
      // Serial.print("entro time shift");Serial.println(millis());
      // Serial.print("Syncronysing");
      myudp.return_timeshift(STA_connected);

      // Serial.print("salgo time shift");Serial.println(millis());
      // Serial.println(" Syncronising return");
      break;
    case SUCCESS_SYCHRONISING:
      myudp.clear_Myudp();
      Serial.println("SUCCESS");
      M5.Speaker.tone(NOTE_DL5, 50);
      delay(50);
      M5.Speaker.tone(NOTE_DH2, 100);
      delay(100);
      FLT_mode = NEW_DATA;
      udp_in_progress = 0;
      break;
    case FAILED_SYCHRONISING:
      flight.boop();
      myudp.clear_Myudp();
      udp_in_progress = 0;
      FLT_mode = UNKNOWN;
      Serial.println("FAILED SYNCHRO");
      break;

    default:
      break;
    }
  }
  break;
  case GLIDERTHROW:
    // gth.manage_throw();//TODO quitando Gthrow
    break;
  case CONNECTED_AS_STA:
    GD.STA_disp();
    delay(10);
    //Serial.printf("ConfigLocalTimeFromNTP status= %d \n",local_RTC.keeper_Synchro_status);
    local_RTC.ConfigLocalTimeFromNTP(0);
    if (local_RTC.syncEventTriggered)
        {
          local_RTC.syncEventTriggered = false;
          local_RTC.processSyncEvent(local_RTC.ntpEvent);
          Serial.print("Event: ");
          Serial.println(local_RTC.my_event_string);
        }
    if (buttonA)
    {
      GD.firsttime_settings = 1;
      GT.back_to_prev_status();
      buttonC = 0;
    }
    if (buttonB)
    {
      // GT.back_to_prev_status();
      download_scheduled_rounds(3, 1);  ///3rd file main_sta=1      
      buttonB = 0;
    }
    if ((buttonC))
    {
      
      send_loop_tfd();
      buttonC = 0;
    }
    break;
  // case RECEIVED_ROUNDS:
  //   GD.received_rounds_disp();
  //   delay(10);
    
  //   if (buttonA)
  //   {
  //     GD.firsttime_settings = 1;
  //     GT.back_to_prev_status();
  //     buttonC = 0;
  //   }
  //   if (buttonB)
  //   {
  //     send_loop_tfd();
  //     buttonB = 0;
  //   }
  //   if ((buttonC))
  //   {
  //     save_file();
  //     buttonC = 0;
  //   }
  //   break;  

  case UPDATE_IN_PROGRESS:
    GD.update_screen();
    if (GT.stored_competition_no > 0)
    {
      if ((GT.stored_competition_no > 0) && (GD.updating_flash))
      { ///////////////format if there is data
        GT.stored_competition_no = 0;
        Serial.printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>formatting \n");
        bool exito = competition.format();
        Serial.printf("<<<<<<<<<<<<<<<<<<<format completed exito %d \n", exito);
      }
      else
      {
        Serial.printf(">>>>>>>>>>>>>>>>>no format needed data was blank \n");
      }
    }
    delay(200);

    break;
  case ACTION_ABOUT_MODE:
  {
    GD.about_screen();
    if (buttonA)
    {
      GD.firsttime_settings = 1;
      GT.back_to_prev_status();
      buttonC = 0;
    }
    if (buttonB)
    {
      buttonB = 0;
    }
    if ((buttonC))
    {
      GT.back_to_prev_status();
      buttonC = 0;
    }
    delay(10);
  }
  break;
  case NEED_FORMAT:
    if (GT.firstime)
    {
      GD.clean_partial_flt();
      flight.boop();
      GD.alabel = "Must Format";
      GD.blabel = "before this change";
      GD.partial_flt();
      GT.firstime = 0;
      // GD.menu_displayUpdate(GT.menupointer);
    }
    if ((buttonA))
    {
      GT.back_to_prev_status();
      buttonA = 0;
    }
    if (buttonB)
    {
      buttonB = 0;
    }

    if ((buttonC))
    {
      GT.back_to_prev_status();
      buttonC = 0;
    }
    break;

    break;
  case RUN_FLIGHT_REQUEST:
    if (GD.firsttime_run == 1)
    {
      GD.confirm_run_flight();
      GD.firsttime_run = 0;
    }
    if (buttonA)
    {
      GD.firsttime_run = 1;
      GT.back_to_prev_status();
      buttonA = 0;
    }
    if (buttonB)
    {
      buttonB = 0;
    }
    if ((buttonC))
    {
      GD.firsttime_run = 1;
      GT.mycompetition.print_contest();
      Serial.printf("Stored status stored round %d Strored group %d stored recno %d GT.GTmenu[GT.menupointer].new_menulevel %d\n", GT.stored_round_no, GT.stored_group_no, GT.stored_record_num, GT.GTmenu[GT.menupointer].new_menulevel);
      flight.start(GT.GTmenu[GT.menupointer].new_menulevel);
      GT.back_to_prev_status();
      buttonC = 0;
    }
    delay(10);

    break;
  /////////////////////
  case CONFIRM_TFD_UPLOAD:
  {
    if (GT.firstime)
    {
      GD.clean_partial_flt();
      GD.alabel = "  Upload records? ";
      // GD.blabel = "   GK Fw: " + String(FLTdata.tfd.Firm_ver);
      GD.partial_flt();
      GT.firstime = 0;
    }
    if (buttonA)
    {
      GT.receiving_working_time = 0;
      GT.back_to_prev_status();
      buttonA = 0;
    }
    if (buttonB)

    {

      buttonB = 0;
    }
    if (buttonC)
    {
      if (GT.tfd_sent_no == 0)
      {
        GD.clean_partial_flt();
        GD.alabel = "Uploading....";
        GD.partial_flt();
        bool kk = send_loop_tfd();
      }
      else
      {
        GT.tfd_sent_no = 0;
        GT.back_to_prev_status();
      }
      buttonC = 0;
    }
    break;
  }
  case RECEIVED_FROM_TIMER:
  {
    // if (true)
    if (GT.firstime)
    {
      GD.clean_partial_flt();
      GD.alabel = "Received " + String(GT.tfd_received) + " rcds";
      GD.partial_flt();
      // GT.save_record();
      GT.firstime = 0;
    }
    if (buttonA)
    {
      GT.tfd_received = 0;
      GT.back_to_prev_status();
      buttonA = 0;
    }
    if (buttonB)
    {
      buttonB = 0;
    }
    if (buttonC)
    {
      GT.tfd_received = 0;
      GT.back_to_prev_status();
      buttonC = 0;
    }
    break;
  }

    //////////////////

  default:
  {
    GT.manage_wifi_input(); // this is like a new list for cases in FLT_mode in GTimer.cpp
  }
  break;
  }

  flight.manage_flight();

  if ((GD.sched_adv_millis_for_partial < millis()) && (GD.sched_adv_millis_for_partial > 0))
  {
    GD.sched_adv_millis_for_partial = 0;
    GD.displayStatus();
  }
  if (FLT_mode == NEW_KEEPER_TFD)
  {
    delay(10);
  }
  if (packet_received)
  {
    myudp.handleUdp();
    packet_received = 0;
  }
  blink_ontime(blink_ontime_flag);
  bool firsttime = 1;
  while (Serial1.available())
  {
    if (firsttime)
    {
      Serial.print("Read from panel bus: ");
      firsttime = 0;
    }
    Serial.print(char(Serial1.read()));
  }
}
