
/* Generated from GenMyModel */

#include "Timer_flight_data.h"
extern GTimer GT;
extern Flight_Manager flight;
extern CompetitionSaved competition;
extern GTdisplay GD;
extern String versionkeeper;
extern String serialnumber;
extern String FLT_filename;
extern int FLT_mode;

void Timer_flight_data::begin()
{
    // tfd.comp_no = GT.mycompetition.head.contestNum;
    // if(tfd.comp_no>16)tfd.comp_no=0;

    timer_to_keeper.tv_sec = 0;
    timer_to_keeper.tv_usec = 0;
    landing_shift.tv_sec = 0;
    landing_shift.tv_usec = 0;
    landing_allowance.tv_sec = 0;
    landing_allowance.tv_usec = 0;

    timerclear(&keeper_takeoff_time);
    timerclear(&keeper_launch_time);
    timerclear(&keeper_landing_time);
    timerclear(&keeper_landing_time);
    timerclear(&keeper_landing_timeout);

    tfd.comp_no = 0;
    tfd.record_num = 0;
    tfd.round_no = 0;
    tfd.group_no = 0;
    tfd.fltnum = 0;
    tfd.pilot_no = 0;
    tfd.pilot_id = 0;
    tfd.F5J_height = 0;
    tfd.F5J_millis = 0;
    tfd.throttle_shut_millis = 0;
    tfd.landing_millis = 0;
    // tfd.landing_pressure = 0;
    tfd.take_off_millis = 0;
    tfd.takeoff_local_secs = 0;
    tfd.takeoff_local_microsecs = 0;
    tfd.timer_to_keeper_sec = 0;
    tfd.timer_to_keeper_usec = 0;
    tfd.local_landing_timeout_sec = 0;
    tfd.local_landing_timeout_usec = 0;
    tfd.rest_millis = 0;
    tfd.flight_mins = 0;
    tfd.flight_secs = 0;
    tfd.transmit_local_secs = 0;
    tfd.transmit_local_usecs = 0;
    tfd.emergency_used = 0;
    tfd.from_NVS = 0;
    tfd.timedout = 0;
    tfd.landing_dist = 0;
    tfd.penalty_pts = 0;
    tfd.raw_score = 0;
    TC = "undefined";
    TC.toCharArray(tfd.serialnumber, 12);
    TC.toCharArray(tfd.Firm_ver, 12);
    TC.toCharArray(tfd.timer_serialnumber, 12);
    TC.toCharArray(tfd.timer_Firm_ver, 12);
}

void Timer_flight_data::set_FLTnum(String arg)
{
    tfd.fltnum = (uint32_t)arg.toInt();
}
void Timer_flight_data::set_flight_mins(String arg)
{
    tfd.flight_mins = (uint32_t)arg.toInt();
}
void Timer_flight_data::set_flight_secs(String arg)
{
    tfd.flight_secs = (uint32_t)arg.toInt();
}
void Timer_flight_data::set_F5J_millis(String arg)
{
    tfd.F5J_millis = (uint32_t)arg.toInt();
}
void Timer_flight_data::set_F5J_height(String arg)
{
    tfd.F5J_height = (uint32_t)arg.toInt();
}
void Timer_flight_data::set_take_off_millis(String arg)
{
    tfd.take_off_millis = (uint32_t)arg.toInt();
}
void Timer_flight_data::set_throttle_shut_millis(String arg)
{
    // Serial.print("ARGUMENTO=");Serial.print(arg);Serial.println("|");
    tfd.throttle_shut_millis = (uint32_t)arg.toInt();
}
void Timer_flight_data::set_landing_millis(String arg)
{
    // Serial.print("ARGUMENTO=");Serial.print(arg);Serial.println("|");
    tfd.landing_millis = (uint32_t)arg.toInt();
}
void Timer_flight_data::set_takeoff_local_secs(String argsecs)
{
    tfd.takeoff_local_secs = (uint32_t)argsecs.toInt();
}
void Timer_flight_data::set_takeoff_local_microsecs(String argmillisecs)
{
    tfd.takeoff_local_microsecs = (uint32_t)(argmillisecs.toInt() * 1000); // we receive millisecs due format in  table at Keeper
}
void Timer_flight_data::set_transmit_local_secs(String argmillisecs)
{
    tfd.transmit_local_secs = (uint32_t)(argmillisecs.toInt());
}
void Timer_flight_data::set_transmit_local_microsecs(String argmillisecs)
{
    tfd.transmit_local_usecs = (uint32_t)(argmillisecs.toInt());
}
void Timer_flight_data::set_rest_millis(String arg)
{
    tfd.rest_millis = (uint32_t)arg.toInt();
}
void Timer_flight_data::set_Firm_ver(String arg)
{
    arg.toCharArray(tfd.Firm_ver, 12);
}
void Timer_flight_data::set_serialnumber(String arg)
{
    arg.toCharArray(tfd.serialnumber, 12);
}

