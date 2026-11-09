
/* Generated from GenMyModel */

#include "FlightManager.h"
#include <Arduino.h>
#include <stdint.h>

#include <sys/time.h>
#include <M5Stack.h>

extern GTimer GT;
extern GTdisplay GD;
extern int page_to_show;
extern int FLT_mode;
extern boolean sinWifi;
extern CompetitionSaved competition;
extern Timer_flight_data FLTdata;
extern String serialnumber;
extern String versionkeeper;
extern boolean busy_mp3;
extern boolean check_for_mp3;
extern uint32_t millis_from_cmd;
extern int gpio_in_pin;
extern RTC local_RTC;
extern char localssid[12];
extern uint32_t GLOBAL_DURATION;

void Flight_Manager::init()
{
    duration = 120;
    if (GLOBAL_DURATION > 0)
    {
        duration = GLOBAL_DURATION;
    }
    pre_notice_minutes = 1;
    in_flight_status = 0;
    pre_notice_status = 0;
    Serial.println("READING SETTINGS");
    read_settings();
    // update_volume(volume);
    // update_brightness(brightness);
    // Serial.print("################SIZE millis #############: "); Serial.println(sizeof(millis()));
    // if ((panel_type==SERIAL_IF)&&(panel_type==SERIAL_IF_EXTENDED)){
    Serial1.begin(baud_rate1, SERIAL_8N1, RXD1, TXD1);
    //}
}
void Flight_Manager::start(uint32_t advance_minutes)
{
    // Serial.println("Flight START COMMAND RECEIVED-------------------->");
    GD.primeravez = 1;
    pre_notice_minutes = advance_minutes;
    uint32_t advance_seconds = 60 * advance_minutes + 15;
    // advance_seconds = 43; ////TODO  PARA PRUEBAS RAPIDAS quitar lo

    if (advance_minutes > 0) /// manual start command from buttons
    {
        GT.begin();
        Serial.println("Flight START COMMAND RECEIVED: MAIN-------------->");
        duration = GT.mycompetition.head.flt_duration;
        if (GLOBAL_DURATION > 0)
        {
            duration = GLOBAL_DURATION;
        }
        // duration=60;
        Serial.print("duration: ");
        Serial.println(duration);
        gettimeofday(&timesent, NULL);
        millis_sent = millis() + 1000 - timesent.tv_usec / 1000; // Aqui se quita los usec para las dos  millis
        local_landing_timeout_sec = timesent.tv_sec + duration + 1 + advance_seconds;
        local_launch_sec = timesent.tv_sec + 1 + advance_seconds;
        valid_local_launch_sec = local_launch_sec;
        valid_local_landing_timeout_sec = local_landing_timeout_sec;
        // Serial.printf("sent %d timeout_sec %d\n", millis_sent, local_landing_timeout_sec);
        millis_landing = millis_sent + duration * 1000 + advance_seconds * 1000;
        millis_launch = millis_sent + advance_seconds * 1000;
        // Serial.printf("launch %d landing %d\n", millis_launch, millis_landing);

        schedule_competition(local_launch_sec);

        //////////////
        ////////////// Guardamos la salida del vuelo como piloto 0 de esa ronda
        //////////////
        GT.stored_group_no++;
        if (GT.stored_group_no < 1)
        {
            GT.stored_group_no = 1;
        }
        if (GT.stored_group_no > GT.mycompetition.head.number_of_groups)
        {
            GT.stored_group_no = 1;
            GT.stored_round_no++;
        }
        if (GT.stored_round_no < 1)
        {
            GT.stored_round_no = 1;
        }
        GT.current_round_no = GT.stored_round_no;
        GT.current_group_no = GT.stored_group_no;
        next_tfd.round_no = GT.stored_round_no;
        next_tfd.group_no = GT.stored_group_no;
        if (GT.stored_round_no > GT.mycompetition.head.number_of_rounds)
        {
            // Serial.print("Competition closed");
            boop();
            GT.stored_round_no = GT.stored_round_no - 1;
            GT.current_round_no = GT.stored_round_no;
            GT.current_group_no = GT.mycompetition.head.number_of_groups;
            page_to_show = ERROR_PAGE;
            FLT_mode = PATATA;
            GD.displayUpdateError("Comp. Closed");
            delay(100);
            return;
        }
        if ((GT.scheduled == 1) && (GT.collector == 1))
        {
            // Serial.print("Competition closed");
            boop();
            GT.stored_round_no = GT.stored_round_no - 1;
            GT.current_round_no = GT.stored_round_no;
            GT.current_group_no = GT.mycompetition.head.number_of_groups;
            page_to_show = ERROR_PAGE;
            FLT_mode = PATATA;
            GD.displayUpdateError("Scheduled ON");
            delay(100);
            return;
        }
        FLTdata.begin();
        Serial.print("===== create tfd.===");
        ///////////////////////////////////
        // As a round is started the epoch of launch and landing
        // is stored as the first record of this round corresponding to tfd.pilot_no = 0;
        // pilot=0 record is retrieved in case of a GliderTimer reset
        ///////////////////////////////////
        // FLTdata.tfd.comp_no = GT.mycompetition.head.contestNum;
        // FLTdata.tfd.record_num = GT.stored_record_num;
        // FLTdata.tfd.round_no = GT.stored_round_no;
        // FLTdata.tfd.group_no = GT.stored_group_no;

        Serial.printf("ROUND INCREASED TO %d GROUP INCREASED TO: %d\n", GT.stored_round_no, GT.stored_group_no);
        // FLTdata.tfd.pilot_no = 0;
        // FLTdata.tfd.fltnum = 999;
        FLTdata.tfd.F5J_millis = duration; /// transmit duration in F5J:millis if pilot_no =0;
        FLTdata.tfd.takeoff_local_secs = local_launch_sec;
        FLTdata.tfd.local_landing_timeout_sec = local_landing_timeout_sec;
        serialnumber.toCharArray(FLTdata.tfd.serialnumber, 12);
        versionkeeper.toCharArray(FLTdata.tfd.Firm_ver, 12);
    }
    else
    {                               /// see Timer_flight_data::last_launch_payload() to understand the variable meaning
        if (next_launch_epoch == 0) /// manual start command from pick data from another Gtimer
        {
            Serial.println("starting flight as collector");
            duration = FLTdata.tfd.F5J_millis;
             if (GLOBAL_DURATION > 0)
    {
        duration = GLOBAL_DURATION;
    }
            local_launch_sec = FLTdata.tfd.takeoff_local_secs;
            local_landing_timeout_sec = local_launch_sec + duration;
            valid_local_launch_sec = local_launch_sec;
            valid_local_landing_timeout_sec = local_landing_timeout_sec;
            gettimeofday(&timesent, NULL);
            millis_launch = millis() + (local_launch_sec - timesent.tv_sec) * 1000 - timesent.tv_usec / 1000;
            millis_landing = millis_launch + duration * 1000;
            GT.stored_round_no = FLTdata.tfd.round_no;
            GT.stored_group_no = FLTdata.tfd.group_no;
            GT.current_round_no = GT.stored_round_no;
            GT.current_group_no = GT.stored_group_no;
        }
        else /// automatic scheduled start command from file scheduled_rounds.csv
        {
            Serial.println("starting flight as scheduled collector");
            duration = next_tfd.F5J_millis;
             if (GLOBAL_DURATION > 0)
    {
        duration = GLOBAL_DURATION;
    }
            local_launch_sec = next_tfd.takeoff_local_secs;
            local_landing_timeout_sec = local_launch_sec + duration;
            gettimeofday(&timesent, NULL);
            millis_launch = millis() + (local_launch_sec - timesent.tv_sec) * 1000 - timesent.tv_usec / 1000;
            millis_landing = millis_launch + duration * 1000;
            // GT.stored_round_no = next_tfd.round_no;
            // GT.stored_group_no = next_tfd.group_no;
            GT.current_round_no = next_tfd.round_no;
            GT.current_group_no = next_tfd.group_no;
        }
    }

    Serial.print("=====complete tfd.===");

    /// competition.save_record(&FLTdata.tfd);    ///cambiado a "manage flight" para permitir "abort"
    //////////////
    background_music_on = 1;

    for (int i = 0; i < MAX_BEEP_POINTS; i++) // Fill the alarm table with timevals corresponding to this launch
    {
        if (alarm[i].event == 0)
        {
            table_index = i - 1; // table_index will be the pointer to the next event in time
            // Serial.print("++++++++++++table index");
            // Serial.println(table_index);
            break;
        }
        if (alarm[i].event == LANDING)
        {
            alarm[i].millis_for_beep = millis_landing + alarm[i].seconds_to_event * 1000;
            if (alarm[i].beep_type == PLAY)
            {
                alarm[i].local_secs_for_beep.tv_sec = local_landing_timeout_sec + alarm[i].seconds_to_event - 1;
                alarm[i].local_secs_for_beep.tv_usec = 1000000 - MP3_LAG;
            }
            else
            {
                alarm[i].local_secs_for_beep.tv_sec = local_landing_timeout_sec + alarm[i].seconds_to_event;
            }
            // Serial.printf("event %d millis %d\n", i, alarm[i].millis_for_beep);
        }
        else
        {
            alarm[i].millis_for_beep = millis_launch + alarm[i].seconds_to_event * 1000;
            if (alarm[i].beep_type == PLAY)
            {
                alarm[i].local_secs_for_beep.tv_sec = local_launch_sec + alarm[i].seconds_to_event - 1;
                alarm[i].local_secs_for_beep.tv_usec = 1000000 - MP3_LAG;
            }
            else
            {
                alarm[i].local_secs_for_beep.tv_sec = local_launch_sec + alarm[i].seconds_to_event;
            }

            // Serial.printf("event %d millis %d\n", i, alarm[i].millis_for_beep);
        }
    }
    in_flight_status = 1;
    Serial.println("flight.start command released");
}
void Flight_Manager::manage_flight()
{
    if (sinWifi)
    {
        Serial.println("rEturned");
        return;
    }
    if (start_betty_deamon)
    {
        perform_betty_deamon();
        // Serial.printf("performing betty step");
    }

    if (in_flight_status)
    {
        gettimeofday(&timesent, NULL);
        struct tm timeinfo;
        getLocalTime(&timeinfo);
        secs_to_launch = local_launch_sec - timesent.tv_sec;
        secs_to_land = local_landing_timeout_sec - timesent.tv_sec;
        int rest_to_land = secs_to_land % 60;
        fraction_in_second = timesent.tv_usec / SECOND_DIVIDER;
        int myminute = 0;
        if (secs_to_launch > 0)
        {
            pre_notice_status = 1;
        }
        // if (millis() > alarm[table_index].millis_for_beep)
        // {
        //     //Serial.print(millis()); Serial.print("  CAMPANITA ? ");Serial.println(alarm[table_index].millis_for_beep);
        // }
        /// EVENT AREA (from table)
        // struct timeval timediff;
        // timersub(&timesent, &alarm[table_index].local_secs_for_beep, &timediff);
        // Serial.printf("; %d ;\n",timediff.tv_usec);
        if (timercmp(&timesent, &alarm[table_index].local_secs_for_beep, >))
        {
            // Serial.printf("diferencia %d :\n",timediff.tv_usec);
            switch (alarm[table_index].beep_type)
            {
            case BIP_:
                bip();
                break;
            case BEEP:
                beep();
                break;
            case BOOP:
                Serial.printf("BOOP milis start : %d\n", millis());
                boop();
                break;
            case BOP_:
                bip();
                break;
            case BPBP:
                bpbp();
                break;
            case PLAY:
                Serial.printf("PLAY milis start : %d  table index %d \n", millis(), table_index);
                // Serial.printf("PLAY milis start : %dfile_number: %d\n", millis(), alarm[table_index].file_number);
                millis_from_cmd = millis();
                MP3.ad_file(alarm[table_index].file_number);
                // Serial.printf("milis start : %dfile_number: %d\n", millis(), alarm[table_index].file_number);

                check_for_mp3 = 1;
                // while (!digitalRead(gpio_in_pin))
                // {
                //     //Serial.print("x");
                // }
                break;
            case STOP:

                MP3.stop();
                Serial.printf("PLAY STOP start : %dfile_number: %d\n", millis(), alarm[table_index].file_number);

                break;
            case _ADV:

                MP3.play_nothing();
                Serial.printf("PLAY NOTHING start : %dfile_number: %d\n", millis(), alarm[table_index].file_number);

                break;
            }

            table_index = table_index - 1;

            // if (table_index < 0)
            // {
            //     in_flight_status = 0;
            //     GT.menupointer = 1;
            //     GT.menulevel = 1;
            //     GD.menu_displayUpdate(GT.menupointer);
            // }
            Serial.printf(" ALARM at min:%d sec: %d micros: %d\n", timeinfo.tm_min, timeinfo.tm_sec, timesent.tv_usec);
            Serial.print("ON Alarm end ");
            local_RTC.printLocalTimeMicros();
        }

        // MIN oclock
        if (((secs_to_land) % 60) == 1)
        {
            if ((secs_to_land != 1) && (secs_to_launch != 1))
            {
                if (secs_to_launch > 0)
                {
                    myminute = secs_to_launch / 60;
                    if (myminute == 1)
                    {
                        myminute = 201;
                        background_music_on = 0;
                    }
                    else
                    {
                        myminute = myminute + 100;
                    }
                }
                else
                {
                    myminute = secs_to_land / 60 + 100;
                }
                if (!min_oclock_play_done)
                {
                    min_oclock_play_done = 1;
                    if (secs_to_launch < 0) // we only play here after launch, no fade
                    {
                        MP3.ad_file(myminute);
                    }
                }
            }
        }
        else
        {
            min_oclock_play_done = 0;
        }
        if ((secs_to_land % 60) == 0) // min oclock beep
        {
            if ((secs_to_land != 0) && (secs_to_launch != 0))
            {
                if (!min_oclock_beep_done)
                {
                    beep();
                    min_oclock_beep_done = 1;
                }
            }
        }
        else
        {
            min_oclock_beep_done = 0;
        }

        ////FADE AREA before launch
        if (secs_to_launch > 59)
        {
            switch (rest_to_land)
            {
            case 3:
                if (last_fraction_in_second != fraction_in_second)
                {
                    MP3.volume_decr();
                    last_fraction_in_second = fraction_in_second;
                    // Serial.println(" VOL3 DECREASE");
                }
                break;
            case 2:
                if (last_fraction_in_second != fraction_in_second)
                {
                    switch (fraction_in_second)
                    {
                    case 0:
                        MP3.volume_decr();
                        // Serial.println(" VOL2 DECREASE");
                        break;

                    case 1:
                        // MP3.volume(0);
                        if (secs_to_launch == 62)
                        {
                            MP3.stop();
                            // Serial.println("VOL Stop ");
                        }
                        else
                        {
                            MP3.volume(0);
                            // Serial.println("VOL  ZERO");
                        }
                        break;
                    case 2:
                        if (secs_to_launch == 62)
                        {
                            MP3.play_nothing();
                            // Serial.println("PLAY NOTHING");
                        }
                        break;

                    default:
                        break;
                    }
                    last_fraction_in_second = fraction_in_second;
                }

                break;

            case 1:
                if (last_fraction_in_second != fraction_in_second)
                {
                    if (fraction_in_second == 0)
                    {
                        MP3.ad_file(myminute);
                        Serial.println("Myminute");
                        last_fraction_in_second = fraction_in_second;
                    }
                    if (fraction_in_second == 1)
                    {
                        MP3.volume(MP3.adv_vol);
                        // Serial.println("AD VOLUME");
                        last_fraction_in_second = fraction_in_second;
                    }
                }
                break;
            case 0:
                if (last_fraction_in_second != fraction_in_second)
                {

                    // if (fraction_in_second == 3)
                    // {
                    //     MP3.volume(0);
                    //     // Serial.println("VOL ZERO");
                    //     last_fraction_in_second = fraction_in_second;
                    // }
                }
                break;

            case 59:

                if (last_fraction_in_second != fraction_in_second)
                {
                    if (fraction_in_second == 0)
                    {
                        MP3.volume(0);
                        // Serial.println("VOL ZERO");
                        last_fraction_in_second = fraction_in_second;
                    }
                    if (fraction_in_second == 1)
                    {
                        MP3.volume(0);
                        // Serial.println("VOL ZERO");   n
                        last_fraction_in_second = fraction_in_second;
                    }
                    if (fraction_in_second == 2)
                    {
                        MP3.volume(0);
                        // Serial.println("VOL ZERO");
                        last_fraction_in_second = fraction_in_second;
                    }
                }
                break;

            case 58:
                if (last_fraction_in_second != fraction_in_second)
                {
                    MP3.volume_incr();
                    // Serial.println("INCR VOLUME");
                    last_fraction_in_second = fraction_in_second;
                }
                break;

            default:
                break;
            }
        }

        if (pre_notice_status)
        {
            if (millis() > millis_launch)
            {
                record_launch_data();
                pre_notice_status = 0;
            }
        }

        // Serial.printf("secs to land: %d  rest_to-launch:  %d fraction_in_second : %d\n",secs_to_land,rest_to_land,fraction_in_second);
        if ((secs_to_land > 62) && (rest_to_land == 23) && (fraction_in_second == 3)) // Synchro with RTC in sec -23
        {
            struct timeval now;
            gettimeofday(&now, NULL);
            local_RTC.ConfigLocalTimeFromRTC();
            Serial.printf("Syncing millis() %d \n", millis());
        }
        manage_panel_in_flight();
        if (FLT_mode == PATATA)
        {
            GD.manage_timer_display();
            GT.clear_buttons();
        }
        if (secs_to_land < -60)
        {
            in_flight_status = 0;
            standby_recipe_counter = 0;
            GT.back_to_prev_status();
            Serial.println("Flight status FINISHED");
        }
    }
    else
    {
        static uint millis_to_clock = 0;
        if ((millis() - millis_to_clock) > 10000)
        {
            millis_to_clock = millis();
            if (panel_type == EXTENDED_SERIAL_IF)
            {
                Serial1.print("R00G00T0000ST\r");
            }
        }
        int file_to_play_req=0; ////para definir fuera del case
        if ((millis() - last_step_millis) > standby_recipe_step_period)
        {
            last_step_millis = millis(); //// Sound Music in not a flight //negative steps only for initalisation
            // Serial.printf("standby_recipe_counter: %d \n", standby_recipe_counter);
            switch (standby_recipe_counter)
            {
            case -3:
                delay(10);
                MP3.volume(0);
                standby_recipe_counter++;
                //.println("por aqui pase 1-3");
                break;
            case -2:
                delay(10);
                MP3.volume(0);
                standby_recipe_counter++;
                // Serial.println("por aqui pase 1-2");
                break;

                // Serial.println("VOL Stop ");
            case -1:
                delay(10);
                MP3.volume(0);
                standby_recipe_counter++;
                //.println("por aqui pase 1-1");
                break;

                // Serial.println("VOL Stop ");
            case 0:
                // MP3.stop();
                // Serial.println("STOP");
                standby_recipe_counter++;
                Serial.println("por aqui pase 1 0");
                break;
            case 1:
                MP3.volume(0);
                Serial.println("Volume 0");
                standby_recipe_counter++;
                // Serial.println("por aqui pase 1 1");
                break;
            case 2:

                Serial.println("MP3.play_something()");
                /// Para poner mas files

                file_to_play_req  = (GT.current_round_no-1 )* GT.mycompetition.head.number_of_groups+ (GT.current_group_no);
                Serial.printf("file_to_play_req: %d GT.current_round_no %d GT.mycompetition.head.number_of_groups %d GT.current_group_no  %d\n", file_to_play_req, GT.current_round_no, GT.mycompetition.head.number_of_groups, GT.current_group_no);
                MP3.play_something(file_to_play_req);
                standby_recipe_counter++;
                delay(100);
                // Serial.println("por aqui pase 1 2");
                break;
            case 3:
                MP3.actual_vol++;

                // Serial.println("MP3.volume(MP3.actual_vol)");
                MP3.volume(MP3.actual_vol);
                // Serial.println("por aqui pase 1 3");
                if (MP3.actual_vol == MP3.music_vol)
                    standby_recipe_counter++;
                break;
            case 4:
                if (GT.scheduled)
                {
                    next_launch_epoch = look_for_new_epoch();
                    // struct timeval now;
                    // gettimeofday(&now, NULL);
                    // next_launch_epoch = now.tv_sec + 320;
                    Serial.printf("4.- NEW EPOCH FOUND: %d \n", next_launch_epoch);
                }
                standby_recipe_counter++;
                // Serial.println("por aqui pase 1 4");

                break;

            case 5:
                if (GT.scheduled)
                {
                    bool shall_we_start = check_for_scheduled_start();
                    // Serial.println("por aqui pase 1 5");
                    if (shall_we_start)
                    {
                        // Serial.println("===============por aqui pase 2");
                        standby_recipe_counter++;
                        // Serial.println("por aqui pase 1 51");
                    }
                }
                break;
            case 6:
                if (GT.scheduled)
                {
                    boop();
                }
                standby_recipe_counter++;
                // Serial.println("por aqui pase 1 6");
                break;
            case 7:
                if (GT.scheduled)
                {
                    boop();
                }
                standby_recipe_counter++;
                // Serial.println("por aqui pase 1 7");
                break;
            case 8:
                if (GT.scheduled)
                {
                    boop();
                    Serial.printf("8.-Final Boop: %d \n", next_launch_epoch);

                    // start(0); /// se lanza el vuelo automatico

                    // next_launch_epoch = 0;
                    start_betty_deamon = 1;
                }
                standby_recipe_counter++;
                // Serial.println("por aqui pase 1 8");
                break;
            case 9:
                if (GT.scheduled)
                {
                    if (start_betty_deamon)
                    {
                        // perform_betty_deamon();
                        // Serial.printf("performing betty step");
                    }
                    else
                    {
                        Serial.printf("9.- EPOCH FOUND automatic start: %d \n", next_launch_epoch);
                        start(0); /// se lanza el vuelo automatico
                        standby_recipe_counter++;
                        next_launch_epoch = 0;
                    }
                }
                else
                {
                    standby_recipe_counter++;
                    // Serial.println("por aqui pase 1 9");
                }
                break;

                break;
            default:
                break;
            }
        }
        // manage_panel_in_flight();
    }
}

