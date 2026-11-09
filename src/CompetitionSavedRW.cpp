#include "CompetitionSavedRW.h"
#include <Arduino.h>
#include "../DebugPrint.h"
extern DebugPrint Debug;
/**
 * Inicializa la Flash si no lo estuviera y devuelve el último vuelo o -1 en caso de error
 * 
 * 
 * */

void CompetitionSaved::partloop(esp_partition_type_t part_type)
{
    esp_partition_iterator_t iterator = NULL;
    const esp_partition_t *next_partition = NULL;
    iterator = esp_partition_find(part_type, ESP_PARTITION_SUBTYPE_ANY, NULL);
    while (iterator)
    {
        next_partition = esp_partition_get(iterator);
        if (next_partition != NULL)
        {
            Serial.printf("partiton addr: 0x%06x; size: 0x%06x; label: %s\n", next_partition->address, next_partition->size, next_partition->label);
            iterator = esp_partition_next(iterator);
        }
        Serial.println("boot partition");
    }
}
unsigned short CompetitionSaved::init()
{
    //  partloop(ESP_PARTITION_TYPE_APP);  //together with function partloop() above will display existing partitions
    //  partloop(ESP_PARTITION_TYPE_DATA); // JHR 13/may/2021
    esp_err_t error;

    int pos = 0;
    int flt = 0;
    currentFlight = 0;
    currentPos = 0; // Where to write next flight number on Directory Page

    keeperpartition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "keeper");
    if (!keeperpartition)
        return -1;
    //   Serial.printf("Start 0x%x Size 0x%x", keeperpartition->address, keeperpartition->size);

    // Buscamos el primer bloque libre
    while (flt != -1)
    {
        currentFlight = flt;
        error = esp_partition_read(keeperpartition, pos, &flt, sizeof(flt));
        if (error)
            return -1;
        currentPos = pos;
        pos += sizeof(flt);
    }
    currentPage = pos / sizeof(flt) - 1; // First page usable from START_DATA_PAGE
    return currentFlight;
}

bool CompetitionSaved::begin_competition(uint32_t flight_number)
{
    esp_err_t error;

    if (flight_number < 1)
        return false;
    currentFlight = flight_number;

    if (!saveCurrentFlightDirectory())
        return false;

    currentRecordInPage = 0;

    // Save only the fligt number on the flight metadata block
    // this would save 4 bytes...
    // error = esp_partition_write(keeperpartition, flight2pos(flight_number), &flight_number, sizeof(flight_number));
    // if (error)
    //     return false;
    return true;
}

bool CompetitionSaved::save_record(struct datastore *datos)
{
    esp_err_t error;

    Serial.printf("Saving a RECORD: Vuelo %d, currentRecordInPage: %d, currentPage: %d\n", currentFlight, currentRecordInPage, currentPage);
    int pos = currentRecordInPage * sizeof(struct datastore) + currentPage * PAGESIZE + START_DATA_PAGE;
    error = esp_partition_write(keeperpartition, pos, (const uint8_t *)datos, sizeof(struct datastore));
    if (error != ESP_OK)
        return false;
    currentRecordInPage++;
    if (currentRecordInPage >= RECORDSPERPAGE)
    {
        if (!saveCurrentFlightDirectory())
            return false;
        currentRecordInPage = 0;
        currentPage++;
    }
    return true;
}

bool CompetitionSaved::erase_competition(struct Contests *fltdata)
{
    // esp_err_t error;

    // //error = esp_partition_write(keeperpartition, flight2pos(fltdata->FLTnum), fltdata, &fltdata->landing_millis-&fltdata->FLTnum); TODO arreglar el puntero
    // error = esp_partition_write(keeperpartition, flight2pos(fltdata->FLTnum), fltdata, 16);
    // if (error)
    //     return false;
    // // currentRecordInPage = 0;
    // // currentPage++;
    // return true;
}

bool CompetitionSaved::save_competition(struct Contests *fltdata)
{
    esp_err_t error;

    error = esp_partition_write(keeperpartition, flight2pos(fltdata->head.contestNum), fltdata, sizeof(struct Contests));
    if (error)
        return false;
    // currentRecordInPage = 0;
    // currentPage++;
    return true;
}