void Timer_flight_data::set_keeperIP(String arg)
{
    char mykeeperIP[20];
    arg.toCharArray(mykeeperIP, 20);

    sscanf(mykeeperIP, "%u.%u.%u.%u", &keeperIP[0], &keeperIP[1], &keeperIP[2], &keeperIP[3]);
}
void Timer_flight_data::set_emergency_used(String arg)
{
    if (((uint8_t)arg.toInt()) > 0)
    {
        tfd.emergency_used = 1;
    }
    else
    {
        tfd.emergency_used = 0;
    }
    // Serial.print("000===arg=");
    // Serial.print(arg);
    // Serial.print("arg.toINT=");
    // Serial.println(tfd.emergency_used);
}
// void Timer_flight_data::set_RTCtimed(String arg)
// {
//     if (((uint8_t)arg.toInt()) > 0)
//     {
//         tfd.RTCtimed = 1;
//     }
//     else
//     {
//         tfd.RTCtimed = 0;
//     }
// }
void Timer_flight_data::set_from_NVS(String arg)
{
    if (((uint8_t)arg.toInt()) > 0)
    {
        tfd.from_NVS = 1;
    }
    else
    {
        tfd.from_NVS = 0;
    }
}

void Timer_flight_data::set_name_in_keeper(String arg)
{

    arg.toCharArray(name_in_keeper, 12);
}
void Timer_flight_data::set_nick_in_keeper(String arg)
{
    if (arg.length() == 0)
    {
        arg = String(tfd.serialnumber);
        arg = arg.substring(arg.length() - 4);
    }
    arg.toCharArray(nick_in_keeper, 5);
}

// void Timer_flight_data::serialnumber(String arg)
// {
//     arg.toChararray(serialnumber,12)
// }

void Timer_flight_data::set_timedout(bool timedout)
{
    tfd.timedout = timedout;
}

void Timer_flight_data::set_landing_dist(uint distance)
{
    tfd.landing_dist = distance;
}