void Flight_Manager::manage_panel_in_flight() // When in flight it is updated BEFORE M5 Screen (due some lag)
                                              // r&Group characters
                                              // all extended
{
    int min = secs_to_land / 60;
    int secs = secs_to_land % 60;
    char label[16];
    if (in_flight_status)
    {
        min = secs_to_land / 60;
        secs = secs_to_land % 60;
        if (pre_notice_status)
        {
            timer_state = TIMER_STATE_PT;
            min = secs_to_launch / 60;
            secs = secs_to_launch % 60;
        }
        else
        {
            if (secs_to_land < 0)
            {
                timer_state = TIMER_STATE_LT;
                min = 0;
                secs = -secs;
            }
            else
            {
                timer_state = TIMER_STATE_WT;
            }
        }
    }
    else
    {
        timer_state = TIMER_STATE_ST;
        min = 99;
        secs = 99;
    }

    switch (panel_type)
    {
    case I2C_IF:
        // if (secs_to_launch < 0) // TODO it will only show panel in case we are in working time
        // {
        //     if (last_panel_secs != secs_to_land)
        //     {
        //         last_panel_secs = secs_to_land;
        //         if (secs_to_land > 0)
        //         {
        //             displaycounter(secs_to_land, displayon, showcounter);
        //             Serial.printf(" secs t : %d\n", secs_to_land);
        //         }
        //         else
        //         {
        //             displaycounter(-secs_to_land, displayon, showcounter);
        //         }
        //     }
        // }
        // break;
        switch (timer_state)
        {
        case TIMER_STATE_ST:
            displayon = 0;
            if (last_panel_secs != secs_to_land)
            {
                last_panel_secs = secs_to_land;
                displaycounter(secs_to_land, displayon, showcounter);
                Serial.printf(" secs t : %d\n", secs_to_land);
            }
            break;
        case TIMER_STATE_PT:
            displayon = 1;
            if (last_panel_secs != secs_to_land)
            {
                if (secs_to_launch < 60)
                {
                    if (secs_to_launch < 11)
                    {
                        displayon = 1;
                        displaycounter(secs_to_launch, displayon, showcounter);
                        Serial.printf(" secs t : %d\n", secs_to_launch);
                        last_panel_secs = secs_to_land;
                    }
                    else
                    {
                        displayon = (secs_to_launch + 1) % 2;
                        last_panel_secs = secs_to_land;
                        displaycounter(GT.mycompetition.head.flt_duration, displayon, showcounter);
                        Serial.printf(" secs t : %d\n", GT.mycompetition.head.flt_duration);
                    }
                }
                else
                {
                    last_panel_secs = secs_to_land;
                    display_round_group(GT.stored_round_no, GT.stored_group_no, displayon, showcounter);
                    // displaycounter(secs_to_land, displayon, showcounter);
                    Serial.printf(" secs t : %d\n", GT.stored_round_no * 100 + GT.stored_group_no);
                }
            }
            break;
        case TIMER_STATE_WT:
            displayon = 1;
            if (last_panel_secs != secs_to_land)
            {
                last_panel_secs = secs_to_land;
                displaycounter(secs_to_land, displayon, showcounter);
                Serial.printf(" secs t : %d\n", secs_to_land);
            }
            break;
        case TIMER_STATE_LT:
            displayon = 1;
            if (last_panel_secs != secs_to_land)
            {
                last_panel_secs = secs_to_land;
                displaycounter(-secs_to_land, displayon, showcounter);
                Serial.printf(" secs t : %d\n", secs_to_land);
            }
            break;
        }
        break;

    case STANDARD_SERIAL_IF:
        snprintf(label, 16, "%02d%02d\r", min, secs);
        if (strcmp(label, last_label) != 0)
        {
            Serial.printf("Sent to panel:  %s \n", label);
            Serial1.printf("A%02d%02d\r", min, secs);
            strcpy(last_label, label);
        }
        break;

        // Standard “Embedded-Ability” format: “Ammss+CR”
        // ASCII character string “Ammss \r” (the letter A, the minutes digits, the seconds digits, the
        // ASCII character for Carriage Return).
        // Extended Protocol format: “R99G99TmmssAA+CR”
        // R99 - 99 is the round number.
        // G99 - 99 is the group number.
        // Tmmss, where “mm” is 2 digits for minutes and “ss” is 2 digits for seconds.
        // AA - two character code for the timer state: PT prep time, WT working time, LT landing time,
        // ST sleep time, DT display time-of-day.
        // CR - the ASCII character for Carriage Return

    case EXTENDED_SERIAL_IF:

        switch (timer_state)
        {
        case TIMER_STATE_ST:
            snprintf(label, 16, "R%02dG%02dT%02d%02dST\r", next_tfd.round_no, next_tfd.group_no, min, secs);
            // snprintf(label, 16, "R%02dG%02dT%02d%02dST\r", GT.stored_round_no, GT.stored_group_no, min, secs);
            break;
        case TIMER_STATE_PT:
            snprintf(label, 16, "R%02dG%02dT%02d%02dPT\r", next_tfd.round_no, next_tfd.group_no, min, secs);
            // snprintf(label, 16, "R%02dG%02dT%02d%02dPT\r", GT.stored_round_no, GT.stored_group_no, min, secs);
            break;
        case TIMER_STATE_WT:
            snprintf(label, 16, "R%02dG%02dT%02d%02dWT\r", next_tfd.round_no, next_tfd.group_no, min, secs);
            // snprintf(label, 16, "R%02dG%02dT%02d%02dWT\r", GT.stored_round_no, GT.stored_group_no, min, secs);
            break;
        case TIMER_STATE_LT:
            snprintf(label, 16, "R%02dG%02dT%02d%02dLT\r", next_tfd.round_no, next_tfd.group_no, min, secs);
            // snprintf(label, 16, "R%02dG%02dT%02d%02dLT\r", GT.stored_round_no, GT.stored_group_no, min, secs);
            break;
        }
        if (strcmp(label, last_label) != 0)
        {
            Serial.printf("Sent to panel:  ");
            Serial.printf("%s", label);
            Serial1.printf("%s", label);
            strcpy(last_label, label);
        }
        break;
    }
}

