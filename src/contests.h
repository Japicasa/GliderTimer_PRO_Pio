
/* Contest.h */

#ifndef DEF_CONTESTS
#define DEF_CONTESTS
#include <Arduino.h>
#include <stdint.h>
#include <sys/time.h>
#include "keeper.h"

#define DATA_VERSION 0x2225

#define MAX_SELECTION_POINTER_COMPETITION 6

#define MAX_SELECTION_POINTER_ROUND 7 // one more for the struct round 0 is the final class
#define MAX_SELECTION_POINTER_GROUP 2  // one more for the struct Group 0 is the final class
#define MAX_SELECTION_POINTER_PILOT 10 // one more for the struct, leave pilot=0 blank 1.0.4  
#define GROUP_MAX_ROUND 13
#define GROUP_MAX_GROUP 12
#define GROUP_MAX_SELECTION_POINTER_ROUND 13
#define GROUP_MAX_SELECTION_POINTER_PILOT MAX_SELECTION_TABLE_PILOTS
//#define MAX_SELECTION_POINTER_PILOT 36 // one more for the struct, leave pilot=0 blank 2.0.4
typedef struct Contests_Head
{
	
	uint16_t contestNum;
	uint16_t data_version;
	uint32_t number_of_rounds;
	uint32_t number_of_groups;
	uint32_t number_of_pilots;
	uint32_t pilots_per_round;
	uint32_t external_comp_no;
	char external_comp_ID[24]="1CFFFFFFFFF7";
	uint32_t groups_enabled;
	// uint32_t strict_landing_rules; //TODO???
	uint32_t draw_locked;
	uint32_t landing_millis_allowance; // milliseconds
	uint32_t flt_duration;			   //seconds
};
typedef struct Draw_list
{
public:
	uint8_t pilot_number;	//ignore pilot number= 0, corresponds with the table index in pilots_table			
	uint8_t round;
	uint8_t group;
	uint8_t discarded_round;
	uint16_t pilot_id; //One in the pilots_table
	uint16_t record_num;
	uint16_t round_rank;
	uint16_t FAI_score;
		//uint32_t penalty;
	
	
};

// typedef struct Draw_table
// {
// public:
// 	uint32_t pilot_number; //ignore pilot number= 0, corresponds with the table index
// 	uint32_t pilot_id;	   //One in the pilots_table
// 	uint8_t round;
// 	uint8_t group;  	// index = (round-1) *MAX ROUND NUMBER (6?) + pilot bib
// 	uint16_t landing_spot;
// };

typedef struct Epoch_table
{
public:
	uint8_t round;
	uint8_t group;
	uint16_t delay_in_launch;  //TODO Useless
	uint32_t launch_epoch;
};
class Contests
{
public:
	struct Contests_Head head;
	struct Draw_list draw_list [MAX_SELECTION_POINTER_ROUND][MAX_SELECTION_POINTER_PILOT];// round=0 means final; pilot=0 leave empty
	
		  //index is (round-1)*number_of_pilots+group
	//struct Draw_table draw_table[MAX_SELECTION_POINTER_ROUND][MAX_SELECTION_POINTER_PILOT];	
	void calculate_scores();
	void read_rank_list();
	void print_contest();
	void close_competition();
};
#endif