bool Timer_flight_data::check_sychro_needed()
{
    keeperRTC = 0;
    if (tfd.takeoff_local_secs > 1612137600) // Date and time (GMT):> Monday, 1 February 2021 0:00:00
    {
        keeperRTC = 1;
        return 0; ///  para comprobar synchro, mirar de poner aqui 1
    }
    if (tfd.emergency_used)
    {
        return 0;
    }
    if (tfd.from_NVS)
    {
        return 0;
    }
    if (GT.stored_competition_closed)
    {
        return 0;
    }
    if (GT.strict_timing > 0) // TODO 4.15
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

uint32_t Timer_flight_data::check_landing_moment()
{

    if (tfd.emergency_used) // Check if Emergency Used
    {
        GD.clean_partial_flt();
        GD.alabel = "Emergency used ";
        tfd.timedout = 2;
        return INVALID;
    }
    bothRTC = timerRTC & keeperRTC;
    if (!keeperRTC && tfd.from_NVS)
    {
        return UNKNOWN; // if Keeper was powered down and it did not have RTC
    }

    if (GT.strict_timing == 0) ///  TODO 4.15 bypasses the Checking and makes it manual
    {
        return UNKNOWN;
    }

    ///// Start of timing checks for the flight //////////
    /// takeoff: the moment keeper measures throttle advance
    /// landing: the moment keeper detects contact to terrain
    /// launch : the moment working time starts
    /// landing_timeout : the moment working time finishes
    ///
    /// kepper_xxx epoch in keeper clock
    /// timer_xxxx epoch in timer clock
    ///
    /// timer_to_keeper the timeshit to pas from timer clock to (+) keeper  (positive is keeper clock is "later")
    /////

    landing_allowance.tv_sec = 0;
    landing_allowance.tv_usec = LANDING_ALLOWANCE_MICROS + SYNCHRO_ALLOWANCE_MICROS;
    keeper_takeoff_time.tv_sec = tfd.takeoff_local_secs;
    keeper_takeoff_time.tv_usec = tfd.takeoff_local_microsecs;
    Serial.printf("Landing allowance %d milliseconds\n", landing_allowance.tv_usec);

    /// timeval::timeshift for the flight, from takeoff
    landing_shift.tv_sec = (int)((tfd.landing_millis - tfd.take_off_millis) / 1000);
    landing_shift.tv_usec = (uint32_t)(((tfd.landing_millis - tfd.take_off_millis) % 1000) * 1000);
    // Serial.printf("Landing shift %d seconds %d milliseconds\n", landing_shift.tv_sec, landing_shift.tv_usec);
    // Serial.printf("%d minutes %d seconds\n", tfd.flight_mins, tfd.flight_secs);

    /// landing time= takeof time shifted for flight shift
    keeper_landing_time = shift_time(keeper_takeoff_time, landing_shift);

    //////LOCAL TIMER CONDITIONS

    keeper_launch_time.tv_sec = flight.valid_local_launch_sec - 1; //////LOCAL TIMER CONDITIONS: LAUNCH
    keeper_launch_time.tv_usec = 1000000 - SYNCHRO_ALLOWANCE_MICROS;
    keeper_launch_time = shift_time(keeper_launch_time, timer_to_keeper); // the time working time start in keeper clock, inclusive allowance

    Serial.printf("Launch_t %d seconds %d milliseconds\n", keeper_launch_time.tv_sec, keeper_launch_time.tv_usec);

    keeper_landing_timeout.tv_sec = flight.valid_local_landing_timeout_sec; //////LOCAL TIMER CONDITIONS: LAND
    keeper_landing_timeout.tv_usec = 0;                                     /// it is always a second o'clock

    keeper_landing_timeout = shift_time(keeper_landing_timeout, landing_allowance);
    keeper_landing_timeout = shift_time(keeper_landing_timeout, timer_to_keeper); // the time working time finishes in keeper clock, inclusive allowance
    Serial.printf("Land TO %d seconds %d milliseconds\n", keeper_landing_timeout.tv_sec, keeper_landing_timeout.tv_usec);

    tfd.local_landing_timeout_sec = keeper_landing_timeout.tv_sec;
    tfd.local_landing_timeout_usec = keeper_landing_timeout.tv_usec; // stored in the database

    Serial.printf("tfd.local_landing_timeout_sec %d tfd.local_landing_timeout_sec %d \n", tfd.local_landing_timeout_sec, tfd.local_landing_timeout_sec);
    Serial.println();
    timeshift results_launch = compare_times(keeper_launch_time, keeper_takeoff_time);
    timeshift results = compare_times(keeper_landing_time, keeper_landing_timeout);
    if (results_launch.tv_sec < 0)
    {
        GD.clean_partial_flt();
        tfd.timedout = 2;
        GD.alabel = "Too Early Launch ";
        // Serial.println("return INVALID Too Early Launch;");
        return INVALID;
    }
    if (results.tv_sec < (-60))
    {
        GD.clean_partial_flt();
        tfd.timedout = 2;
        GD.alabel = "Landing time ";
        // Serial.println("return INVALID Landing time;");

        return INVALID;
    }
    else
    {

        if (results.tv_sec < (0)) // the landing was detected after the landing_timeout
        {
            landing_shift = shift_time(landing_shift, results); // flight time is reduced
            if ((tfd.landing_millis - tfd.take_off_millis) > (GT.mycompetition.head.flt_duration * 1000 - 100))
            {
                landing_shift.tv_sec = landing_shift.tv_sec - 1;
            }
            tfd.flight_secs = landing_shift.tv_sec % 60;
            tfd.flight_mins = (landing_shift.tv_sec - tfd.flight_secs) / 60;
            // Serial.printf("Vuelo recortado a: %d minutos; %d segundos\n", tfd.flight_mins, tfd.flight_secs);

            tfd.timedout = 1;
            return TIMED_OUT;
        }
        else
        {

            if ((tfd.landing_millis - tfd.take_off_millis) > (GT.mycompetition.head.flt_duration * 1000 - 100))
            {
                landing_shift.tv_sec = landing_shift.tv_sec - 1;
                tfd.landing_millis = tfd.landing_millis - 1000;
                tfd.flight_secs = landing_shift.tv_sec % 60;
                tfd.flight_mins = (landing_shift.tv_sec - tfd.flight_secs) / 60;
                tfd.timedout = 1;
                // Serial.printf("Vuelo Ligeramente excedido recortado a: %d minutos; %d segundos\n", tfd.flight_mins, tfd.flight_secs);
                return TIMED_OUT; // the flight was longer than working time but was OK due allowances
            }
            tfd.timedout = 0;
            return SUCCESS;
        }
    }
}

timeshift Timer_flight_data::compare_times(timeval before, timeval after)
{
    timeshift shift;
    if (after.tv_usec > before.tv_usec)
    {
        shift.tv_usec = after.tv_usec - before.tv_usec;
        shift.tv_sec = after.tv_sec - before.tv_sec;
    }
    else
    {
        shift.tv_usec = after.tv_usec + 1000000 - before.tv_usec;
        shift.tv_sec = after.tv_sec - before.tv_sec - 1;
        // Serial.printf("results after %d before %d seconds\n", after.tv_sec, before.tv_sec);
    }
    return shift;
}
timeval Timer_flight_data::shift_time(timeval origin, timeshift shift)
{
    timeval shifted;
    int buff = origin.tv_usec + shift.tv_usec;
    if (buff > 1000000)
    {
        shifted.tv_usec = buff - 1000000;
        shifted.tv_sec = origin.tv_sec + shift.tv_sec + 1;
    }
    else
    {
        if (buff < 0)
        {
            shifted.tv_usec = buff + 1000000;
            shifted.tv_sec = origin.tv_sec + shift.tv_sec - 1;
        }
        else
        {
            shifted.tv_usec = buff;
            shifted.tv_sec = origin.tv_sec + shift.tv_sec;
        }
    }
    return shifted;
}

timeshift Timer_flight_data::shift_time(timeshift origin, timeshift shift)
{
    timeshift shifted;
    int buff = origin.tv_usec + shift.tv_usec;
    if (buff > 1000000)
    {
        shifted.tv_usec = buff - 1000000;
        shifted.tv_sec = origin.tv_sec + shift.tv_sec + 1;
    }
    else
    {
        if (buff < 0)
        {
            shifted.tv_usec = buff + 1000000;
            shifted.tv_sec = origin.tv_sec + shift.tv_sec - 1;
        }
        else
        {
            shifted.tv_usec = buff;
            shifted.tv_sec = origin.tv_sec + shift.tv_sec;
        }
    }
    return shifted;
}
void Timer_flight_data::sprint_tfd(struct datastore tfd, char *payload)
{
    char buffer[40];
    if (FLT_mode == CONNECTED_AS_STA)
    {

        //         Here is the sample request used for testing. You should create your own request.

        // http://gliderscore.com/scoringdatamanage.aspx?ACTION=ImportScore&ID=1C4E74c26627&RD=1&GR=2&P=76&T=958&H=135&LDG=6&GT75m=N&PLTY=0
        // Responses that you might receive are these:
        //             Response.Write("Failed-CompNotFound")
        //             Response.Write("Failed-ScoreAlreadyUpdated")
        //             Response.Write("Failed-GT75m-TimeHeightLandingnotZero")
        //             Response.Write("Failed-PenaltyMustBePositive")
        //             Response.Write("Failed-TimeEntered-HeightDataMissing")
        //             Response.Write("Failed-HeightEntered-TimeDataMissing")
        //             Response.Write("Failed-TimeSecondsOver59")
        //             Response.Write("Failed-TimeHasTooManyDecimals")
        //             Response.Write("Failed-TimeGreaterThanMaximum")
        //             Response.Write("Failed-InvalidLandingData")
        //             Response.Write("Failed-MatchingRecordNotFound")
        //             Response.Write("ScoreUpdated")

        strcpy(payload, LINK_TRAG);
        // snprintf(buffer, 40, "ID=");
        snprintf(buffer, 40, "ID=%s", GT.mycompetition.head.external_comp_ID);
        // Serial.printf("byffer %s",buffer);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&RD=%d", tfd.round_no + 6 * GT.comp_step);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&GR=%d", tfd.group_no);
        strcat(payload, buffer);
        // snprintf(buffer, 40, "&P=%d", tfd.pilot_id);
        snprintf(buffer, 40, "&P=%d", tfd.pilot_id);
        strcat(payload, buffer);
        int tt = tfd.flight_mins * 100 + tfd.flight_secs;
        if ((tfd.landing_dist > 75) || (tfd.emergency_used != 0))
        {
            strcat(payload, "&T=0&H=0&LDG=0");
        }
        else
        {
            snprintf(buffer, 40, "&T=%d", tt);
            strcat(payload, buffer);
            snprintf(buffer, 40, "&H=%d", (uint32_t)(tfd.F5J_height / 10));
            strcat(payload, buffer);
            snprintf(buffer, 40, "&LDG=%d", tfd.landing_dist);
            strcat(payload, buffer);
        }

        if (tfd.landing_dist > 75)
        {
            snprintf(buffer, 40, "&GT75m=Y");
        }
        else
        {
            snprintf(buffer, 40, "&GT75m=N");
        }
        strcat(payload, buffer);
        snprintf(buffer, 40, "&PLTY=0");
        strcat(payload, buffer);
        //Serial.printf(" El trag de GS es: %s\n", payload);
    }
    else
    {
        strcpy(payload, TIMER_TRAG);
        snprintf(buffer, 40, "comp_no=%d", tfd.comp_no);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&record_num=%d", tfd.record_num);

        strcat(payload, buffer);
        snprintf(buffer, 40, "&round_no=%d", tfd.round_no);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&group_no=%d", tfd.group_no);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&pilot_no=%d", tfd.pilot_no);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&pilot_id=%d", tfd.pilot_id);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&fltnum=%d", tfd.fltnum);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&take_off_millis=%d", tfd.take_off_millis);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&takeoff_local_secs=%d", tfd.takeoff_local_secs);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&takeoff_local_microsecs=%d", tfd.takeoff_local_microsecs);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&throttle_shut_millis=%d", tfd.throttle_shut_millis);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&F5J_height=%d", tfd.F5J_height);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&F5J_millis=%d", tfd.F5J_millis);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&landing_millis=%d", tfd.landing_millis);
        strcat(payload, buffer);
        // snprintf(buffer, 40, "&landing_pressure=%d", tfd.landing_pressure);
        // strcat(payload, buffer);
        snprintf(buffer, 40, "&rest_millis=%d", tfd.rest_millis);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&flight_mins=%d", tfd.flight_mins);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&flight_secs=%d", tfd.flight_secs);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&transmit_local_secs=%d", tfd.transmit_local_secs);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&transmit_local_usecs=%d", tfd.transmit_local_usecs);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&timer_to_keeper_sec=%d", tfd.timer_to_keeper_sec);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&timer_to_keeper_usec=%d", tfd.timer_to_keeper_usec);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&emergency_used=%d", tfd.emergency_used);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&from_NVS=%d", tfd.from_NVS);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&local_landing_timeout_sec=%d", tfd.local_landing_timeout_sec);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&local_landing_timeout_usec=%d", tfd.local_landing_timeout_usec);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&timedout=%d", tfd.timedout);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&landing_dist=%d", tfd.landing_dist);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&penalty_pts=%d", tfd.penalty_pts);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&raw_score=%d", tfd.raw_score);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&serialnumber=%s", tfd.serialnumber);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&Firm_ver=%s", tfd.Firm_ver);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&timer_serialnumber=%s", tfd.timer_serialnumber);
        strcat(payload, buffer);
        snprintf(buffer, 40, "&timer_Firm_ver=%s", tfd.timer_Firm_ver);
        strcat(payload, buffer);
    }
}