void Flight_Manager::manage_panel_in_standby()
{
    // if (standby_recipe_counter == 0)
    // {
    //     switch (panel_type)
    //     {
    //     case I2C_IF:
    //         if (panel_enabled)
    //         {
    //             displaycounter(0, displayon, showcounter);
    //             Serial.printf(" secs t : %d\n", secs_to_land);
    //         }

    //         break;
    //     case SERIAL_IF:
    //         /* code */
    //         break;
    //     case SERIAL_IF_EXTENDED:
    //         /* code */
    //         break;
    //     default:
    //         break;
    //     }
    // }
}

void Flight_Manager::record_launch_data()

{
    ///////////////////////////////////
    // As a round is started the epoch of launch and landing
    // is stored as the first record of this round corresponding to tfd.pilot_no = 0;
    // pilot=0 record is retrieved in case of a GliderTimer reset
    ///////////////////////////////////
    valid_local_launch_sec = local_launch_sec; /// in case GT.scheduled=1
    valid_local_landing_timeout_sec = local_landing_timeout_sec;
    GT.stored_round_no = next_tfd.round_no;
    GT.stored_group_no = next_tfd.group_no;
    GT.current_round_no = GT.stored_round_no;
    GT.current_group_no = GT.stored_group_no;

    FLTdata.begin(); // cleaned in case GKeeper flights were received for auxiliary params
    FLTdata.tfd.comp_no = GT.mycompetition.head.contestNum;
    FLTdata.tfd.round_no = GT.stored_round_no;
    FLTdata.tfd.group_no = GT.stored_group_no;
    FLTdata.tfd.pilot_no = 0;
    FLTdata.tfd.fltnum = 999;
    FLTdata.tfd.F5J_millis = duration; /// transmit duration in F5J:millis if pilot_no =0;
    FLTdata.tfd.takeoff_local_secs = local_launch_sec;
    FLTdata.tfd.local_landing_timeout_sec = local_landing_timeout_sec;
    FLTdata.tfd.take_off_millis = pre_notice_minutes;
    // FLTdata.TC = serialnumber.substring(3);
    serialnumber.toCharArray(FLTdata.tfd.timer_serialnumber, 12);

    versionkeeper.toCharArray(FLTdata.tfd.timer_Firm_ver, 12);
    GT.stored_record_num++; /// we are ready for storing this launch
    FLTdata.tfd.record_num = GT.stored_record_num;

    Serial.printf("TTTTTTTTTTTT NEW FLIGHT STORED recno. = %d stored rec.= %d   : ", FLTdata.tfd.record_num, GT.stored_record_num);
    // serialnumber.toCharArray(FLTdata.tfd.serialnumber, 12);
    // versionkeeper.toCharArray(FLTdata.tfd.Firm_ver, 12);
    // serialnumber.toCharArray(FLTdata.tfd.timer_serialnumber, 12);
    // versionkeeper.toCharArray(FLTdata.tfd.timer_Firm_ver, 12);
    FLTdata.print_tfd(FLTdata.tfd);
    GT.mycompetition.print_contest();
    competition.save_record(&FLTdata.tfd); /// para permitir "abort"
}
void Flight_Manager::bip()
{
    M5.Speaker.beep();
}
void Flight_Manager::beep()
{
    M5.Speaker.tone(NOTE_DH2, 300);
}
void Flight_Manager::boop()
{

    M5.Speaker.tone(NOTE_DL5, 400);
}
void Flight_Manager::bpbp()
{
    M5.Speaker.tone(NOTE_DH2, 500);
}

