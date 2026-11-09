#ifndef QueueRegister_h
#define QueueRegister_h

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <SPIFFS.h>
#include "src/Timer_flight_data.h"
#include "DebugPrint.h"


class QueueDatos
{
private:
    QueueHandle_t queue;

public:
    void begin();
    BaseType_t add(struct  datastore data);
    struct datastore* get();
    int flush();
};

extern QueueDatos queueDatos;
extern QueueDatos queueEvents;
#endif