void Timer_flight_data::print_tfd(struct datastore tfd)

{
    char payload[800];
    sprint_tfd(tfd, payload);
    Serial.println(payload);
    //Serial.printf("CompoNo: %d RecNo: %d RoundNo: %d GroupNo: %d pilotNo: %d pilotId: %d FLTnum: %d \n", tfd.comp_no, tfd.record_num, tfd.round_no, tfd.group_no, tfd.pilot_no, tfd.pilot_id, tfd.fltnum);
}

Splitted Timer_flight_data::splitPL(String payload)
{
    // int start = payload.indexOf("=");
    // int end = payload.indexOf("&");
    // Splitted splitted;
    // // Serial.print(end);
    // // Serial.println(" " + payload);

    // //Serial.println(payload);
    // splitted.cacho = payload.substring(0, start + 1);
    // // Serial.print(splitted.cacho);
    // // Serial.println(splitted.numero);
    // if (end > 0)
    // {
    //     splitted.numero = payload.substring(start + 1, end);
    //     splitted.split = payload.substring(end + 1);
    // }
    // else
    // {
    //     splitted.numero = payload.substring(start + 1);
    // }
    // return splitted;
}

/*!
 * @brief This internal API is used to generate the string to respond
 * note this string is in the format of the one sent by GK
 * @param[in]  void
 * @param[out] payload a text string for response
 *
 */