void Flight_Manager::update_volume(uint32_t setting)
{
    M5.Speaker.setVolume(setting);
    volume = setting;
    M5.Speaker.update();
    save_settings();
}
void Flight_Manager::update_brightness(uint32_t setting)
{
    M5.Lcd.setBrightness(setting);
    brightness = setting;
    save_settings();
}

void Flight_Manager::update_collector(bool setting)
{
    GT.collector = setting;
    save_settings();
    // ESP.restart();
}

void Flight_Manager::update_groups(uint32_t setting)
{
    GT.groups = setting;
    save_settings();
    // ESP.restart();
}

void Flight_Manager::update_scheduled(uint32_t setting)
{
    GT.scheduled = setting;
    save_settings();
    // ESP.restart();
}

void Flight_Manager::read_settings()
{
    String line = "";
    File file = SPIFFS.open("/settings.txt", "r+");
    if (!file)
    {
        Serial.println("FILE SETTINGS NOT FOUND");
        volume = 6;
        brightness = 250;
        GT.collector = 0;
        GT.groups = 0;
        GT.scheduled = 0;
        GT.comp_step = 0;
        panel_type = 0;
        baud_rate1 = 9600;
        save_settings();
    }
    else
    {
        int xx = 1;
        while (file.available())
        {
            // Lets read line by line from the file
            line = "";
            line = file.readStringUntil('\n');
            line.replace("\n", "");
            line.replace("\r", "");
            int I = line.indexOf("=");

            // Serial.println(I);
            // Serial.print("linevalue");
            Serial.println(line);

            if (line.substring(0, I) == "volume")
            {
                volume = line.substring(I + 1).toInt();
                M5.Speaker.setVolume(volume);
                M5.Speaker.update();
            }
            if (line.substring(0, I) == "brightness")
            {
                brightness = line.substring(I + 1).toInt();
                M5.Lcd.setBrightness(brightness);
            }
            if (line.substring(0, I) == "collector")
            {
                GT.collector = line.substring(I + 1).toInt();
                if (GT.collector == 1)
                {
                    GD.background_colorx = GD.green;
                    // strcpy(localssid, "GliderTimeC");
                }
                else
                {
                    GD.background_colorx = GD.dark_blue;
                    // GD.background_colorx = GD.green;
                }
                if (GT.collector == 2)
                {
                    GD.background_colorx = 0;
                    strcpy(localssid, "GliderTmain");
                }
            }
            if (line.substring(0, I) == "groups")
            {
                GT.groups = line.substring(I + 1).toInt();
                Serial.printf(" groups :  %d \n", GT.groups);
            }
            yield();
            if (line.substring(0, I) == "scheduled")
            {
                GT.scheduled = line.substring(I + 1).toInt();
                Serial.printf("scheduled :  %d \n", GT.scheduled);
            }
            ///////////////new settings
            if (line.substring(0, I) == "comp_step")
            {
                GT.comp_step = line.substring(I + 1).toInt();
                Serial.printf("comp_step :  %d \n", GT.comp_step);
            }
            if (line.substring(0, I) == "strict_timing")
            {
                GT.strict_timing = line.substring(I + 1).toInt();
                // GT.strict_timing = 1          ;  ////////// TODO Sergio UI !!!
                Serial.printf("strict_timing :  %d \n", GT.strict_timing);
            }
            if (line.substring(0, I) == "panel_type")
            {
                panel_type = line.substring(I + 1).toInt();
                Serial.printf("panel_type :  %d \n", panel_type);
            }
            if (line.substring(0, I) == "baud_rate1")
            {
                baud_rate1 = line.substring(I + 1).toInt();
                Serial.printf("baud_rate1 :  %d \n", baud_rate1);
            }
            if (line.substring(0, I) == "background_volume")
            {
                MP3.music_vol = line.substring(I + 1).toInt()+5;  ///subidoo de 20max ahora sera 20+5
                Serial.printf("music_vol :  %d \n", MP3.music_vol);
            }
            if (line.substring(0, I) == "announcement_volume")
            {
                MP3.adv_vol = line.substring(I + 1).toInt();
                Serial.printf("adv_vol :  %d \n", MP3.adv_vol);
            }

            yield();
            xx++;
            if (xx > 14)
                break;
        }
        file.close();
    }
}
void Flight_Manager::print_settings()
{

    Serial.printf(" volume :  %d \n", volume);
    Serial.printf(" brightness :  %d \n", brightness);
    Serial.printf(" collector :  %d \n", GT.collector);
    Serial.printf(" groups :  %d \n", GT.groups);
    Serial.printf("scheduled :  %d \n", GT.scheduled);
    Serial.printf("comp_step :  %d \n", GT.comp_step);
    Serial.printf("strict_timing :  %d \n", GT.strict_timing);
    Serial.printf("panel_type :  %d \n", panel_type);
    Serial.printf("baud_rate1 :  %d \n", baud_rate1);
    Serial.printf("music_vol :  %d \n", MP3.music_vol);
    Serial.printf("adv_vol :  %d \n", MP3.adv_vol);
}

