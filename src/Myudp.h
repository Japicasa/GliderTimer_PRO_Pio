

#ifndef DEF_MYUDP
#define DEF_MYUDP

//#include <iostream>

#include <Arduino.h>
//#include <stdint.h>

#include <sys/time.h>
//#include <ESP8266WiFi.h>
#include <WiFi.h> // Si utilizas el ESP32, debes ponerlo así.
// #include <WiFiUdp.h>
#include "AsyncUDP.h"
#include "Timer_flight_data.h"
#include "GTimer.h"

#define ITERATIONS_FOR_REFERENCE 10
#define TIME_FOR_UDP_RETRY 2500 //miliseconds
#define MAX_PACKET_LOST 10
#define GOOD_TRIP_MS 300
#define SYCHRONISING 1
#define NOT_SYCHRONISING 0
#define SUCCESS_SYCHRONISING 3
#define FAILED_SYCHRONISING 4


class Myudp
{
	public:
	// WiFiUDP Udp;
	

	unsigned int localUdpPort = 3333;
	//const char * udpAddress = "192.168.4.2";
	IPAddress udpAddress;
						  // local port to listen on
	char incomingPacket[255];							  // buffer for incoming packets
	char replyPacket[50] ; // a reply string to send back
	//char replyPacket[] = "Hi there! Got the message :-)"; // a reply string to send back
	uint32_t millis_out;
	uint32_t millis_in;
	uint32_t timer_out_millis_inpack;
	uint32_t round_trip;
	uint32_t seq=1;
	uint32_t out_seq=0;
	bool wait_flag; //1 if there is a web request
	bool mypacket_received;
	
	String tc = "hi  there soy yo";
    String incoming_package;
	uint32_t incomingPacket_millis;
	
	timeval timer_udp_timeval;
	timeval keeper_udp_timeval;
	timeshift my_timer_to_keeper;
	int my_reference_sec;
	int my_reference_shift;
	int my_reference_usec;
	int timeshift_iteration_counter=-5;  //will discard the first 5 iterations
	int syncro_status;
	int packet_lost;
	
	void begin();
	void handleUdp();
	void return_timeshift(bool STA_connected);
	void extract_pack_times();
	void evaluate_timeshift();
	void clear_Myudp();
};



#endif