/*!
 * @brief This internal API is used to generate a string from a tfd given  used  to geenrate a csv dump to respond
 *
 * @param[in]  mytfd
 * @param[out] payload a text string for response
 *
 */

void Timer_flight_data::datastore2String(struct datastore mytfd, bool csv_enabled, char *mypayload)
{
    // char mypayload[200];
    char deli[4];
    if (csv_enabled)
    {
        strcpy(deli, ";");
    }
    else
    {
        strcpy(deli, ";");
    }
    /////////////////////////   1    2
    snprintf(mypayload, 500, "%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%d%s%s%s%s%s%s%s%s%s\n",
             mytfd.comp_no, deli,
             mytfd.record_num, deli,
             mytfd.round_no, deli,
             mytfd.group_no, deli,
             mytfd.pilot_no, deli,
             mytfd.pilot_id, deli,
             mytfd.fltnum, deli,
             mytfd.take_off_millis, deli,
             mytfd.takeoff_local_secs, deli,
             mytfd.takeoff_local_microsecs, deli,
             mytfd.throttle_shut_millis, deli,
             mytfd.F5J_height, deli,
             mytfd.F5J_millis, deli,
             mytfd.landing_millis, deli,
             //  mytfd.landing_pressure, deli,
             mytfd.rest_millis, deli,
             mytfd.flight_mins, deli,
             mytfd.flight_secs, deli,
             mytfd.transmit_local_secs, deli,
             mytfd.transmit_local_usecs, deli,
             mytfd.timer_to_keeper_sec, deli,
             mytfd.timer_to_keeper_usec, deli,
             mytfd.emergency_used, deli,
             mytfd.from_NVS, deli,
             mytfd.local_landing_timeout_sec, deli,
             mytfd.local_landing_timeout_usec, deli,
             mytfd.timedout, deli,
             mytfd.landing_dist, deli,
             mytfd.penalty_pts, deli,
             mytfd.raw_score, deli,
             mytfd.serialnumber, deli,
             mytfd.Firm_ver, deli,
             mytfd.timer_serialnumber, deli,
             mytfd.timer_Firm_ver, deli);
}