void Flight_Manager::save_settings()

{
    Serial.println("SAVING!!!! SETTINGS");
    File file = SPIFFS.open("/settings.txt", "w+");
    String TC = "volume=" + String(volume) + "\n";
    TC += "brightness=" + String(brightness) + "\n";
    TC += "collector=" + String(GT.collector) + "\n";
    TC += "groups=" + String(GT.groups) + "\n";
    TC += "scheduled=" + String(GT.scheduled) + "\n";
    TC += "comp_step=" + String(GT.comp_step) + "\n";
    TC += "strict_timing=" + String(GT.strict_timing) + "\n";
    TC += "panel_type=" + String(panel_type) + "\n";
    TC += "baud_rate=" + String(baud_rate1) + "\n";
    TC += "background_volume=" + String(MP3.music_vol) + "\n";
    TC += "announcement_volume=" + String(MP3.adv_vol) + "\n";

    file.print(TC);
    // file.print("\n");
    // Serial.print(TC);
    file.close();
}

uint32_t Flight_Manager::look_for_new_epoch()
{
    File file = SPIFFS.open("/scheduled_rounds.csv", "r");
    if (!file)
    {
        Serial.println("no scheduled table found");
    }
    else
    {
        uint xx = 1;
        uint round = 0;
        uint group = 0;
        uint32_t epoch;

        String line = file.readStringUntil('\n');
        line.replace("\n", "");
        line.replace("\r", "");
        Serial.println(line);
        if (line.indexOf("seq_number;Comp_numberNo;") >= 0) // Old format
        {
            while (file.available())
            {
                // Lets read line by line from the file
                line = file.readStringUntil('\n');
                line.replace("\n", "");
                line.replace("\r", "");

                Serial.println(line);

                uint32_t kk = GT.esplit(line, ";");
                if (GT.fields[1] > 0)
                {
                    GT.mycompetition.head.flt_duration = GT.fields[6]; // duration SPECIAL SIOEN
                    epoch = GT.fields[5];
                    struct timeval now;
                    gettimeofday(&now, NULL);
                    // Serial.printf("compara epoch %d now %d \n", epoch, now.tv_sec);
                    if ((epoch > now.tv_sec) && ((epoch - now.tv_sec) < 3600))
                    {
                        next_tfd.F5J_millis = GT.fields[6]; // duration
                        next_tfd.takeoff_local_secs = GT.fields[5];
                        next_tfd.local_landing_timeout_sec = next_tfd.takeoff_local_secs + next_tfd.F5J_millis;

                        next_tfd.round_no = GT.fields[3];
                        next_tfd.group_no = GT.fields[4];
                        next_tfd.comp_no = GT.fields[2];
                        next_tfd.record_num = GT.fields[1];
                        GT.mycompetition.head.external_comp_no = GT.fields[2];

                        return epoch;
                    }
                }
            }
            Serial.println("Endo of competition launch table");
        }
        else
        { /// New format
            while (file.available())
            {
                // Lets read line by line from the file
                line = file.readStringUntil('\n');
                line.replace("\n", "");
                line.replace("\r", "");

                Serial.println(line);

                uint32_t kk = GT.esplit(line, ";");
                if (GT.fields[1] > 0)
                {
                    GT.mycompetition.head.flt_duration = GT.fields[6]; // duration SPECIAL SIOEN
                    epoch = GT.fields[5];
                    struct timeval now;
                    gettimeofday(&now, NULL);
                    // Serial.printf("compara epoch %d now %d \n", epoch, now.tv_sec);
                    if ((epoch > now.tv_sec) && ((epoch - now.tv_sec) < 3600))
                    {
                        next_tfd.F5J_millis = GT.fields[6]; // duration
                        next_tfd.takeoff_local_secs = GT.fields[5];
                        next_tfd.local_landing_timeout_sec = next_tfd.takeoff_local_secs + next_tfd.F5J_millis;

                        next_tfd.round_no = GT.fields[2];
                        next_tfd.group_no = GT.fields[3];
                        next_tfd.comp_no = GT.fields[4]; // Here is the refly
                        next_tfd.record_num = GT.fields[1];
                        GT.mycompetition.head.external_comp_no = GT.fields[4];
                        return epoch;
                    }
                }
            }
            Serial.println("Endo of competition");
        }
    }
    return 0;
}

