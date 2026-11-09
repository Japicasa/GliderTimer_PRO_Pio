#ifndef StructSavedRW_h
#define StructSavedRW_h

#include <functional>
#include "esp_partition.h"
#include <esp_ota_ops.h>
#include "../keeper.h"
#include "../DebugPrint.h"
#include "Timer_flight_data.h"
// #include "Draw_list.h"
#include "Contests.h"

//CompetitionsavedRW

/**
 *  +--------------+
 *  | DIRECTORY    |
 *  +--------------+
 *  | METADATA     |
 *  +--------------+
 *  | PILOTS       |
 *  +--------------+
 *  | RECORDS DATA |->fltdata (datastore)
 *  .              .
 *  .              .
 *  +--------------+
 * 
 * */

#define PAGESIZE 4096
#define START_METADATA 1 // Start block for Flight metadata page
#define NUMBER_OF_METADATA_BLOCKS 10
#define NUMBER_OF_PILOTS_BLOCKS 0
#define NUMBER_OF_BUFFER_BLOCKS 0
const int NUMBER_OF_PAGE_HEADERS = START_METADATA + NUMBER_OF_METADATA_BLOCKS + NUMBER_OF_PILOTS_BLOCKS+NUMBER_OF_BUFFER_BLOCKS;
const int START_METADATA_PAGE = START_METADATA * PAGESIZE;
const int START_PILOTS_PAGE = (START_METADATA + NUMBER_OF_METADATA_BLOCKS) * PAGESIZE;
const int START_BUFFERS_PAGE = (START_METADATA + NUMBER_OF_METADATA_BLOCKS+NUMBER_OF_PILOTS_BLOCKS) * PAGESIZE;

const int START_DATA_PAGE = PAGESIZE * NUMBER_OF_PAGE_HEADERS;  // Where page for flight records starts
const int RECORDSPERPAGE = PAGESIZE / sizeof(struct datastore ); // How many records we can store on each page


struct flash_status
{
    uint32_t totalFlashSize;
    uint16_t recordsPerPage;
    uint8_t flightRecordSize;
    uint32_t totalFlashForRecords;
    uint16_t currentFlight;
    uint16_t numberOfRecordsUsed;
    uint8_t maxFlightsAllowed;
    uint32_t maxRecordsAllowed;
    uint16_t totalBlocksUsed;
};

class CompetitionSaved
{

private:
    const esp_partition_t *keeperpartition;
    uint32_t currentFlight;
    uint32_t currentPos;          // Where to store or read next flight on Directory page
    uint32_t currentRecordPos;    // Where to read next datastore record
    uint32_t currentPage;         // First free page
    uint32_t currentRecordInPage; // Keep records count on the current page

    uint32_t currentBlockSend; // Used to send binary FLash block to App

    long flight2pos(short flight_number);
    //   bool saveCurrentFlightMetadataHeader();
    bool saveCurrentFlightDirectory();
    bool checkEmpyPartition();

public:
    void partloop(esp_partition_type_t part_type);
    bool format();                            // Format flash partition
    unsigned short init();                    // Initialize the flight, returns last flight
    bool begin_competition(uint32_t flight_number); // A competition start
    bool save_record(struct datastore  *datos);
    bool erase_competition(struct Contests *fltdata); // for marking for erase a competition
    bool save_competition(struct Contests *fltdata);
    bool save_competition_head(struct Contests_Head *fltdata);
    struct datastore  *getFirstRecord();
    // struct datastore  *getFirstRecord(unsigned short flight);
    bool getFirstRecord(unsigned short flight, struct datastore *datos);
    bool getNextRecord(struct datastore  *data);
    struct datastore  *getPrevRecord();                      //JHR 8 04 2020
    struct datastore  *getLastRecord(unsigned short flight); //JHR 09 04 2020
    struct Contests *getFlightData(unsigned short flight);     //Load competition, pointer
    struct Contests Load_Competition(unsigned short flight);  //Load competition struct
    struct flash_status *getFlashStatus();
    uint8_t *getFirstFlashBlock();
    uint8_t *getNextFlashBlock();
    //void move_block();  //usado para cambiar formato de datos
};

#endif