String Timer_flight_data::last_launch_payload()
{
    char payload[800];
    begin();
    tfd.comp_no = GT.stored_competition_no;
    tfd.group_no = GT.stored_group_no;
    tfd.round_no = GT.stored_round_no;
    tfd.fltnum = 999;
    tfd.F5J_millis = GT.mycompetition.head.flt_duration; /// para el caso de  mandar un wT
    tfd.take_off_millis = flight.pre_notice_minutes;
    tfd.takeoff_local_secs = flight.local_launch_sec;
    versionkeeper.toCharArray(tfd.Firm_ver, 12);
    serialnumber.toCharArray(tfd.serialnumber, 12);
    struct timeval timesent;
    if (!gettimeofday(&timesent, NULL))
    {
        // Serial.println("Failed to obtain timeX"); //pareciara que si 0 es bien...
        // return;
    }
    tfd.transmit_local_secs = timesent.tv_sec;
    tfd.transmit_local_usecs = timesent.tv_usec;
    sprint_tfd(tfd, payload);
    Serial.print("payload");
    Serial.println(payload);
    return String(payload);
}

bool Timer_flight_data::payload2tfd(String payload)
{
    // begin(); //limpiamos FLTdata
    // Splitted splitted = splitPL(payload);
    // //keeperIP = splitted.numero;
    // splitted = splitPL(splitted.split);
    // tfd.fltnum = splitted.numero.toInt(); //FLTnum, the first after a "="
    // splitted = splitPL(splitted.split);
    // tfd.flight_mins = splitted.numero.toInt();
    // splitted = splitPL(splitted.split);
    // tfd.flight_secs = splitted.numero.toInt();
    // splitted = splitPL(splitted.split);
    // tfd.F5J_height = splitted.numero.toInt();
    // splitted = splitPL(splitted.split);
    // tfd.F5J_millis = splitted.numero.toInt();
    // splitted = splitPL(splitted.split);
    // tfd.take_off_millis = splitted.numero.toInt();
    // splitted = splitPL(splitted.split);
    // tfd.throttle_shut_millis = splitted.numero.toInt();
    // splitted = splitPL(splitted.split);
    // tfd.landing_millis = splitted.numero.toInt();
    // splitted = splitPL(splitted.split);
    // tfd.takeoff_local_secs = splitted.numero.toInt();
    // splitted = splitPL(splitted.split);
    // tfd.rest_millis = splitted.numero.toInt();
    // splitted = splitPL(splitted.split);
    // tfd.takeoff_local_microsecs = splitted.numero.toInt() * 1000; /// from milis to micros
    // splitted = splitPL(splitted.split);
    // splitted.numero.toCharArray(tfd.Firm_ver, 12);
    // splitted = splitPL(splitted.split);
    // splitted.numero.toCharArray(tfd.serialnumber, 12);
    // splitted = splitPL(splitted.split);
    // tfd.emergency_used = splitted.numero.toInt();
    // splitted = splitPL(splitted.split);
    // //   tfd.RTCtimed=splitted.numero.toInt();
    // splitted = splitPL(splitted.split);
    // tfd.from_NVS = splitted.numero.toInt();
    // splitted = splitPL(splitted.split);
    // //   tfd.boolpad=splitted.numero.toInt();
    // splitted = splitPL(splitted.split);
    // tfd.transmit_local_secs = splitted.numero.toInt();
    // splitted = splitPL(splitted.split);
    // tfd.transmit_local_usecs = splitted.numero.toInt();

    // //
    // //
    // //
    // //
    // //   FLTdata.set_from_NVS(server.arg("from_NVS"));
}