uint32_t Flight_Manager::look_for_duration_from_file()
{
    File file = SPIFFS.open("/scheduled_rounds.csv", "r");
    if (!file)
    {
        Serial.println("no scheduled table found");
    }
    else
    {
        uint xx = 1;
        uint round = 0;
        uint group = 0;
        uint32_t epoch;

        String line = file.readStringUntil('\n');
        line.replace("\n", "");
        line.replace("\r", "");
        Serial.println(line);
        if (line.indexOf("seq_number;Comp_numberNo;") >= 0) // Old format
        {
            while (file.available())
            {
                // Lets read line by line from the file
                line = file.readStringUntil('\n');
                line.replace("\n", "");
                line.replace("\r", "");

                Serial.println(line);

                uint32_t kk = GT.esplit(line, ";");
                if (GT.fields[1] > 0)
                {
                    GT.mycompetition.head.flt_duration = GT.fields[6]; // duration SPECIAL SIOEN
                    return GT.fields[6];
                }
            }
            Serial.println("Endo of competition launch table");
        }
        else
        { /// New format
            while (file.available())
            {
                // Lets read line by line from the file
                line = file.readStringUntil('\n');
                line.replace("\n", "");
                line.replace("\r", "");

                Serial.println(line);

                uint32_t kk = GT.esplit(line, ";");
                if (GT.fields[1] > 0)
                {
                    GT.mycompetition.head.flt_duration = GT.fields[6]; // duration SPECIAL SIOEN
                    return GT.fields[6];
                }
            }
            Serial.println("Endo of competition");
        }
    }
    return 0;
}

