/**
 * Gestion de colas FreRtos para la cominicaicon entre los bucles
 * **/

#include "QueueRegister.h"
#include "DebugPrint.h"
#include "keeper.h"

QueueDatos queueDatos;  // Cola para datos
QueueDatos queueEvents; // Cola para enventos reseñables
extern DebugPrint Debug;
/**
 * Métodos para la gestion de la cola de datos a recolectar
 **/

void QueueDatos::begin()
{
    queue = xQueueCreate(200, sizeof(datastore));

    if (queue == NULL)
    {
        // No he podido crear la cola y para
        Debug.println("Error creating the queue");
        while (true)
            ; // Hasta el fin de los días
    }
}

BaseType_t QueueDatos::add(struct datastore data)
{
    if (xQueueSend(queue, &data, 0) == errQUEUE_FULL) // Devuelve errQUEUE_FULL en caso de que la cola este llena
    {
        Debug.print("Cola llena");
        return errQUEUE_FULL;
    }
    return pdTRUE;
}

struct datastore *QueueDatos::get()
{
    static struct datastore data;

    struct datastore *resultado = NULL;
    //Debug.println(String(millis()));

    if (xQueueReceive(queue, &(data), (TickType_t)5) == pdTRUE) // Espero un poco a ver si hy algo en la cola
    {
        // Correcto
        resultado = &data;
    }
// Debug.println(String(millis()));
    return resultado;
}
int QueueDatos::flush()
{
//     static struct datastore data;
//     static struct datastore quedata[2];
//     int save_pointer = 0;
//     bool lectura_buena=0;
//     // struct datastore *resultado = NULL;
//     // chupamos los datos de la cola
//    if (xQueueReceive(queue, &(data) , ( TickType_t )10) == pdTRUE) // Espero un poco a ver si hy algo en la cola
//     {
//         // Correcto
//         quedata[save_pointer] = data;
//         // mencpy(&data, &quedata[save_pointer], sizeof(data));
//        // print_measured_data(quedata);
//         lectura_buena=1;
//         save_pointer++;
//     }
//     // cuando llego aqui la cola esta vacia-> grabo a disco los datos
//     if (lectura_buena)
//     {
//        // print_measured_data(quedata);
//         // File file = SPIFFS.open("/structsaved.bin", "a");
//         // file.write((const uint8_t *)&data[save_pointer], save_pointer * sizeof(data[0]));
//         // file.close();
//     }
//     return save_pointer;
return 0;
}