bool Timer_flight_data::str_splitPL(String payload)
{
    char str[800];
    char *pch;
    char *pcv;
    char kk[28];
    char qq[28];
    bool confirmed_tfd = 0;
    begin();

    if (payload.length() > 0)
    {
        payload.toCharArray(str, 800);
        pch = strtok(str, "?&");
        int i = 0;
        // pch = strtok(NULL, "?&="); ///cojo la segunda trama
        while (pch != NULL)
        {
            //Serial.printf("coincidencia encontrada %s = %d",pch,tfd.comp_no);

            sprintf(kk, "%s", pch);

            if (strcmp(kk, "comp_no") == 0)
                tfd.comp_no = atoi(pcv);

            if (strcmp(kk, "record_num") == 0)
                tfd.record_num = atoi(pcv);

            if (strcmp(kk, "round_no") == 0)
                tfd.round_no = atoi(pcv);

            if (strcmp(kk, "group_no") == 0)
                tfd.group_no = atoi(pcv);

            if (strcmp(kk, "pilot_no") == 0)
                tfd.pilot_no = atoi(pcv);

            if (strcmp(kk, "pilot_id") == 0)
            {
                tfd.pilot_id = atoi(pcv);
            }
            if (strcmp(kk, "fltnum") == 0)
            {
                tfd.fltnum = atoi(pcv);
            }
            if (strcmp(kk, "FLTnum") == 0)
            {
                tfd.fltnum = atoi(pcv);
            }
            if (strcmp(kk, "take_off_millis") == 0)
                tfd.take_off_millis = atoi(pcv);

            if ((strcmp(kk, "takeoff_local_secs") == 0) || (strcmp(kk, "takeoff_localsecs") == 0))
                tfd.takeoff_local_secs = atoi(pcv);

            if ((strcmp(kk, "takeoff_local_microsecs") == 0) || (strcmp(kk, "takeoff_localmillisecs") == 0))
                tfd.takeoff_local_microsecs = atoi(pcv);

            if (strcmp(kk, "throttle_shut_millis") == 0)
                tfd.throttle_shut_millis = atoi(pcv);

            if (strcmp(kk, "F5J_height") == 0)
                tfd.F5J_height = atoi(pcv);

            if (strcmp(kk, "F5J_millis") == 0)
                tfd.F5J_millis = atoi(pcv);

            if (strcmp(kk, "landing_millis") == 0)
                tfd.landing_millis = atoi(pcv);

            // if (strcmp(kk, "landing_pressure") == 0)
            //     tfd.landing_pressure = atoi(pcv);

            if (strcmp(kk, "rest_millis") == 0)
                tfd.rest_millis = atoi(pcv);

            if (strcmp(kk, "flight_mins") == 0)
                tfd.flight_mins = atoi(pcv);

            if (strcmp(kk, "flight_secs") == 0)
                tfd.flight_secs = atoi(pcv);

            if (strcmp(kk, "transmit_local_secs") == 0)
                tfd.transmit_local_secs = atoi(pcv);

            if ((strcmp(kk, "transmit_local_usecs") == 0) || (strcmp(kk, "transmit_local_microsecs") == 0))
                tfd.transmit_local_usecs = atoi(pcv);

            if (strcmp(kk, "timer_to_keeper_sec") == 0)
                tfd.timer_to_keeper_sec = atoi(pcv);

            if (strcmp(kk, "timer_to_keeper_usec") == 0)
                tfd.timer_to_keeper_usec = atoi(pcv);

            if (strcmp(kk, "emergency_used") == 0)
                tfd.emergency_used = atoi(pcv);

            if (strcmp(kk, "from_NVS") == 0)
                tfd.from_NVS = atoi(pcv);

            if (strcmp(kk, "local_landing_timeout_sec") == 0)
                tfd.local_landing_timeout_sec = atoi(pcv);

            if (strcmp(kk, "local_landing_timeout_usec") == 0)
                tfd.local_landing_timeout_usec = atoi(pcv);

            if (strcmp(kk, "timedout") == 0)
                tfd.timedout = atoi(pcv);

            if (strcmp(kk, "landing_dist") == 0)
                tfd.landing_dist = atoi(pcv);

            if (strcmp(kk, "penalty_pts") == 0)
                tfd.penalty_pts = atoi(pcv);

            if (strcmp(kk, "raw_score") == 0)
                tfd.raw_score = atoi(pcv);

            if (strcmp(kk, "serialnumber") == 0)
            {
                strcpy(tfd.serialnumber, pcv);
                confirmed_tfd = 1;
            }

            if (strcmp(kk, "Firm_ver") == 0)
                strcpy(tfd.Firm_ver, pcv);

            if (strcmp(kk, "timer_serialnumber") == 0)
                strcpy(tfd.timer_serialnumber, pcv);

            if (strcmp(kk, "timer_Firm_ver") == 0)
                strcpy(tfd.timer_Firm_ver, pcv);

            if (strcmp(kk, "name") == 0) /// only from keeper
                strcpy(name_in_keeper, pcv);

            if (strcmp(kk, "nick") == 0)
                strcpy(nick_in_keeper, pcv);

            pch = strtok(NULL, "?&="); /// cojo la primera trama (&)
            pcv = strtok(NULL, "?&="); /// cojo la segunda trama (=)
            if (pcv != NULL)
            {
                sprintf(kk, "%s", pch);
                sprintf(qq, "%s", pcv);
                Serial.printf("pch= %s ; pcv %s \n", kk, qq);
            }
        }
        // delay(4000);

        // Serial.println("a tfd wad created from String");
        // print_tfd(tfd); // return splitted;
    }
    else
    {
        Serial.println("Alarm!! wrong payload input!!!!!!!!!!!!!!!!!!!!!!!!!");
    }
    return confirmed_tfd;
}