bool Flight_Manager::check_for_scheduled_start()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    if ((next_launch_epoch - now.tv_sec) < SCHEDULED_PRE_NOTICE)
    {
        return 1;
    }
    return 0;
}

void Flight_Manager::schedule_competition(uint32_t seconds_for_next_advert) // if seconds for >> time 2021-> launch
{
    int actual_grp = GT.stored_group_no + 1;
    int actual_seq;
    if (actual_grp > GT.mycompetition.head.number_of_groups)
    {
        actual_seq = GT.stored_round_no * GT.mycompetition.head.number_of_groups + 1;
        actual_grp = 1;
    }
    else
    {
        if (GT.stored_round_no == 0)
        {
            actual_seq = 1;
        }
        else
        {
            actual_seq = (GT.stored_round_no - 1) * GT.mycompetition.head.number_of_groups + actual_grp;
        }
    }
    GT.mycompetition.head.flt_duration = 600; ///// XXXJHR
    // GT.mycompetition.head.flt_duration = 390; ///// BUG TODO SIOEN
    int launch_separation_sec = GT.mycompetition.head.flt_duration + SCHEDULED_PRE_NOTICE + SCHEDULED_PRE_FLT_SEPARATION;
    int durat = GT.mycompetition.head.flt_duration;
    Serial.printf("actual:  seq %d Round %d group %d separation secs %d", actual_seq, GT.stored_round_no, actual_grp, launch_separation_sec);

    struct timeval now;
    gettimeofday(&now, NULL);
    struct tm *t_l;
    time_t laun_sec = now.tv_sec;
    t_l = localtime(&laun_sec);
    Serial.printf("Current local time and date: %s\n", asctime(t_l));
    uint32_t epoch = now.tv_sec + seconds_for_next_advert + SCHEDULED_PRE_NOTICE;
    if (seconds_for_next_advert > SECONDS_2021)
    {
        epoch = seconds_for_next_advert;
    }
    File file = SPIFFS.open("/scheduled_rounds.csv", "w");
    char line[150];
    snprintf(line, 150, "seq_number;Comp_numberNo;round;group;launch_time.tv_sec;duration;tm_sec;tm_min;tm_hour;tm_mday;tm_mon;tm_year\n");
    file.print(line);
    for (int seq = 1; seq < (GT.mycompetition.head.number_of_groups * GT.mycompetition.head.number_of_rounds + 1); seq++)
    {
        int ext_C_no = GT.mycompetition.head.external_comp_no;
        int rnd = ((seq - 1) / GT.mycompetition.head.number_of_groups) + 1;
        int grp = seq - (rnd - 1) * GT.mycompetition.head.number_of_groups;
        time_t laun_sec = epoch + (seq - actual_seq) * launch_separation_sec;
        struct tm *t_l;
        t_l = localtime(&laun_sec);
        snprintf(line, 150, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d\n", seq, ext_C_no, rnd, grp, laun_sec, durat, t_l->tm_sec, t_l->tm_min, t_l->tm_hour, t_l->tm_mday, t_l->tm_mon, t_l->tm_year + 1900);
        file.print(line);
        Serial.print(line);
    }
    file.close();
}

void Flight_Manager::print_betty_func()
{
   // printf("betty_step %d millis() %d actual vol %d \n", betty_step, millis(), MP3.actual_vol);
}

void Flight_Manager::perform_betty_deamon()
{
    if ((millis() - last_betty_millis) < betty_period)
    {
        return;
    }
    else
    {
        last_betty_millis = millis();
    }
    switch (betty_step)
    {
    case 0:
        MP3.volume_decr();
        betty_step++;
        break;
    case 1:
        MP3.volume_decr();
        betty_step++;
        break;
    case 2:
        MP3.volume_decr();
        betty_step++;
        break;
    case 3:
        MP3.volume_decr();
        betty_step++;
        break;
    case 4:
        MP3.volume_decr();
        betty_step++;
        break;
    case 5:
        MP3.volume(0);
        betty_step++;
        break;
    case 6:
        MP3.ad_file(next_round());
        adv_millis = millis();
        betty_step++;
        break;
    case 7:
        MP3.volume(MP3.adv_vol);
        betty_step++;
        break;
    case 8:
        if ((millis() - adv_millis) > wait_for_round)
        {
            MP3.volume(10);////////////////////////
            betty_step++;
        }
        break;
    case 9:
        MP3.ad_file(next_group());
        adv_millis = millis();
        betty_step++;
        break;
    case 10:
        delay(159);
        Serial.printf("Vol announcement------------------------------<: %d \n", MP3.adv_vol);
        MP3.volume(MP3.adv_vol);
        betty_step++;
        break;
    case 11:
        if ((millis() - adv_millis) > wait_for_group)
        {
            MP3.volume(0);
            Serial.printf("Vol zero \n");
            betty_step++;
        }
        break;
    case 12:
        MP3.volume_incr();
        // Serial.printf("Vol increase \n");
        betty_step++;
        break;
    case 13:
        MP3.volume_incr();
        // Serial.printf("Vol increase \n");
        betty_step++;
        break;
    case 14:
        MP3.volume_incr();
        // Serial.printf("Vol increase \n");
        betty_step++;
        break;
    case 15:
        start_betty_deamon = 0;
        Serial.printf("Betty finish \n");

        betty_step = 0;
        break;

    default:
        break;
    }
    //  Serial.printf("performed betty step %D\n", betty_step);
     print_betty_func();
}

uint32_t Flight_Manager::next_round()
{
    if (next_tfd.round_no == 0)
    {
        int next_group = GT.stored_group_no + 1;
        if (GT.stored_group_no == -1)
        {
            Serial.println("group increased to 1");
            next_group = 1;
        }
        int next_round = GT.stored_round_no;
        Serial.printf("GT.stored_group_no %d GT.stored_round_no %d\n ", GT.stored_group_no, GT.stored_round_no);
        if (next_group > GT.mycompetition.head.number_of_groups)
        {
            next_group = 1;
            next_round = GT.stored_round_no + 1;
        }
        if (GT.stored_round_no == 0)
        {
            next_round = 1;
        }
        // if (next_round > GT.mycompetition.head.number_of_rounds)
        // {
        //     next_round = 0;
        //     next_group = 0;
        // }
        next_round = next_round + 6 * GT.comp_step;
        Serial.printf("next_round %d next_group %d\n ", next_round, next_group);
        // MP3.file_to_play = (next_round - 1) * GT.mycompetition.head.number_of_groups + next_group;
        // Serial.printf("FILE TO PLAY: %d \n",MP3.file_to_play);
        // if ((MP3.file_to_play < 1) || (MP3.file_to_play > 36))
        // {
        //     MP3.file_to_play = 1;
        // }
        // next_round = 3000 + next_round; // next_round = 3001;
        next_round = 2300 + next_round; // next_round = 3001;
        Serial.printf("round file: %d \n", next_round);
        return next_round;
    }
    else
    {
        // return 3000 + next_tfd.round_no;
        return 2300 + next_tfd.round_no;
    }
}

uint32_t Flight_Manager::next_group()
{
    if (next_tfd.group_no == 0)
    {
        int next_group = GT.stored_group_no + 1;
        int next_round = GT.stored_round_no;
        if (next_group > GT.mycompetition.head.number_of_groups)
        {
            next_group = 1;
            next_round = GT.stored_round_no + 1;
        }
        if (GT.stored_round_no == 0)
        {
            GT.stored_round_no = 1;
        }
        if (next_round > GT.mycompetition.head.number_of_rounds)
        {
            next_round = 0;
            next_group = 15;
        }
        next_group = 2400 + next_group;
        // next_group = 4000 + next_group;
        // next_group = 4015;
        Serial.printf("groups file: %d \n", next_group);
        return next_group;
    }
    else
    {
        return 2400 + next_tfd.group_no;
    }
}