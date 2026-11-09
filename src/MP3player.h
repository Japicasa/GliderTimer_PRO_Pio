
/* Generated from GenMyModel */

#ifndef DEF_MP3PLAYER
#define DEF_MP3PLAYER

//#include <iostream>
#include <Arduino.h>
#include <stdint.h>
#include <sys/time.h>
#include "DFRobotDFPlayerMini.h"
//SoftwareSerial mySoftwareSerial(D7, D8); // RX, TX

#define RXD2 16
#define TXD2 17
#define MUSIC_VOL 15
#define ADV_VOL 30
#define FILES_IN_FOLDER 11

class MP3player
{
public:
	DFRobotDFPlayerMini myDFPlayer;
	int music_vol;
	int adv_vol;
	int actual_vol;
	uint8_t file_to_play;
	bool firstime=1;
	void setup();
	void play_something(int filetoplay_request = 0);
	void play_nothing();
	void ad_file(int filetoplay);
	void stop();
	void play(int filetoplay);
	void volume_decr();
	void volume(int vol_dem);
	void volume_incr();
	void printDetail(uint8_t type, int value);
};

#endif
