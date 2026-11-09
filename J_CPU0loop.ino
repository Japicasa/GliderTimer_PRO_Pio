/**
 * 
 * Este bucle trata todas la interacciones no criticas en la CPU0
 * 
**/

// // #include "DebugPrint.h"
// // #include "QueueRegister.h"

// void loopCPU0(void *parameter)
// {
//   // static unsigned long tak = 0;
//   // static unsigned long t = 0; // para timer casposo ahora da el tiempo desde setup
//   // static byte last_FLT_mode = CONFIG;
//   // int nloops;

//   // while (true)
//   // {
//   // //   if (mp3->isRunning()) {
//   // //   if (!mp3->loop()) mp3->stop();
//   // // } else {
//   // //   Serial.printf("MP3 done\n");
//   // //   delay(1000);
//   // // }

//   //   //   check_queues(); // Comprobamos las colas y grabamos los eventos en SPIFF
//   //   if (last_FLT_mode != FLT_mode)
//   //   {
//   //     Debug.println("Nuevo modo: " + String(FLT_mode));
//   //     last_FLT_mode = FLT_mode;
//   //     if (FLT_mode==NEW_DATA)
//   //     {
//   //       partial_FLT_save();
//   //       Debug.println("========================================PARTIAL FLIGHT SAVED " );
//   //       }
//   //   }

//   //   if (screwF5JFlight)
//   //   {
//   //     F5J_height = 0;
//   //     F5J_millis = 0;
//   //     throttle_shut_millis = 0;
//   //     partial_FLT_save();
//   //     screwF5JFlight = 0;
//   //   }
//   //   check_queues();
//   //   delay(1);    
//   // }
// }

// void check_queues()
// {
//   // struct datastore *datos = NULL;
//   // static datastore quedata[20];
//   // static short save_pointer = 0;
//   // static bool primeravez = true;
//   // bool lectura_buena = 0;

//   // datos = queueDatos.get();
//   // while (datos != NULL)
//   // {

//   //   //quedata[save_pointer++] = datos;
//   //   memcpy(&quedata[save_pointer++], datos, sizeof(quedata[0])); ///solo se guarda un registro cada vez
//   //   datos = queueDatos.get();
//   //   lectura_buena = 1;
//   //   Debug.println(String(save_pointer));
//   // }

//   // if (save_pointer > 0)
//   // {
//   //   if (primeravez)
//   //   {
//   //     FLTnum++;
//   //    // competition.save_competition(FLTnum); // Guardamos el comienzo del vuelo
//   //     primeravez = false;
//   //   }

//   //   Debug.println("round_no=  " + String(quedata[0].round_no) + " landing_timedout: " + String(quedata[0].timedout));

//   //   // Salvamos en Flash
//   //   competition.save_record(&quedata[0]);

//   //   save_pointer = 0;
//   // }
//   // ////////////////////////////////////////////
//   // /**
//   //  * Analizamos la cola de eventos para grabarlos a disco
//   //  * */
//   // datos = queueEvents.get();
//   // while (datos != NULL)
//   // {
//   //   save_event(datos);
//   //   datos = queueDatos.get();
//   // }
// }

// // #define MAX_EVENTS_ARRAY 100
// // static datastore eventos[MAX_EVENTS_ARRAY];
// // static uint8_t eventsHead = 0;
// // static uint8_t eventsTail = 0;
// // static boolean eventsFull = false;

// void save_event(struct datastore *datos)
// {

//   // memcpy(&eventos[eventsHead], datos, sizeof(struct datastore));

//   // if (eventsFull)
//   // {
//   //   eventsTail++;
//   //   if (eventsTail == MAX_EVENTS_ARRAY)
//   //   {
//   //     eventsTail = 0;
//   //   }
//   // }

//   // eventsHead++;
//   // if (eventsHead == MAX_EVENTS_ARRAY)
//   // {
//   //   eventsHead = 0;
//   // }
//   // eventsFull = (eventsHead == eventsTail);

//   // Debug.println("XXX EVENTO : millis=" + String(millis()) +
//   //               " gzsm=" + String(datos->gzsm) +
//   //               //" x=" + String(datos->x) +
//   //               " z=" + String(datos->z) +
//   //               " tap=" + String(datos->tap) +
//   //               " altitude=" + String(datos->altitude) +
//   //               " pressure= " + String(datos->pressure));
// }

// struct datastore *read_event()
// {
//   // struct datastore *data;