bool CompetitionSaved::save_competition_head(struct Contests_Head *fltdata)
{
    esp_err_t error;

    error = esp_partition_write(keeperpartition, flight2pos(fltdata->contestNum), fltdata, sizeof(struct Contests_Head));
    if (error)
        return false;
    // currentRecordInPage = 0;
    // currentPage++;
    return true;
}
struct Contests *CompetitionSaved::getFlightData(unsigned short flight)
{
    Serial.print("SIZE of unsigned short:");
    Serial.println(sizeof(flight));
    static struct Contests fltdata;
    esp_err_t error;

    error = esp_partition_read(keeperpartition, flight2pos(flight), &fltdata, sizeof(struct Contests));
    if (error || fltdata.head.contestNum == -1)
        return NULL;
    return &fltdata;
}
//////////////JHR
struct Contests CompetitionSaved::Load_Competition(unsigned short flight)
{
    static struct Contests fltdata; 
    esp_err_t error;

    error = esp_partition_read(keeperpartition, flight2pos(flight), &fltdata, sizeof(struct Contests));
    // if (error || fltdata.contestNum == -1)
    //     return NULL;
    return fltdata;
}
/////////////////////JHR

struct datastore *CompetitionSaved::getFirstRecord() // nos da el primer registro de la tabla de datastores
{
    static struct datastore datos;
    esp_err_t error;

    currentPage = 0;
    currentPos = 0;
    currentRecordPos = 0;
    currentRecordInPage = 0;

    //   Serial.printf("Read: 0x%x ", START_DATA_PAGE);
    error = esp_partition_read(keeperpartition, START_DATA_PAGE, &datos, sizeof(struct datastore));
    if (error != ESP_OK || datos.fltnum == USHRT_MAX)
        return NULL;

    currentRecordPos += sizeof(sizeof(struct datastore));
    currentRecordInPage++;
    return &datos;
}

// struct datastore *CompetitionSaved::getFirstRecord(unsigned short flight) //// nos da el primer reegistro del Metadad=flight (aqui comp_no es) datastores
// {
//     static struct datastore datos;
//     esp_err_t error;

//     currentPage = -1;
//     currentPos = 0;
//     currentRecordPos = 0;
//     currentRecordInPage = 0;
//     currentFlight = -1;

//     // Looking for the page containing this flight records
//     while (currentFlight != flight)
//     {
//         error = esp_partition_read(keeperpartition, currentPos, &currentFlight, sizeof(currentFlight));
//         if (error != ESP_OK || currentFlight == -1)
//             return NULL;
//         currentPos += sizeof(currentFlight);
//         currentPage++;
//     }

//     int pos = currentRecordInPage * sizeof(struct datastore) + currentPage * PAGESIZE + START_DATA_PAGE;
//     error = esp_partition_read(keeperpartition, pos, &datos, sizeof(struct datastore));
//     currentRecordInPage++;
//     return &datos;
// }
bool CompetitionSaved::getFirstRecord(unsigned short flight, struct datastore *datos) //// nos da el primer reegistro del Metadad=flight (aqui comp_no es) datastores
{
    //static struct datastore datos;
    esp_err_t error;

    currentPage = -1;
    currentPos = 0;
    currentRecordPos = 0;
    currentRecordInPage = 0;
    currentFlight = -1;

    // Looking for the page containing this flight records
    while (currentFlight != flight)
    {
        error = esp_partition_read(keeperpartition, currentPos, &currentFlight, sizeof(currentFlight));
        if (error != ESP_OK || currentFlight == -1)
            return false;
        currentPos += sizeof(currentFlight);
        currentPage++;
        //Serial.print("pos"); Serial.println(currentPos);
    }

    int pos = currentRecordInPage * sizeof(struct datastore) + currentPage * PAGESIZE + START_DATA_PAGE;
    error = esp_partition_read(keeperpartition, pos, datos, sizeof(struct datastore));
    //Serial.printf("esta es la competicion no. leida %d \n",datos->comp_no);
    if (datos->comp_no < 64)
    {
        currentRecordInPage++;
    }
    Serial.printf("First record: pos %d currentPAge %d STARTDATA %d \n",pos,currentPage,START_DATA_PAGE);
    return true;
}
bool CompetitionSaved::getNextRecord(struct datastore *datos)
{
    //static struct datastore datos;
    esp_err_t error;
    int pos = currentRecordInPage * sizeof(struct datastore) + currentPage * PAGESIZE + START_DATA_PAGE;
    error = esp_partition_read(keeperpartition, pos, datos, sizeof(struct datastore));
    if (error != ESP_OK)
        return false;

    if (datos->comp_no > 64)
    {
        return false;
    }

    currentRecordInPage++;
    if (currentRecordInPage >= RECORDSPERPAGE)
    {
        currentRecordInPage = 0;
        currentPage++;
        return true;
    }
    if (datos->comp_no == USHRT_MAX)
    {
        // check if next page has data
        currentRecordInPage = 1;
        currentPage++;

        pos = currentPage * PAGESIZE + START_DATA_PAGE;
        error = esp_partition_read(keeperpartition, pos, &datos, sizeof(struct datastore));
        if (error != ESP_OK || datos->comp_no == USHRT_MAX)
            return false;
    }
    //    Serial.printf("Read: 0x%x Current Next record %d ", pos, currentRecordInPage);
    return true;
}

