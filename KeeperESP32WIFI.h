///

#ifndef keeperESP32WIFI_h
#define keeperESP32WIFI_h


#include <WiFi.h>
#include <WiFiClient.h>
//** #include <WebServer.h>
#include <AsyncTCP.h> //**
#include <AsyncUDP.h>
#include <ESPAsyncWebServer.h> //**
#include <ESPmDNS.h>
#include <DNSServer.h> //captive
#include "src/Myudp.h"
#include "DebugPrint.h"
//update
// **#include <Update.h>
///OTA
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//#include <ArduinoOTA.h>
#include <WiFiMulti.h>   //**
#include "RTC.h"  //**


#include "src/CompetitionSavedRW.h"
#include "src/GTimer.h"
#include "src/GTdisplay.h"
#include "src/Gthrow.h"
#include "src/FlightManager.h"
////////////

//** const char* updateFirm = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update Firmware'></form>";
//** const char* updateSpiff = "<form method='POST' action='/updateUI' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update UI'></form>";
////////
//String keeperIP="";

#define FILESYSTEM SPIFFS
// // You only need to format the filesystem once
// #define FORMAT_FILESYSTEM false
// #define Debug Serial

// #if FILESYSTEM == FFat
// #include <FFat.h>
// #endif
// #if FILESYSTEM == SPIFFS
// #include <SPIFFS.h>
// #endif


// const char* ssid = "RasmaWiFi";
// const char* password = "camaleon";
// const char* ssid = "RoderoAP";
// const char* password = "culogordo";

// const char* ssid = "MiFibra-4B62";
// const char* password = "culogordoplus";

// const char* ssid = "GliderKeeper";
// const char* password = "123456789";


// String localssid="GliderTimer";
// String localpassword="123456789";




//** extern WebServer server;
//** extern DNSServer dnsServer;
static bool STAconnected = 1;

extern AsyncWebServer server;    //** 
extern DNSServer dnsServer;   //**
extern RTC local_RTC;   //**

extern void wifiDisconnect();   //**
extern void readGliderKeeperConfig();   //**
extern void set_server_functions();   //**
extern bool connecttowifi(); 
  //**



//holds the current upload
extern File fsUploadFile;
//extern GTdisplay GD;

#endif
