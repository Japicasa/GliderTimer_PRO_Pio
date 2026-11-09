
/* Generated from GenMyModel */

#ifndef DEF_GTHOW_H
#define DEF_GTHOW_H

//#include <iostream>
#include <Arduino.h>
#include <stdint.h>

#include <sys/time.h>
#include <M5Stack.h>

#include <WiFi.h>
#include <WiFiClient.h>
//#include <WebServer.h>
#include <ESPmDNS.h>
#include <DNSServer.h> //captive
#include <HTTPClient.h>

#include "GTdisplay.h"

#define ANGLEVIEW 0
#define THROWVIEW 1
#define CHORD_SET_1 2
#define ANGLE_RESET 3
#define QUAD_ANGLE_RESET 4
#define ANGLEVIEW3 5
#define THROWVIEW3 6
#define CHORD_SET_2 7
#define CHORD_SET_3 8
#define CHORD_SET_4 9
#define ANGLEVIEW4 10
#define THROWVIEW4 11

class Gthrow
{
public:
    const char *ssid = "GliderThrow4";
	const char *ssid1 = "GliderThrow";
	const char *ssid4 = "GliderThrow4";
	const char *password = "123456789";
	const char *host = "glidertimer";

	uint32_t throw_status;
	uint32_t last_throw_status = 100;
	float angle_1;
	float angle_2;
	float angle_3;
	float angle_4;
	float diff2_1;
	float diff4_3;
	uint32_t chord_1;
	uint32_t chord_2;
	uint32_t chord_3;
	uint32_t chord_4;
	bool first_chord = 1;
	bool use_payload = 1;
	float throw_1;
	float throw_2;
	float throw_3;
	float throw_4;
	String trag;
	String payload;
	String TC;

	void button_update();
	void begin(int throw_type);
	void client_request();
	void payload2values();
	void manage_throw();
	void simplesplit();
	void longsplit();
	void split_payload();
	void next_chord();
	void chord_increase();
	void chord_decrease();
	String padea5(float num);
	String padea3(int num);
	void chord_button_and_display();
	void tests_trag(String trag, bool needs_payload);
	void toggle_trag(String trag, bool needs_payload);
};
extern bool button_pressed;
extern bool buttonA;
extern bool buttonB;
extern bool buttonC;

#endif