void Timer_flight_data::create_dummy_group_data(int round, int group)
{

    for (int piloti = 1; piloti <= GT.mycompetition.head.number_of_pilots; piloti++)
    {
        begin();
        // print_tfd(GT.draw_data[GT.stored_round_no][piloti]);
        // tfd = GT.draw_data[GT.stored_round_no][piloti];

        Serial.printf("Creating dummy GT.stored_round_no %d GT.stored_group_no %d GT.mycompetition.draw_list[GT.stored_round_no][piloti].group %d \n", GT.stored_round_no, GT.stored_group_no, GT.mycompetition.draw_list[GT.stored_round_no][piloti].group);
        if ((GT.mycompetition.draw_list[GT.stored_round_no][piloti].group == GT.stored_group_no) || (GT.groups_enabled == 0))
        {
            tfd.comp_no = GT.stored_competition_no;
            tfd.record_num = 0;
            tfd.round_no = round;
            tfd.group_no = group;
            tfd.fltnum = 888;
            tfd.pilot_no = piloti;
            tfd.pilot_id = GT.pilots_table[piloti].pilot_id;
            tfd.F5J_height = 100 + 10 * piloti;
            tfd.F5J_millis = 11000;
            tfd.throttle_shut_millis = 10000;
            tfd.landing_millis = 12000;
            // tfd.landing_pressure = 0;
            tfd.take_off_millis = 9000;
            tfd.takeoff_local_secs = 1000000;
            tfd.takeoff_local_microsecs = 100000;
            tfd.timer_to_keeper_sec = 0;
            tfd.timer_to_keeper_usec = 0;
            tfd.local_landing_timeout_sec = 1000000;
            tfd.local_landing_timeout_usec = 0;
            tfd.rest_millis = 0;
            tfd.flight_mins = piloti % 10;
            tfd.flight_secs = (10 * round + group) % 60;
            tfd.transmit_local_secs = 1000000; // this is now timer to keeper shift
            tfd.transmit_local_usecs = 100000;
            tfd.emergency_used = 0;
            tfd.from_NVS = 1;
            tfd.timedout = 0;
            tfd.landing_dist = group;
            tfd.penalty_pts = 0;
            tfd.raw_score = tfd.flight_mins * 60 - tfd.F5J_height / 2 + 50 - (tfd.landing_dist - 1) * 5;
            strncpy(tfd.serialnumber, GT.pilots_table[piloti].serialnumber, 12);
            FLT_filename.toCharArray(tfd.Firm_ver, 12); // this is an invention to make this work
            serialnumber.toCharArray(tfd.timer_serialnumber, 12);
            versionkeeper.toCharArray(tfd.timer_Firm_ver, 12);

            GT.stored_record_num++;
            tfd.record_num = GT.stored_record_num; // update draw data and save
            GT.draw_data[round][piloti] = tfd;
            competition.save_record(&tfd);
            print_tfd(tfd);
        }
    }
}