//////////////////////////////////////////////
//// Devuelve el ultimo Registro del vuelo
//////////////////////////////////////////////
struct datastore *CompetitionSaved::getLastRecord(unsigned short flight) //
{
    Serial.print("mela ESTAS PELANDO!!!!");
    flight = flight + 1;
    static struct datastore datos;
    esp_err_t error;
    currentPage = -1;
    currentPos = 0;
    currentRecordPos = 0;
    currentRecordInPage = 0;
    currentFlight = -1;

    // Looking for the page containing this flight records
    while (currentFlight != flight)
    {
        error = esp_partition_read(keeperpartition, currentPos, &currentFlight, sizeof(currentFlight));
        if (error != ESP_OK || currentFlight == -1)
        {
            Debug.println("START_current REJECTED pos :" + String(currentPos));
            return NULL;
        }
        currentPos += sizeof(currentFlight);
        currentPage++;
    }
    int pos = currentRecordInPage * sizeof(struct datastore) + currentPage * PAGESIZE + START_DATA_PAGE;
    error = esp_partition_read(keeperpartition, pos, &datos, sizeof(struct datastore));
    Debug.println("currentPage : " + String(currentPage));
    Serial.println("START_DATA_PAGE : " + String(START_DATA_PAGE));
    Debug.println("currentRecordInPage : " + String(currentRecordInPage));
    Debug.print("Read First record record: " + String(datos.fltnum) + " Recno. " + String(datos.record_num) + " pos " + String(pos) + " Recinpage " + String(currentRecordInPage));
    // Backward one page and then look for the last record and return it
    currentPage--;
    currentRecordInPage = 0;
    flight = flight - 1;
    currentFlight = flight;
    pos = currentRecordInPage * sizeof(struct datastore) + currentPage * PAGESIZE + START_DATA_PAGE;
    while (currentFlight == flight)
    {
        pos = currentRecordInPage * sizeof(struct datastore) + currentPage * PAGESIZE + START_DATA_PAGE;
        error = esp_partition_read(keeperpartition, pos, &datos, sizeof(struct datastore));
        if (error != ESP_OK || currentFlight == -1)
            return NULL;
        currentFlight = datos.fltnum;
        currentRecordInPage++;
        Debug.println("flight: " + String(flight) + " current " + String(currentFlight));
    }
    currentRecordInPage--;
    pos = currentRecordInPage * sizeof(struct datastore) + currentPage * PAGESIZE + START_DATA_PAGE;
    error = esp_partition_read(keeperpartition, pos, &datos, sizeof(struct datastore));
    currentFlight = datos.fltnum;
    return &datos;
}
//////////////////
//  Devuelve el registro previo del mismo vuelo  JHR 08 APR 2020
//////////////////
struct datastore *CompetitionSaved::getPrevRecord()
{
    static struct datastore datos;
    esp_err_t error;
    currentRecordInPage--;
    if (currentRecordInPage > RECORDSPERPAGE)
    {
        currentRecordInPage = RECORDSPERPAGE - 1;
        currentPage--;
    }
    int pos = currentRecordInPage * sizeof(struct datastore) + currentPage * PAGESIZE + START_DATA_PAGE;
    error = esp_partition_read(keeperpartition, pos, &datos, sizeof(struct datastore));
    if (error != ESP_OK)
        return NULL;
    //Debug.println("Read Prev FLTnum: "+String(datos.fltnum)+" Recno. "+String(datos.record_num)+" pressure "+ String(datos.pressure) +" tap "+String(datos.tap));
    return &datos;
}