//   // if (!eventsFull && (eventsHead == eventsTail))
//   // { // El buffer esta vacio
//   //   return NULL;
//   // }
//   // data = &eventos[eventsTail];
//   // eventsFull = false;
//   // eventsTail = (eventsTail + 1) % MAX_EVENTS_ARRAY;
//   // return data;
// }

// /********************************
//  * Una vez parada la grabacion
//  * mira en los eventos el tiempo de contacto
//  * y si no lo encuentra asigna el tiempo de reposo
//  *******************************/
// void analize_event()
// {

//   // TODO
//   // struct datastore *myevent;
//   // myevent = read_event();
//   // while (myevent != NULL)
//   // {
//   //   int deltapress = abs(landing_pressure - myevent->pressure);
//   //   Debug.print(".leido. ");
//   //   Debug.print(String(myevent->millis));
//   //   Debug.print("dP: ");
//   //   Debug.println(String(deltapress));
//   //   if (landing_millis == 0)
//   //   {
//   //     Debug.println("dP:A");

//   //     Debug.println("dP:B");
//   //     if ((rest_millis - myevent->millis) < 25000)
//   //     {
//   //       Debug.println("dP:C");
//   //       if (deltapress < landing_pressure_thr)
//   //       {
//   //         Debug.println("dP:DDDDDDDDDDDDDDDDDDDDDD!!!");
//   //         landing_millis = myevent->millis;
//   //       }
//   //     }
//   //     // }
//   //   }
//   //   myevent = read_event();
//   // }

//   // if (landing_millis == 0)
//   // {
//   //   //////////////////////////// Aqui empieza el analizador fino de aterrizaje
//   //   landing_millis=Lander.analise_FLT_recs();
//   //   ///////////////////////////landing_millis = rest_millis - 3000; ///esto es lo de antes
//   //   Debug.print("0000000000000000000000000");
//   // }
//   // if (landing_millis > take_off_millis)
//   // {
//   //   flight_time = (landing_millis - take_off_millis) / 1000.0;
//   //   flight_mins = int(flight_time) / 60;
//   //   flight_secs = int(flight_time) % 60;
//   // }
// }

// /******************************
//  * Calcula el momento de contacto TODO Donde ponerlo?
//  * y la duracion del vuelo
//  * guarda la configuración
//  ******************************/
// void eval_landingTime()
// {
// //   analize_event();
// // //////////////////////////////////////////////  WARNING
// //   //FLTnum++;  ///
// //   #ifndef FLT_target
// //    last_FLT_save();
// //   #endif
// //   // savenewpoint();
// //   //displayameasurementscreen();
// //   //connecttowifi(); // TODO Ni WiFi
// //   Debug.print("EVALUATED");
// }



// void print_measured_data(struct datastore *datos)
// {
//   // static unsigned long last_millis = 10010;
//   // if ((millis() - last_millis) > 2000)
//   // {
//   //   Debug.println("Grabado: millis=" + String(millis()) +
//   //                 " gzsm=" + String(datos->gzsm) +
//   //                 //" x=" + String(datos->x) +
//   //                 " z=" + String(datos->z) +
//   //                 " tap=" + String(datos->tap) +
//   //                 " altitude=" + String(datos->altitude) +
//   //                 " pressure= " + String(datos->pressure));
//   //   last_millis = millis();
//   // }
// }

// /****************************
// * Guarda los resultados y la configuracion en Pagina de directorio Generado automaticamente
// ****************************/
// void last_FLT_save()
// {
//   // struct Contests fltdata;

//   // fltdata.FLTnum = FLTnum;
//   // fltdata.flight_mins = flight_mins;
//   // fltdata.flight_secs = flight_secs;
//   // fltdata.F5J_height = (uint32_t)round(F5J_height * 10);
//   // fltdata.F5J_millis = F5J_millis;
//   // fltdata.landing_millis = landing_millis;
//   // fltdata.throttle_shut_millis = throttle_shut_millis;
//   // fltdata.take_off_millis = take_off_millis;
//   // fltdata.landing_pressure = landing_pressure;
//   // fltdata.takeoff_localsecs = takeoff_localsecs;
//   // fltdata.rest_millis = rest_millis;
//   // fltdata.takeoff_localmilisecs = takeoff_localmilisecs;

//   // competition.endFlight(&fltdata);
//   /// FLTnum++  ///; duplicado
// }

// void partial_FLT_save()
// {
//   // struct Constests fltdata;

//   // fltdata.contestNum = competition.contestNum;
  
//   // fltdata.deleted = 0;

//   //competition.savePartialFlight(&fltdata); //TODO
// }