bool CompetitionSaved::format()
{
    Debug.println("Formating");
    esp_err_t error;
    keeperpartition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "keeper");
    if (!keeperpartition)
    {
         Serial.print("Failed format :-( ");
        return false;
    }
    else
    {
        // Serial.print("direccion: ");
        // Serial.println(keeperpartition->address);
        // Serial.print("size: ");
        // Serial.println(keeperpartition->size);
    }

    error = esp_partition_erase_range(keeperpartition, 0, keeperpartition->size);
    if (error)
        return false;
    if (checkEmpyPartition())
        return true;

    return false;
}

/**
 * Return the complete status of the Flash
 * */
struct flash_status *CompetitionSaved::getFlashStatus()
{
    static struct flash_status status;
    status.totalFlashSize = keeperpartition->size;
    status.recordsPerPage = RECORDSPERPAGE;
    status.flightRecordSize = sizeof(struct datastore);
    status.totalFlashForRecords = keeperpartition->size - PAGESIZE * 2;
    status.currentFlight = currentFlight;
    status.numberOfRecordsUsed = currentPage * RECORDSPERPAGE + currentRecordInPage;
    status.maxFlightsAllowed = NUMBER_OF_METADATA_BLOCKS * PAGESIZE / sizeof(struct Contests);
    status.maxRecordsAllowed = status.totalFlashForRecords / status.flightRecordSize;
    status.totalBlocksUsed = currentPage;
    return &status;
}

/** 
 * Start sending binary data in 4k block
 * */

uint8_t *CompetitionSaved::getFirstFlashBlock()
{
    static uint8_t buffer[PAGESIZE];
    esp_err_t error;

    currentBlockSend = 0;
    if (currentPage == 0)
        return NULL;

    error = esp_partition_read(keeperpartition, currentBlockSend * PAGESIZE, &buffer, PAGESIZE);
    if (error != ESP_OK)
        return NULL;
    currentBlockSend++;
    return buffer;
}

/**
 * Send next block of data, return null if none more is available
 * 
 * */

uint8_t *CompetitionSaved::getNextFlashBlock()
{
    static uint8_t buffer[PAGESIZE];
    esp_err_t error;

    if (currentBlockSend > currentPage + NUMBER_OF_PAGE_HEADERS)
        return NULL;

    error = esp_partition_read(keeperpartition, currentBlockSend * PAGESIZE, &buffer, PAGESIZE);
    if (error != ESP_OK)
        return NULL;
    currentBlockSend++;
    return buffer;
}

/*********************
 *  Private functions
 *********************/

// Calculates the position where the flight metadata is stored, here flight metadata is the competition header
long CompetitionSaved::flight2pos(short flight_number)
{
    return (START_METADATA_PAGE + (flight_number - 1) * sizeof(struct Contests));
    ////return (START_METADATA_PAGE + (flight_number )* PAGESIZE); //si quisieramos un bloque por competitcion
}

/**
 * This store only the fligh number on the records keeping block
 * */
bool CompetitionSaved::saveCurrentFlightDirectory()
{
    // int current;
    esp_err_t error;
    //   Serial.printf("\nCurrent pos %d, currentFlight %d\n", currentPos, currentFlight);
    error = esp_partition_write(keeperpartition, currentPos, &currentFlight, sizeof(currentFlight));
    if (error != ESP_OK)
        return false;

    currentPos += sizeof(currentFlight);
    return true;
}

bool CompetitionSaved::checkEmpyPartition()
{
    esp_err_t error;
    uint32_t data = 0xFFFFFFFF;
    // uint32_t size = keeperpartition->size / sizeof(uint16_t); // Partion size in uint16_t words
    uint32_t size = currentPage * PAGESIZE + START_DATA_PAGE; // Check only to the last page used
    Debug.print("Checking " + String(size) + " bytes ...");
    for (uint32_t i = 0; i < size && data == 0xFFFFFFFF; i += sizeof(data))
    {
        error = esp_partition_read(keeperpartition, i, &data, sizeof(data));
        if (error)
            return false;
    }
    Debug.println(" Done empty");
    if (data == 0xFFFFFFFF)
        return true;
    else
        return false;
}

// usado para cambiar el formato de datos
// void CompetitionSaved::move_block(){
//     keeperpartition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "keeper");
//     int32_t buff[1024];
//         esp_partition_read(keeperpartition, 3*PAGESIZE, &buff, PAGESIZE);
//         esp_partition_write(keeperpartition, START_DATA_PAGE, &buff, PAGESIZE);
//         esp_partition_erase_range(keeperpartition, 3*PAGESIZE , PAGESIZE);
//         Serial.print(" block BU MOVED ................:\n");
// }