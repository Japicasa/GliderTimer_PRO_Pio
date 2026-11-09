#include "GTimer.h"
//#include "../keeper.h"
extern GTdisplay GD;
extern GTimer GT;
extern Timer_flight_data FLTdata;
extern CompetitionSaved competition;
extern Flight_Manager flight;

extern uint8_t FLT_mode;
extern String versionkeeper;
extern String serialnumber;
extern uint8_t page_to_show;
extern boolean sinWifi;
extern uint32_t GLOBAL_DURATION;

extern void send_single_tfd();

void GTimer::begin()
{
    delay(100);
    // Serial.printf("por aqui pase !!!!!!!!!!!! 1 millis %d \n", millis());
    Serial.println();

    clear_mycompetition(); // clean mycompetition.head

    // Serial.println("GT clear_mycompetition s  :");
    delay(100);
    // Serial.printf("por aqui pase !!!!!!!!!!!! 2 millis %d \n", millis());
    Serial.println();

    clear_draw_data(); // clear the records in draw data

    // Serial.println("GT clear_draw_data s  :");
    delay(100);
    // Serial.printf("por aqui pase !!!!!!!!!!!! 3 millis %d \n", millis());
    Serial.println();

    clear_draw_list(); // clear the records in draw data

    // Serial.println("GT clear_draw_list s  :");
    delay(100);

    clear_contest_pilot_list(); // clear the pilots

    stored_round_no = 0; // global variables of GTimer
    current_round_no = 0;
    stored_group_no = 0;
    stored_record_num = 0;
    current_round_no = 0;
    current_record_num = 0;
    current_group_no = 0;
    selected_pilots = 0;
    registered_pilots = 0;
    new_pilot_added = 0;
    receiving_working_time = 0;
    // Serial.printf("por aqui pase !!!!!!!!!!!! 4 millis %d \n", millis());
    // Serial.println();

    stored_competition_no = competition.init(); /// the good competition from flash

    // Serial.printf("por aqui pase !!!!!!!!!!!! 5 %d \n", millis());
    Serial.println();
    Serial.printf("this is the stored competition %d \n", stored_competition_no);

    if (stored_competition_no < 0)
    {
        manage_error(ERROR_FLASH);
    }
    current_competition_no = stored_competition_no;
    stored_competition_closed = 1;

    if (mycompetition.head.number_of_rounds > 1000)
    {
        Serial.printf("BAD COMPETITION IN FLASH!!!, formatting");
        competition.format();
        ESP.restart();
    }
    /// qqq load competition
    if (stored_competition_no > 0)
    {
        Serial.printf("loading competition millis %d\n", millis());

        mycompetition = competition.Load_Competition(stored_competition_no);
        clear_draw_list();
        stored_round_no = 0;
        stored_group_no = 0;
        registered_pilots = 0;
        groups_enabled = mycompetition.head.groups_enabled;
        set_competition_max_values();

        if (mycompetition.draw_list[0][0].pilot_id != COMPETITION_CLOSED)
        { // check for closed
            stored_competition_closed = 0;
        }
        Serial.printf("loading draw data %d\n", millis());

        load_draw_data(stored_competition_no); // Loads draw_data in its memory structure from tfd´s recorded in keeperpartition,
        // returns the environtmental values as current_xx
        //     Serial.printf("=====================================loaded draw data %d\n", millis());
        //    print_draw_data();
        //    Serial.printf("=====================================loaded draw data END %d\n", millis());
        stored_round_no = current_round_no;
        stored_record_num = current_record_num;
        stored_group_no = current_group_no;
    }
    else
    {

        groups_enabled = (groups > 0);
    }
    // Serial.printf(" mycompetiton.head.number_of_groups %d groups %d ",mycompetition.head.number_of_groups, groups);
    Serial.printf("From Records: Competition number %d Rounds : %d mycompetition.head.number_of_groups : %d G_enabled %d DATA version %d Record _num %d \n", mycompetition.head.contestNum, mycompetition.head.number_of_rounds, mycompetition.head.number_of_groups, groups_enabled, mycompetition.head.data_version, stored_record_num);
    if (groups_enabled > 0)
    {
        load_pilot_master_file();
        load_draw_from_file();
    }
    else
    {
        // Serial.printf("por aqu pase 6!!!!!!!!!! millis %d \n", millis());
        load_pilots_table();
    }
    // check_for_end_round();

    // Serial.printf("por aqu pase 7!!!!!!!!!!\n");
    print_draw_data();
    // print_draw_list();
    // Serial.printf("por aqu pase 8!!!!!!!!!!\n");

    if (stored_competition_closed == 0)
    // if ((stored_competition_closed == 0) && (!collector))
    {
        if (!groups_enabled)
        {
            mycompetition.calculate_scores();
            Serial.printf("updating Scores On load !!!!!!!!!!\n");
            print_draw_list();
        }
    }
    mycompetition.print_contest();
    // Serial.printf("por aqu pase 9!!!!!!!!!!\n");
}

void GTimer::close_competition()
{
    if (stored_competition_closed == 0)
    {
        mycompetition.draw_list[0][0].pilot_id = COMPETITION_CLOSED;
        competition.save_competition(&mycompetition);
        stored_competition_closed = 1;
    }
}

void GTimer::load_pilots_table()
{
    Serial.println("load started ");
    // copiad a de read_settings
    String line = "";
    File file = SPIFFS.open("/pilotos.csv", "r+");
    Serial.println("load started2 ");
    if (!file)
    {
        Serial.println("no pilots table found");
        return;
    }
    else
    {
        int xx = 1;
        bool deleted = 0;
        bool one_deleted = 0;
        String myname;
        line = file.readStringUntil('\n');
        line.replace("\n", "");
        line.replace("\r", "");

        Serial.println(line);
        if ((line == "pilot_id;serialnumber;name;nick_name;") || (line == "pilot_id;serialnumber;name;nick_name"))
        {
            while (file.available())
            {
                // Lets read line by line from the file
                line = file.readStringUntil('\n');
                line.replace("\n", "");
                line.replace("\r", "");
                Serial.println(line);
                int I = line.indexOf(";");
                deleted = 0;

                Serial.println(I);
                Serial.print("linevalue ");
                Serial.println(line);
                // pilots_table[xx].pilot_id = line.substring(0, I).toInt();
                pilots_table[xx].pilot_id = xx;
                line = line.substring(I + 1);
                I = line.indexOf(";");
                // Serial.println(line);
                line.substring(0, I).toCharArray(pilots_table[xx].serialnumber, 12);
                line = line.substring(I + 1);
                I = line.indexOf(";");
                // Serial.println(line);
                myname = line.substring(0, I);
                if ((myname.indexOf("Delete") > -1) || (myname.indexOf("delete") > -1) || (myname.indexOf("DELETE") > -1))
                {
                    deleted = 1;
                    one_deleted = 1;
                }

                line.substring(0, I).toCharArray(pilots_table[xx].name, 25);
                line = line.substring(I + 1);
                I = line.indexOf(";");
                // Serial.println(line);
                line.substring(0, I).toCharArray(pilots_table[xx].nick_name, 5);
                pilots_table[0].pilot_id = xx; // the number of valid stored pilots is here
                xx++;
                if (deleted)
                {
                    xx = xx - 1;
                    pilots_table[0].pilot_id = xx - 1;
                }
                if (xx > MAX_SELECTION_TABLE_PILOTS)
                {
                    file.close();
                    Serial.println("Too many pilots");
                    return;
                }
            }
        }
        else
        {
            Serial.println("Problems with the format of file");
            file.close();
            return;
        }
        file.close();
        if (one_deleted)
        { // save the pilots after one deleted found
            int new_num = pilots_table[0].pilot_id;
            Serial.println("Saving pilots table after a deleted");

            File file = SPIFFS.open("/pilotos.csv", "w+");
            line = "pilot_id;serialnumber;name;nick_name;\n";
            file.print(line);
            for (int i = 1; i <= new_num; i++)
            {
                line = String(pilots_table[i].pilot_id) + ";" +
                       String(pilots_table[i].serialnumber) + ";" +
                       String(pilots_table[i].name) + ";" +
                       String(pilots_table[i].nick_name) + ";\n";
                if (i == new_num)
                {
                    line.replace("\n", "");
                }
                // Serial.print("linevalue ");
                //     Serial.println(line);
                file.print(line);
            }
            file.close();
            line = "";
            for (int i = 1; i <= 15; i++) /// hay que linpiar hasta el fondo
            {
                pilots_table[i].pilot_id = 0;
                line.toCharArray(pilots_table[i].serialnumber, 12);
                line.toCharArray(pilots_table[i].name, 25);
                line.toCharArray(pilots_table[i].nick_name, 5);
            }
            load_pilots_table();
        }
        Serial.println("Successful simple table pilot load");
    }
}
void GTimer::load_pilot_master_file()
{

    String line = "";
    File file = SPIFFS.open("/pilot_master_file.csv", "r+");
    if (!file)
    {
        Serial.println("no pilot_master_file found");
        return;
    }
    else
    {
        int xx = 1;
        int round = 0;
        int group = 0;
        int l_spot = 0;

        int pilot_number = 0;
        int pilot_id = 0;
        for (int i = 0; i < MAX_SELECTION_POINTER_PILOT; i++)
        {
            group_pilots_table[i].pilot_id = 0;
            // group_pilots_table[i].serialnumber2[0] = 0;
            // group_pilots_table[i].serialnumber3[0] = 0;
            // pilots_table[i].name[0] = '\0';  //TODO pilots name
            // pilots_table[i].nick_name[0] = '\0';
            // pilot_id_contest[i] = 0;
        }
        line = file.readStringUntil('\n');
        line = file.readStringUntil('\n');
        line = file.readStringUntil('\n');
        line.replace("\n", "");
        line.replace("\r", "");
        int pilot_format = line.indexOf("CompID=");
        if (pilot_format >= 0)
        {
            int idx = line.indexOf("=");
            // line.substring(idx + 1, idx + 13).toCharArray(mycompetition.head.external_comp_ID, 20);
            line.substring(idx + 1).toCharArray(mycompetition.head.external_comp_ID, 20);
        }
        line = file.readStringUntil('\n');
        line.replace("\n", "");
        line.replace("\r", "");
        // Serial.print("xx");
        // Serial.println(line);
        if ((line.indexOf("NAME;") >= 0) || (line.indexOf("Name;") >= 0))
        {
            int i = 0;
            while (file.available())
            {
                // Lets read line by line from the file
                line = file.readStringUntil('\n');

                line.replace("\n", "");
                line.replace("\r", "");
                line.replace(",", "-");
                // Serial.println(line);
                // uint32_t kk = esplit(line, ";");
                // for (int i =1; i<20; i++){
                //     Serial.println(textxtract(line, ";", i));
                // }
                if (pilot_format < 0)
                {
                    i++;
                    uint32_t kk = esplit(line, ";");
                    String name = textxtract(line, ";", 1);
                    String WorkPhone = textxtract(line, ";", 11);
                    Serial.println(WorkPhone);
                    String QRcode = textxtract(line, ";", 12);
                    // Serial.println(QRcode);
                    name.replace("\"", "");
                    WorkPhone.replace("\"", "");
                    WorkPhone.replace("*", "");
                    QRcode.replace("\"", "");
                    uint32_t QQ = QRcode.indexOf("?ID=");
                    kk = QRcode.indexOf("&P=");
                    if (kk == -1)
                    {
                        kk = QRcode.indexOf("?P=");
                        if (kk == -1)
                        {
                            Serial.println("PILOT NUMBER NOT FOUND!!");
                        }
                    }
                    if (i == 1)
                        QRcode.substring(QQ + 4, kk).toCharArray(mycompetition.head.external_comp_ID, 20);
                    Serial.printf("la com ID es: %s \n", mycompetition.head.external_comp_ID);
                    QRcode = QRcode.substring(kk + 3);
                    // Serial.print("workphone ");
                    // Serial.println(WorkPhone);
                    Serial.print("QRcode fo P= ");
                    Serial.println(QRcode);

                    group_pilots_table[i].pilot_id = QRcode.toInt();
                    kk = esplit(WorkPhone, "/");
                    char SerialNumber[12];

                    textxtract(WorkPhone, "/", 1).toCharArray(SerialNumber, 12);
                    group_pilots_table[i].serialnumber = char2_SN(SerialNumber); // for groups serialnumber is an uint32

                    textxtract(WorkPhone, "/", 2).toCharArray(SerialNumber, 12);
                    group_pilots_table[i].serialnumber2 = char2_SN(SerialNumber);

                    textxtract(WorkPhone, "/", 3).toCharArray(SerialNumber, 12);
                    group_pilots_table[i].serialnumber3 = char2_SN(SerialNumber);

                    // textxtract(WorkPhone, "/", 2).toCharArray(pilots_table[i].serialnumber2, 12);
                    // textxtract(WorkPhone, "/", 3).toCharArray(pilots_table[i].serialnumber3, 12);

                    // textxtract(WorkPhone, "/", 1).toCharArray(pilots_table[i].serialnumber, 12);
                    // textxtract(WorkPhone, "/", 2).toCharArray(pilots_table[i].serialnumber2, 12);
                    // textxtract(WorkPhone, "/", 3).toCharArray(pilots_table[i].serialnumber3, 12);

                    // name.toCharArray(pilots_table[i].name, 25);   //TODO pilots name
                    // name.toCharArray(pilots_table[i].nick_name, 5);
                    if (pilots_table[i].pilot_id > 0)
                    {
                        Serial.printf("pilot: %d nombre: %s nick: %s id: %d SN: %s SN2: %s SN3: %s \n", i, "name", "nick", pilots_table[i].pilot_id, pilots_table[i].serialnumber, pilots_table[i].serialnumber2, pilots_table[i].serialnumber3);
                    }
                }
                else
                {
                    // Serial.println("PILOT FORMAT: NEW!!");
                    i = fill_pilot_data(line, i);
                }
            }
        }
        else
        {
            Serial.print("Problems with format of pilots master list");
        }
    }
    file.close();
    Serial.println("Successfull pilot_table load from file");
}
void GTimer::load_draw_from_file()
{
    // copiad a de read_settings
    // if (stored_competition_closed == 1)
    // {

    //     return;
    // }
    String line = "";
    File file = SPIFFS.open("/draw_table.csv", "r+");
    if (!file)
    {
        Serial.println("no draw table found");
        return;
    }
    else
    {
        uint xx = 1;
        uint round = 0;
        uint group = 0;
        max_group_in_file = 0;
        max_round_in_file = 0;
        max_pilot_number_in_file = 0;
        uint pilot_number = 0;
        uint pilot_id = 0;
        line = file.readStringUntil('\n');
        line.replace("\n", "");
        line.replace("\r", "");
        // Serial.println(line);
        if (line.indexOf("CompNo;TaskNo;RoundNo;GroupNo;ReFlightNo;PilotNo;") >= 0)
        {
            while (file.available())
            {
                // Lets read line by line from the file
                line = file.readStringUntil('\n');
                line.replace("\n", "");
                line.replace("\r", "");
                uint32_t kk = esplit(line, ";");
                round = fields[3];
                if (round >= GROUP_MAX_ROUND)
                {
                    Serial.println("GROUP_MAX_ROUND reached");
                    break;
                }
                if ((fields[1] > 0))
                {
                    mycompetition.head.external_comp_no = fields[1];
                    round = fields[3];
                    pilot_id = fields[6];
                    group = fields[4];
                    pilot_number = find_pilot_ID_in_table(pilot_id);

                    group_draw_list[round][pilot_number].round = round;
                    group_draw_list[round][pilot_number].pilot_number = pilot_number;
                    if (stored_competition_no == 2) // competition 2 is the flyoff
                    {
                        group_draw_list[round][pilot_number].group = 1;
                    }
                    else
                    {
                        group_draw_list[round][pilot_number].group = group;
                    }
                    group_draw_list[round][pilot_number].pilot_id = pilot_id;

                    if (group > max_group_in_file)
                    {
                        max_group_in_file = group;
                        Serial.print(max_group_in_file);
                        mycompetition.head.number_of_groups = max_group_in_file;
                        // Serial.println("   GROUP INCREASED!!");
                    }
                    if (round > max_round_in_file)
                    {
                        max_round_in_file = round;
                        Serial.print(max_round_in_file);
                        mycompetition.head.number_of_rounds = max_round_in_file;
                        // Serial.println("   ROUND INCREASED!!");
                    }
                    if (pilot_number > max_pilot_number_in_file)
                    {
                        max_pilot_number_in_file = pilot_number;
                        Serial.printf("max_pilot_number_in_file %d \n", max_pilot_number_in_file);
                    }
                    xx++;
                    if (xx > ((GROUP_MAX_SELECTION_POINTER_ROUND - 1) * (GROUP_MAX_SELECTION_POINTER_PILOT - 1)))
                    {
                        file.close();
                        Serial.println("Too many draw lines ");
                        return;
                    }
                }
                else
                {
                    Serial.println("BYPASSED line!");
                }
                // Serial.printf("drawLine: %d round: %d pilot_number: %d group: %d id: %d \n", xx - 1, group_draw_list[round][pilot_number].round, group_draw_list[round][pilot_number].pilot_number, group_draw_list[round][pilot_number].group, group_draw_list[round][pilot_number].pilot_id);
            }
        }
        else
        {
            Serial.println("Problems with the format of file draw.csv");
            file.close();
            return;
        }
        file.close();
        Serial.printf("Successful Draw list load__ rounds %d \n", fields[3]);
        Serial.printf("XXXXX max_pilot_number_in_file %d \n", max_pilot_number_in_file);
    }
}

uint32_t GTimer::new_pilot_in_pilots_table()
{
    if (pilots_table[0].pilot_id > MAX_SELECTION_TABLE_PILOTS)
    {
        return 0; // no room for more pilots
    }
    else
    {
        pilots_table[0].pilot_id++;
        int new_num = pilots_table[0].pilot_id;
        String line = String(FLTdata.tfd.serialnumber);
        line = line.substring(7);
        // Serial.print("new_num ") ;
        // Serial.println(new_num);
        pilots_table[new_num].pilot_id = new_num;
        sprintf(pilots_table[new_num].serialnumber, "%s", FLTdata.tfd.serialnumber, 12);
        sprintf(pilots_table[new_num].nick_name, "%s", FLTdata.nick_in_keeper, 5);
        // Serial.println(pilots_table[new_num].nick_name);
        sprintf(pilots_table[new_num].name, "%s", FLTdata.name_in_keeper, 12);
        File file = SPIFFS.open("/pilotos.csv", "w+");
        line = "pilot_id;serialnumber;name;nick_name;\n";
        file.print(line);
        for (int i = 1; i <= new_num; i++)
        {
            // line = String(pilots_table[i].pilot_id) + ";" +
            line = String(i) + ";" +
                   String(pilots_table[i].serialnumber) + ";" +
                   String(pilots_table[i].name) + ";" +
                   String(pilots_table[i].nick_name) + ";\n";
            if (i == new_num)
            {
                line.replace("\n", "");
            }
            file.print(line);
            // Serial.println(line);
        }
        file.close();
        return new_num;
    }
}

uint32_t GTimer::find_sn_in_pilots_table(char serialnumber[12])
{
    for (int pilot_id = 0; pilot_id < 16; pilot_id++)
    {
        if (strcmp(serialnumber, pilots_table[pilot_id].serialnumber) == 0)
        {
            return pilot_id;
        }
    }
    return 0;
}

void GTimer::load_draw_data(uint32_t competition_no)
{
    group_draw_data_pointer = 0;
    for (int loader = 1; loader < (max_round * max_pilot); loader++)
    {
        FLTdata.begin();
        bool found;

        if (loader == 1)
        {
            found = competition.getFirstRecord(competition_no, &FLTdata.tfd);
        }
        else
        {
            found = competition.getNextRecord(&FLTdata.tfd);
        }
        // Serial.print("found");
        // Serial.println(found);

        if (found)
        {
            Serial.println("===NEW record FOUND ===");
            // FLTdata.print_tfd(FLTdata.tfd);
            /// xyxy
            Serial.printf("CompoNo: %d RecNo: %d RoundNo: %d GroupNo: %d pilotNo: %d pilotId: %d FLTnum: %d \n", FLTdata.tfd.comp_no, FLTdata.tfd.record_num, FLTdata.tfd.round_no, FLTdata.tfd.group_no, FLTdata.tfd.pilot_no, FLTdata.tfd.pilot_id, FLTdata.tfd.fltnum);

            /////////////////////////////////////////////////////////////
            // if(FLTdata.tfd.pilot_id==5){
            //     FLTdata.tfd.pilot_no=3;
            // }
            ///////////////////////////////////////////////////////

            if ((FLTdata.tfd.comp_no == competition_no) && (FLTdata.tfd.round_no <= max_group) && (FLTdata.tfd.pilot_no <= max_pilot))
            {
                // Serial.println("===NEW record loaded===");
                if (groups_enabled)
                {
                    group_draw_data_pointer++;
                    if (group_draw_data_pointer == MAX_GROUP_DRAW_DATA)
                    {
                        Serial.println("===re_KK===");
                        group_draw_data_pointer = 1;
                    }
                    // Serial.print(" tfd");
                    // FLTdata.print_tfd(FLTdata.tfd);
                    // Serial.print(" drawdata");
                    group_draw_data[group_draw_data_pointer] = FLTdata.tfd;
                    // Serial.printf("<>group draw data pointer %d \n Guardado en group_draw_data: ", group_draw_data_pointer);
                    // FLTdata.print_tfd(group_draw_data[group_draw_data_pointer]);
                }
                else
                {
                    draw_data[FLTdata.tfd.round_no][FLTdata.tfd.pilot_no] = FLTdata.tfd;
                    if (mycompetition.draw_list[0][FLTdata.tfd.pilot_no].pilot_id == 0)
                    {
                        Serial.println("===loading Pilot ID===");
                        for (int i = 0; i < MAX_SELECTION_POINTER_ROUND; i++)
                        {
                            mycompetition.draw_list[i][FLTdata.tfd.pilot_no].pilot_id = FLTdata.tfd.pilot_id;
                            mycompetition.draw_list[i][FLTdata.tfd.pilot_no].group = 1;
                        }
                    }
                    // serialnum_contest[FLTdata.tfd.pilot_no] = String(FLTdata.tfd.serialnumber);
                }
                if (current_round_no != FLTdata.tfd.round_no)
                {
                    current_round_no = FLTdata.tfd.round_no;
                    Serial.print("XXX NEW round loaded===");
                    Serial.println(current_round_no);
                }
                current_record_num = loader; // loadear
                current_group_no = FLTdata.tfd.group_no;

                // serialnum_contest[FLTdata.tfd.pilot_no] = String(FLTdata.tfd.serialnumber);
                strcpy(pilots_table[FLTdata.tfd.pilot_no].serialnumber, FLTdata.tfd.serialnumber);

                // Serial.print("serialnumber ");
                // Serial.println(pilots_table[FLTdata.tfd.pilot_no].serialnumber);
                if (FLTdata.tfd.pilot_no > registered_pilots)
                {
                    registered_pilots = FLTdata.tfd.pilot_no;
                }
                if (FLTdata.tfd.pilot_no == 0) /// Stored Launch info for the round/group
                {
                    flight.local_launch_sec = FLTdata.tfd.takeoff_local_secs;
                    flight.local_landing_timeout_sec = FLTdata.tfd.local_landing_timeout_sec;
                    flight.valid_local_launch_sec = flight.local_launch_sec; /// valid is for keeper landing check
                    flight.valid_local_landing_timeout_sec = flight.local_landing_timeout_sec;
                    // Serial.printf("Comp %d, Record: %d, round: %d, Lauch Secs: %d\n", competition_no, current_record_num, stored_round_no, flight.local_launch_sec);
                }

                // FLTdata.print_tfd(GT.draw_data[FLTdata.tfd.round_no][FLTdata.tfd.pilot_no]);
                // Serial.printf("XXXXXXXXXXXXComp %d, Record: %d, round: %d\n", competition_no, stored_record_num, current_round_no);
                // Serial.print("Leyendo ");
                // FLTdata.print_tfd(FLTdata.tfd);
            }
            else
            {
                // Serial.printf("Comp %d, Record: %d, round: %d\n", competition_no, stored_record_num, stored_round_no);
                // Serial.println("===BAD FORMAT ftldata===");
                Serial.printf("<final>comp changed/ round or pilot number exceeded %d \n", group_draw_data_pointer);
                break;
            }
        }
        else
        {
            // Serial.print("XXX FINAL round loaded===");
            // Serial.println(stored_round_no);
            Serial.printf("<final_not found>group draw data pointer %d \n", group_draw_data_pointer);
            break;
        }

        // buscar donde ponerlo////////////
    }
}

void GTimer::load_stored_competition(uint32_t competition_no)
{
    if (competition_no > stored_competition_no)
    {
        return;
    }
    close_competition();

    clear_mycompetition();
    clear_draw_data();
    clear_draw_list();
    clear_contest_pilot_list();
    current_competition_no = competition_no;

    // Serial.print("Begin de competition+++++++++++++ encontrado?:  ");
    // Serial.println(competition_no);
    if (competition_no > 0)
    {
        mycompetition = competition.Load_Competition(competition_no);
        current_round_no = 0;
        load_draw_data(competition_no);
        // for (int loader = 1; loader < (MAX_SELECTION_POINTER_ROUND * MAX_SELECTION_POINTER_PILOT); loader++)
        // {
        //     FLTdata.begin();
        //     bool found;

        //     if (loader == 1)
        //     {
        //         found = competition.getFirstRecord(competition_no, &FLTdata.tfd);
        //     }
        //     else
        //     {
        //         found = competition.getNextRecord(&FLTdata.tfd);
        //     }
        //     // Serial.print("found");
        //     // Serial.println(found);

        //     if (found)
        //     {
        //         if ((FLTdata.tfd.comp_no == competition_no) && (FLTdata.tfd.round_no <= MAX_SELECTION_POINTER_ROUND) && (FLTdata.tfd.pilot_no <= MAX_SELECTION_POINTER_PILOT))
        //         {
        //             // Serial.println("===NEW recond loaded===");
        //             GT.draw_data[FLTdata.tfd.round_no][FLTdata.tfd.pilot_no] = FLTdata.tfd;
        //             current_round_no = FLTdata.tfd.round_no;
        //             current_record_num = loader;
        //             // Serial.print("XXX NEW round loaded===");
        //             // Serial.println(current_round_no);
        //             //
        //             serialnum_contest[FLTdata.tfd.pilot_no] = String(FLTdata.tfd.serialnumber);
        //             // Serial.print("serialnumber ");
        //             // Serial.println(serialnum_contest[FLTdata.tfd.pilot_no]);
        //             if (FLTdata.tfd.pilot_no > registered_pilots)
        //             {
        //                 registered_pilots = FLTdata.tfd.pilot_no;
        //             }

        //             //FLTdata.print_tfd(GT.draw_data[FLTdata.tfd.round_no][FLTdata.tfd.pilot_no]);
        //             // Serial.printf("Comp %d, Record: %d, round: %d\n", current_competition_no, current_record_num, current_round_no);
        //         }
        //         else
        //         {
        //             // Serial.printf("Comp %d, Record: %d, round: %d\n", current_competition_no, current_record_num, current_round_no);
        //             // Serial.println("===BAD FORMAT ftldata===");
        //             break;
        //         }
        //     }
        //     else
        //     {
        //         // Serial.print("XXX FINAL round loaded===");
        //         // Serial.println(current_round_no);
        //         break;
        //     }

        //     // buscar donde ponerlo////////////
        // }
        // Serial.print("XXX FINAL round loaded===");
        // Serial.println(current_round_no);
        if (current_round_no == 0)
        {
            current_round_no = 1;
            // Serial.println("===Round Number forced to 1===");
        }
        // Serial.print("XXX FINAL round loaded===");
        // Serial.println(current_round_no);

        // Serial.print("Pilot_0_id: ");
        // Serial.println(serialnum_contest[0]);
        // Serial.print("Pilot_1_id: ");
        // Serial.println(serialnum_contest[1]);
        // Serial.print("Pilot_2_id: ");
        // Serial.println(serialnum_contest[2]);
        // Serial.print("Pilot_3_id: ");
        // Serial.println(serialnum_contest[3]);
        // Serial.print("LOADED  COMPETITION");
        // Serial.println(mycompetition.head.contestNum);
        // Serial.print("Rounds: ");
        // Serial.println(mycompetition.head.number_of_rounds);
        // Serial.print("number_of_pilots: ");
        // Serial.println(mycompetition.head.number_of_pilots);
        // Serial.println("New Comp LOADED flash======================");

        // check_for_end_round();
        mycompetition.read_rank_list();
        print_draw_data();
        // mycompetition.calculate_scores();
        mycompetition.print_contest();
        // if (current_competition_no == 1)
        // {
        //     close_competition();
        //     competition.save_competition(&mycompetition);
        //     Serial.println("SAVED COMP 1        !!!");
        // }
    }
}

void GTimer::clear_mycompetition()
{
    mycompetition.head.contestNum = 0;
    mycompetition.head.number_of_rounds = 0;
    mycompetition.head.number_of_groups = 0;
    mycompetition.head.number_of_pilots = 0;
    // mycompetition.head.scores_locked = 0;
    mycompetition.head.draw_locked = 0;
    mycompetition.head.landing_millis_allowance = 400; // TODO poner una macro
    mycompetition.head.flt_duration = 600;
    if (GLOBAL_DURATION > 0)
    {
        mycompetition.head.flt_duration = GLOBAL_DURATION;
    }
    // clear_contest_pilot_list();
    // for (int pilot = 0; pilot < MAX_SELECTION_POINTER_PILOT; pilot++)
    // {
    //     pilots_table[pilot].pilot_id = 0;
    //     pilots_table[pilot].serialnumber = "\0";
    //     pilots_table[pilot].serialnumber2 = "\0";
    //     pilots_table[pilot].serialnumber = "\0";
    //     pilots_table[pilot].name = "\0";
    //     pilots_table[pilot].nick_name = "\0";
    // }
}

void GTimer::clear_draw_list() // this function is not really needed
{
    memset(mycompetition.draw_list, 0, sizeof(mycompetition.draw_list));
    memset(group_draw_list, 0, sizeof(group_draw_list));
    // for (int round = 0; round < MAX_SELECTION_POINTER_ROUND; round++)
    // {
    //     for (int pilot = 0; pilot < MAX_SELECTION_POINTER_PILOT; pilot++)
    //     {

    //         mycompetition.draw_list[round][pilot].pilot_number = 0;
    //         mycompetition.draw_list[round][pilot].pilot_id = 0;
    //         mycompetition.draw_list[round][pilot].group = 0;
    //         mycompetition.draw_list[round][pilot].round = 0;
    //         mycompetition.draw_list[round][pilot].round_rank = 0;
    //         mycompetition.draw_list[round][pilot].FAI_score = 0;
    //        // mycompetition.draw_list[round][pilot].penalty = 0;
    //         mycompetition.draw_list[round][pilot].discarded_round = 0;
    //         // for (int group = 0; group < MAX_SELECTION_POINTER_GROUP; group++)
    //         // {
    //         //     rank_list[round][group][pilot] = 0;
    //         // }
    //     }
    // }
}

void GTimer::clear_draw_list_scores() // this function is not really needed
{

    for (int round = 0; round < MAX_SELECTION_POINTER_ROUND; round++)
    {

        for (int pilot = 0; pilot < MAX_SELECTION_POINTER_PILOT; pilot++)
        {

            mycompetition.draw_list[round][pilot].FAI_score = 0;
            mycompetition.draw_list[round][pilot].round_rank = 0;
            // mycompetition.draw_list[round][pilot].penalty = 0;
            mycompetition.draw_list[round][pilot].discarded_round = 0;
            for (int group = 0; group < MAX_SELECTION_POINTER_GROUP; group++)
            {
                rank_list[round][group][pilot] = 0;
            }
        }
    }
}

void GTimer::clear_draw_data() // this function is not really needed

{
    String vacio = "            ";
    memset(draw_data, 0, sizeof(draw_data));
    memset(group_draw_data, 0, sizeof(group_draw_data));
    // for (int round = 0; round < MAX_SELECTION_POINTER_ROUND; round++)
    // {
    //     for (int pilot = 0; pilot < MAX_SELECTION_POINTER_PILOT; pilot++)
    // for (int round = 0; round < 2; round++)
    // {
    //     for (int pilot = 0; pilot < 2; pilot++)
    //     {
    //         draw_data[round][pilot].comp_no = 0;
    //         draw_data[round][pilot].record_num = 0;
    //         draw_data[round][pilot].round_no = 0;
    //         draw_data[round][pilot].group_no = 0;
    //         draw_data[round][pilot].pilot_no = 0;
    //         draw_data[round][pilot].fltnum = 0;
    //         draw_data[round][pilot].take_off_millis = 0;
    //         draw_data[round][pilot].takeoff_local_secs = 0;
    //         draw_data[round][pilot].takeoff_local_microsecs = 0;
    //         draw_data[round][pilot].throttle_shut_millis = 0;
    //         draw_data[round][pilot].F5J_height = 0;
    //         draw_data[round][pilot].F5J_millis = 0;
    //         draw_data[round][pilot].landing_millis = 0;
    //         // draw_data[round][pilot].landing_pressure = 0;
    //         draw_data[round][pilot].rest_millis = 0;
    //         draw_data[round][pilot].flight_mins = 0;
    //         draw_data[round][pilot].flight_secs = 0;
    //         draw_data[round][pilot].transmit_local_secs = 0;
    //         draw_data[round][pilot].transmit_local_usecs = 0;
    //         draw_data[round][pilot].timer_to_keeper_sec = 0;
    //         draw_data[round][pilot].timer_to_keeper_usec = 0;
    //         draw_data[round][pilot].emergency_used = 0;
    //         // draw_data[round][pilot].RTCtimed = 0;
    //         draw_data[round][pilot].from_NVS = 0;
    //         // draw_data[round][pilot].boolpad = 0;
    //         // draw_data[round][pilot].landing_spot_no = 0;
    //         draw_data[round][pilot].local_landing_timeout_sec = 0;
    //         draw_data[round][pilot].local_landing_timeout_usec = 0;
    //         draw_data[round][pilot].timedout = 0;
    //         draw_data[round][pilot].landing_dist = 0;
    //         draw_data[round][pilot].penalty_pts = 0;
    //         draw_data[round][pilot].raw_score = 0;
    //         vacio.toCharArray(draw_data[round][pilot].serialnumber, 12);
    //         vacio.toCharArray(draw_data[round][pilot].Firm_ver, 12);
    //     }
    // }
    Serial.println("Draw_data cleared: ");
}
void GTimer::clear_contest_pilot_list()
{
    memset(pilots_table, 0, sizeof(pilots_table));
    memset(group_pilots_table, 0, sizeof(group_pilots_table));

    // for (int pilot = 0; pilot < MAX_SELECTION_POINTER_PILOT; pilot++)
    // {
    //     pilots_table[pilot].pilot_id = 0;
    //     pilots_table[pilot].serialnumber[0] = '\0';
    //     pilots_table[pilot].serialnumber2[0] = '\0';
    //     pilots_table[pilot].serialnumber[0] = '\0';
    //     // pilots_table[pilot].name[0] = '\0';
    //     // pilots_table[pilot].nick_name[0] = '\0'; //TODO pilots name
    // }
}

void GTimer::create_dummy_draw_list() // this function is not really needed
{
    for (int round = 0; round < MAX_SELECTION_POINTER_ROUND; round++)
    {
        for (int pilot = 0; pilot < MAX_SELECTION_POINTER_PILOT; pilot++)
        {
            mycompetition.draw_list[round][pilot].pilot_id = pilot * 20;
            mycompetition.draw_list[round][pilot].FAI_score = round * 100;
            mycompetition.draw_list[round][pilot].round_rank = pilot;
            // mycompetition.draw_list[round][pilot].penalty = 255;
            mycompetition.draw_list[round][pilot].discarded_round = mycompetition.head.contestNum * 16 * 16 + round * 16 + pilot;
        }
    }
}

void GTimer::create_new_competition_from_menu()
{
    close_competition();
    clear_mycompetition();
    clear_draw_data();
    clear_draw_list();
    clear_contest_pilot_list();
    stored_competition_no++;
    mycompetition.head.contestNum = (uint16_t)stored_competition_no;
    mycompetition.head.data_version = DATA_VERSION;
    mycompetition.head.number_of_rounds = GTmenu[menupointer].new_menulevel;
    mycompetition.head.number_of_groups = 1;
    mycompetition.head.groups_enabled = groups;
    Serial.print("selected pilots ON CReate");
    Serial.println(selected_pilots);
    mycompetition.head.number_of_pilots = selected_pilots;
    mycompetition.head.flt_duration = flt_duration * 60;
    if (GLOBAL_DURATION > 0)
    {
        mycompetition.head.flt_duration = GLOBAL_DURATION;
    }
    mycompetition.head.draw_locked = 0xAAAAAABB; // TODO remove this
    stored_round_no = 0;
    current_round_no = stored_round_no;
    stored_record_num = 0;
    stored_competition_closed = 0;

    // clear_draw_list();
    // create_dummy_draw_list();

    competition.begin_competition(stored_competition_no);
    competition.save_competition_head(&mycompetition.head); // reserve the block in the directory
    // competition.save_competition(&mycompetition);         //TODO should be short save only head
    //  Serial.print("NEW COMPETITION");
    //  Serial.println(mycompetition.head.contestNum);
    //  Serial.print("Rounds: ");
    //  Serial.println(mycompetition.head.number_of_rounds);
    //  Serial.print("FAI: ");
    //  Serial.print(mycompetition.draw_list[1][2].FAI_score);
    //  Serial.print("number_of_pilots: ");
    //  Serial.println(mycompetition.head.number_of_pilots);
    //  Serial.println("New Comp stored in flash======================");
    print_draw_data();
    mycompetition.print_contest();
    GT.begin();
}

void GTimer::create_competition_from_file()
{
    stored_competition_no = competition.init();
    if (stored_competition_no > 2)
    {
        Serial.println("ESTAS EN LA MIERDA 2 jajajajaja!");
        return;
    }

    close_competition();
    stored_competition_no++;
    mycompetition.head.contestNum = stored_competition_no;
    mycompetition.head.data_version = DATA_VERSION;
    mycompetition.head.number_of_rounds = max_round_in_file;
    mycompetition.head.number_of_groups = max_group_in_file;
    mycompetition.head.number_of_pilots = max_pilot_number_in_file;
    mycompetition.head.groups_enabled = 1;
    // mycompetition.head.pilots_per_round = (uint32_t)((mycompetition.head.number_of_pilots - 1) / mycompetition.head.number_of_groups) + 1;
    mycompetition.head.landing_millis_allowance = 400;

    if (stored_competition_no == 2) /// this is the flyoff
    {
        Serial.println("Created por #2");
        mycompetition.head.flt_duration = 900;
        if (GLOBAL_DURATION > 0)
        {
            mycompetition.head.flt_duration = GLOBAL_DURATION;
        }
        mycompetition.head.number_of_rounds = 3;
        mycompetition.head.number_of_groups = 1;
        mycompetition.print_contest();
        Serial.println("=======================");
    }
    else // this is the competition with the groups in the file
    {
        Serial.println("Created por #1");
        mycompetition.head.flt_duration = 600;
        if (GLOBAL_DURATION > 0)
        {
            mycompetition.head.flt_duration = GLOBAL_DURATION;
        }
        if (mycompetition.head.number_of_rounds > GROUP_MAX_ROUND)
        {
            Serial.println("ESTAS EN LA MIERDA jajajajaja! se ha corrompido algo");
            mycompetition.head.number_of_rounds = 6;
            mycompetition.head.number_of_pilots = 5;
            mycompetition.head.number_of_groups = 1;
        }
        Serial.println("++++++++++++++++++++++++++");
        mycompetition.print_contest();
    }
    competition.begin_competition(stored_competition_no);
    competition.save_competition_head(&mycompetition.head);
    print_draw_data();
    mycompetition.print_contest();
    GT.begin();
}
void GTimer::manage_show_pilot_data()
{
    if (buttonA)
    {
        menupointer = 1;
        menulevel = 1;
        page_to_show = MENU_PAGE;
        GD.menu_displayUpdate(menupointer);
        buttonA = 0;
        // Serial.print(" Size of Contests ");
        // Serial.println(sizeof(struct Contests));
    }
    if (buttonB)
    {
        pilot_number++;
        if (pilot_number > mycompetition.head.number_of_pilots)
        {
            pilot_number = 1;
            // Serial.print("MenuPointer-- ");
        }
        buttonB = 0;
        GD.menu_display_list_pilot(pilot_number);
    }
    if (buttonC)
    {
        GD.zoom_level++;
        if (GD.zoom_level > 2)
        {
            GD.zoom_level = 0;
        };
        GD.menu_display_list_pilot(pilot_number);
        buttonC = 0;
    }
    // button_pressed=0;
    if (button_pressed)
    {
        // Serial.print("MenuPointer ");
        // Serial.print(menupointer);
        // Serial.print("MenuLevel ");
        // Serial.print(menulevel);
        // Serial.print(" stored_competition_no: ");
        // Serial.print(stored_competition_no);
        // Serial.print(" stored_round_no: ");
        // Serial.print(stored_round_no);
        // Serial.print(" stored_record_num: ");
        // Serial.print(stored_record_num);
        // Serial.print(" closed: ");
        // Serial.println(stored_competition_closed);
        button_pressed = 0;
    }
}

void GTimer::manage_show_error_data()
{ // not in use by the moment
    if (buttonA)
    {
        menupointer = 1;
        menulevel = 1;
        page_to_show = MENU_PAGE;
        GD.menu_displayUpdate(menupointer);
        buttonA = 0;
    }
    if (buttonB)
    {
        menupointer = 1;
        page_to_show = MENU_PAGE;
        menulevel = 1;
        GD.menu_displayUpdate(menupointer);
        buttonB = 0;
    }
    if (buttonC)
    {
        menupointer = 1;
        page_to_show = MENU_PAGE;
        menulevel = 1;
        GD.menu_displayUpdate(menupointer);
        buttonC = 0;
    }
    // button_pressed=0;
    if (button_pressed)
    {
        // Serial.print("MenuPointer ");
        // Serial.print(menupointer);
        // Serial.print("MenuLevel ");
        // Serial.print(menulevel);
        // Serial.print(" current_competition_no: ");
        // Serial.print(current_competition_no);
        // Serial.print(" current_round_no: ");
        // Serial.print(current_round_no);
        // Serial.print(" stored_competition_no: ");
        // Serial.print(stored_competition_no);
        // Serial.print(" stored_round_no: ");
        // Serial.print(stored_round_no);
        // Serial.print(" stored_record_num: ");
        // Serial.print(stored_record_num);
        // Serial.print(" closed: ");
        // Serial.println(stored_competition_closed);
        button_pressed = 0;
    }
}

void GTimer::manage_show_round_data()
{
    if (buttonA)
    {
        menupointer = 1;
        menulevel = 1;
        page_to_show = MENU_PAGE;
        GD.menu_displayUpdate(menupointer);
        buttonA = 0;
        // Serial.print(" Size of Contests ");
        // Serial.println(sizeof(struct Contests));
    }
    if (buttonB)
    {
        round_number++;
        if (round_number > mycompetition.head.number_of_rounds)
        {
            round_number = 0;
            // Serial.print("MenuPointer-- ");
        }
        buttonB = 0;
        GD.menu_display_list_round(round_number);
    }
    if (buttonC)
    {
        GD.zoom_level++;
        if (GD.zoom_level > 2)
        {
            GD.zoom_level = 0;
        };
        GD.menu_display_list_round(round_number);
        buttonC = 0;
    }
    // button_pressed=0;
    if (button_pressed)
    {
        // Serial.print("MenuPointer ");
        // Serial.print(menupointer);
        // Serial.print(" stored_competition_no: ");
        // Serial.print(stored_competition_no);
        // Serial.print(" stored_round_no: ");
        // Serial.print(stored_round_no);
        // Serial.print(" stored_record_num: ");
        // Serial.print(stored_record_num);
        // Serial.print(" closed: ");
        // Serial.println(stored_competition_closed);
        button_pressed = 0;
    }
}

void GTimer::manage_error(int error_code)
{
    menulevel = error_code;
    page_to_show = MENU_PAGE;
    FLT_mode = PATATA;
    perform_menu_action(ACTION_MENU);
}

void GTimer::manage_management()

{
    if (buttonA)
    {
        buttonA = 0;
        // competition.move_block();

        menupointer = 1;
        menulevel = 1;
        GD.menu_displayUpdate(menupointer);
    }
    if (buttonB)
    {
        menupointer++;
        if ((menupointer >= MAX_MENUPOINTER) || (GTmenu[menupointer].menulevel != menulevel))
        {
            menupointer--;
            // Serial.println("MenuPointer-- ");
            while (GTmenu[menupointer - 1].menulevel == menulevel)
            { // JHR no te olvides en tu vida == dentro de un IF
                menupointer--;
            }
        }
        buttonB = 0;
        GD.menu_displayUpdate(menupointer);
    }
    if (buttonC)
    {
        menulevel = GTmenu[menupointer].new_menulevel;
        if ((menulevel > 10000) && (menulevel < 20000))
        {
            flt_duration = menulevel % 100;
            menulevel = (menulevel - flt_duration) / 100;
            // Serial.print("flt_duration ");
            // Serial.print(flt_duration);
            // Serial.print(" menulevel ");
            // Serial.print(menulevel);
        }
        else
        {
            if (menulevel > 20000)
            {
                selected_pilots = menulevel % 100;
                menulevel = (menulevel - selected_pilots) / 100;
                menulevel = menulevel - 100;
                Serial.print("selected pilots ");
                Serial.print(selected_pilots);
                Serial.print(" menulevel ");
                Serial.print(menulevel);
            }
            else
            {
                if (menulevel > 1000)
                {
                    Serial.print(" menulevel ");
                    Serial.println(menulevel);
                    selected_pilots = menulevel % 10;
                    menulevel = (menulevel - selected_pilots) / 10;
                    Serial.print("selected pilots ");
                    Serial.print(selected_pilots);
                    Serial.print(" menulevel ");
                    Serial.print(menulevel);
                }
                else
                {
                    Serial.print("selected pilots ");
                    Serial.print(selected_pilots);
                    Serial.print(" menulevel ");
                    Serial.print(menulevel);
                }
            }
        }
        perform_menu_action(GTmenu[menupointer].action);
        buttonC = 0;
    }
    if (button_pressed)
    {
        // Serial.print("MenuPointer ");
        // Serial.print(menupointer);
        // Serial.print("MenuLevel ");
        // Serial.print(menulevel);
        // Serial.print(" current_competition_no: ");
        // Serial.print(current_competition_no);
        // Serial.print(" current_round_no: ");
        // Serial.print(current_round_no);
        // Serial.print(" stored_competition_no: ");
        // Serial.print(stored_competition_no);
        // Serial.print(" stored_round_no: ");
        // Serial.print(stored_round_no);
        // Serial.print(" stored_record_num: ");
        // Serial.print(stored_record_num);
        // Serial.print(" closed: ");
        // Serial.println(stored_competition_closed);
        button_pressed = 0;
    }
}

void GTimer::perform_menu_action(uint32_t action_code)
{
    // Serial.println("Boton C pisado!!");
    switch (action_code)
    {
    case ACTION_MENU:                               // Jump to 'new_menulevel'
        if ((menulevel == 114) && (groups_enabled)) // we break menu to crete new competition
        {
            create_competition_from_file();
            menupointer = 1;
            menulevel = GTmenu[menupointer].menulevel;
            GD.menu_displayUpdate(menupointer);
        }
        if ((menulevel == 11) && (groups_enabled))
        {
            GD.display_status();
        }

        for (int j = 0; j < (MAX_MENUPOINTER + 1); j++)
        {

            // Serial.println(j);
            if (GTmenu[j].menulevel == menulevel)
            {
                menupointer = j;
                break;
            }
            if (j == MAX_MENUPOINTER)
            {
                menupointer = 1;
                break;
            }
        }
        GD.menu_displayUpdate(menupointer);
        // Serial.print("MenuPointerC ");Serial.print(menupointer); Serial.print(" menu[level] ");Serial.print(GTmenu[menupointer].menulevel); Serial.print(" menulevel ");Serial.print(menulevel);Serial.println(" Display Action Menu") ;
        break;

    case ACTION_LIST_ROUND:
    {
        Serial.print("MenuPointer ");
        Serial.print(menupointer);
        Serial.print(" Round # ");
        Serial.print(GTmenu[menupointer].new_menulevel);
        Serial.println(" Display Action ");
        round_number = GTmenu[menupointer].new_menulevel;

        // GD.menu_displayUpdate(menupointer);
        page_to_show = ROUND_PAGE;
        GD.menu_display_list_round(round_number);
    }
    break;
    case ACTION_LIST_PILOT:
    {
        // Serial.print("MenuPointer ");
        // Serial.print(menupointer);
        // Serial.print(" Round # ");
        // Serial.print(GTmenu[menupointer].new_menulevel);
        // Serial.println(" Display Action ");
        pilot_number = GTmenu[menupointer].new_menulevel;

        // GD.menu_displayUpdate(menupointer);
        page_to_show = PILOT_PAGE;
        GD.menu_display_list_pilot(pilot_number);
    }
    break;

    case ACTION_CLOSE_COMPETITION:
    {
        // Serial.print("Close competition ");

        mycompetition.close_competition();
        delay(300); /// TODO announce something in the display
        GT.begin();

        menupointer = 3;
        menulevel = GTmenu[menupointer].menulevel;
        GD.menu_displayUpdate(menupointer);
    }
    break;
    case ACTION_ERASE_COMP:
    {
        // Serial.print("Erase competition ");

        bool exito = competition.format();
        GT.begin();
        // Serial.println(exito);
        menupointer = 3;
        menulevel = GTmenu[menupointer].menulevel;
        GD.menu_displayUpdate(menupointer);
    }
    break;

    case ACTION_NEW_COMP:
    {
        // Serial.println("NEW COMPETITION");
        create_new_competition_from_menu();
        menupointer = 1;
        menulevel = GTmenu[menupointer].menulevel;
        GD.menu_displayUpdate(menupointer);
    }
    break;
    case ACTION_SELECT_COMPETITION:
    {
        // Serial.println("SELECT COMPETITION ");
        current_competition_no = GTmenu[menupointer].new_menulevel;
        load_stored_competition(current_competition_no);
        menupointer = 3;
        menulevel = GTmenu[menupointer].menulevel;
        GD.menu_displayUpdate(menupointer);
    }
    break;
    case ACTION_RUN_FLIGHT:
    {
        // Serial.printf("Flight in %d1 minutes \n", GTmenu[menupointer].new_menulevel);
        if (all_scores_in_round())
        {
            Serial.println("FLIght start");
            flight.start(GTmenu[menupointer].new_menulevel);
        }
        else
        {
            Serial.println("FLT_mode = RUN_FLIGHT_REQUEST");
            FLT_mode = RUN_FLIGHT_REQUEST;
        }
        // menupointer = 3;
        // menulevel = GTmenu[menupointer].menulevel;
        // GD.menu_displayUpdate(menupointer);
    }
    break;
    case ACTION_SCAN_AP:
    {

        menulevel = GTmenu[menupointer].new_menulevel;
        for (int j = 0; j < (MAX_MENUPOINTER + 1); j++)
        {

            // Serial.println(j);
            if (GTmenu[j].menulevel == menulevel)
            {
                menupointer = j;
                break;
            }
            if (j == MAX_MENUPOINTER)
            {
                menupointer = 1;
                break;
            }
        }
        pick_pointer = menupointer;
        String label = "Scanning";
        label.toCharArray(GT.GTmenu[GT.pick_pointer + 0].menulabel, 24);
        ssid_found[0] = "";
        label = "...";
        label.toCharArray(GT.GTmenu[GT.pick_pointer + 1].menulabel, 24);
        ssid_found[1] = "";
        label.toCharArray(GT.GTmenu[GT.pick_pointer + 2].menulabel, 24);
        ssid_found[2] = "";
        label = "Choose one id";
        label.toCharArray(GT.GTmenu[GT.pick_pointer + 3].menulabel, 24);

        FLT_mode = SCAN_AP;
        action_code = ACTION_MENU;
        menupointer = pick_pointer;
        GD.menu_displayUpdate(menupointer);
    }
    break;
    case ACTION_SELECT_AP:
    {
        selectionAP = GTmenu[menupointer].new_menulevel;
        String ssname = ssid_found[selectionAP].substring(0, 12);
        // Serial.print("******************** ssname  " + ssname);

        if ((ssid_found[selectionAP].length() > 4) && (selectionAP < 3))
        // if (ssname == "GliderKeeper")
        {
            FLT_mode = PICK_DATA; /* code */
        }
        else
        {
            if (GTmenu[menupointer].menulabel[0] == 'S')
            {
                String label = "Scanning";
                label.toCharArray(GT.GTmenu[GT.pick_pointer + 0].menulabel, 24);
                ssid_found[0] = "";
                label = "...";
                label.toCharArray(GT.GTmenu[GT.pick_pointer + 1].menulabel, 24);
                ssid_found[1] = "";
                label.toCharArray(GT.GTmenu[GT.pick_pointer + 2].menulabel, 24);
                ssid_found[2] = "";
                label = "Choose one id";
                label.toCharArray(GT.GTmenu[GT.pick_pointer + 3].menulabel, 24);
                FLT_mode = SCAN_AP;
                action_code = ACTION_MENU;
                menupointer = pick_pointer;
                menulevel = GTmenu[menupointer].menulevel;
                GD.menu_displayUpdate(menupointer);
            }
            else
            {

                String label = "Not valid sel.";
                label.toCharArray(GT.GTmenu[GT.pick_pointer + 3].menulabel, 24);
                FLT_mode = PATATA;
                action_code = ACTION_MENU;
                menupointer = pick_pointer;
                menulevel = GTmenu[menupointer].menulevel;
                GD.menu_displayUpdate(menupointer);
            }
            break;
        }
    }
    break;
    case ACTION_VOLUME:
    {
        flight.update_volume(GTmenu[menupointer].new_menulevel);
        menulevel = 1;
        menupointer = 1;
        flight.bip();
        GD.menu_displayUpdate(menupointer);
    }
    break;
    case ACTION_BRIGHTNESS:
    {
        flight.update_brightness(GTmenu[menupointer].new_menulevel);
        menulevel = 1;
        menupointer = 1;
        GD.menu_displayUpdate(menupointer);
    }
    break;
    case ACTION_GROUP_TOGGLE:
    {
        if (stored_competition_no == 0)
        {
            groups_enabled = !groups_enabled;
            flight.update_groups(groups_enabled);
            flight.read_settings();
            update_setting_labels();
            GD.menu_displayUpdate(menupointer);
            GD.clean_partial_flt();
            GD.alabel = "";
            GD.blabel = "unit will reset...";
            GD.partial_flt();
            menulevel = 1;
            menupointer = 1;
            delay(2000);
            ESP.restart();
            GD.menu_displayUpdate(menupointer);
        }
        else
        {
            menulevel = 1;
            menupointer = 1;
            firstime = 1;
            FLT_mode = NEED_FORMAT;
        }
    }
    break;
    case ACTION_COLLECTOR_TOGGLE:
    {
        if (stored_competition_no == 0)
        {
            collector = !collector;
            flight.update_collector(collector);
            flight.read_settings();
            update_setting_labels();
            GD.menu_displayUpdate(menupointer);
            GD.clean_partial_flt();
            GD.alabel = "";
            GD.blabel = "unit will reset...";
            GD.partial_flt();
            menulevel = 1;
            menupointer = 1;
            delay(2000);
            ESP.restart();
            GD.menu_displayUpdate(menupointer);
        }
        else
        {
            menulevel = 1;
            menupointer = 1;
            firstime = 1;
            FLT_mode = NEED_FORMAT;
        }
    }
    break;
    case ACTION_SCHEDULED_TOGGLE:
    {
        if (stored_competition_no == 0)
        {
            scheduled = !scheduled;
            flight.update_scheduled(scheduled);
            flight.read_settings();
            update_setting_labels();
            GD.menu_displayUpdate(menupointer);
            GD.clean_partial_flt();
            GD.alabel = "";
            GD.blabel = "unit will reset...";
            GD.partial_flt();
            menulevel = 1;
            menupointer = 1;
            delay(2000);
            ESP.restart();
            GD.menu_displayUpdate(menupointer);
        }
        else
        {
            menulevel = 1;
            menupointer = 1;
            firstime = 1;
            FLT_mode = NEED_FORMAT;
        }
    }
    break;
    case ACTION_COLLECTOR_OFF:
    {
        flight.update_collector(0);
        menulevel = 1;
        menupointer = 1;
        GD.menu_displayUpdate(menupointer);
    }
    break;

    case ACTION_ABOUT:
    {
        FLT_mode = ACTION_ABOUT_MODE;
        // if (firstime)
        // {

        //     GD.about_screen();
        //     firstime = 0;
        // }
        // if (buttonA)
        // {
        //     back_to_prev_status();
        //     buttonC = 0;
        // }
        // if (buttonB)
        // {
        //     buttonB = 0;
        // }
        // if ((buttonC))
        // {
        //     back_to_prev_status();
        //     buttonC = 0;
        // }
    }
    break;
    }
}

void GTimer::manage_wifi_input()
{
    static bool timedout = 0;
    // static bool firstime = 1;
    static uint distance = 0;
    const uint16_t distancias[] = {
        0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20,
        30, 40, 50, 60, 70, 80, 90,
        100, 110, 120, 130, 140, 150};
    const uint16_t number_points = 24;

    const char *distancias_C[] = {
        "0"
        "0.2",
        "0.4",
        "0.6",
        "0.8",
        "1.0",
        "1.2",
        "1.4",
        "1.6",
        "1.8",
        "2.0",
        "3.0",
        "4.0",
        "5.0",
        "6.0",
        "7.0",
        "8.0",
        "9.0",
        "10",
        "11",
        "12",
        "13",
        "14",
        "15"};
    

    switch (FLT_mode)
    {
    case NEW_DATA:
        firstime = 1;
        if (receiving_working_time)
        {
            FLT_mode = GT.confirm_for_upload_or_new_launch(); // will return CONFIRM_WORKING_TIME or CONFIRM_TFD_UPLOAD
        }
        else // we have a real flight here
        {
            if (received_from_GTimer)
            {
                FLT_mode = RECEIVED_FROM_TIMER;
                Serial.print("received from TIMER****************FLT MODE= ");
                Serial.println(FLT_mode);
                GT.received_from_GTimer = 0;
            }
            else
            {
                FLT_mode = assign_pilot_and_round(FLTdata.tfd); // includes check_landing_moment
                Serial.print("received from keeper****************FLT MODE= ");
                Serial.println(FLT_mode);
            }
        }
        GD.display_fltdata(FLTdata.tfd);
        FLTdata.print_tfd(FLTdata.tfd);
        break;
    case SUCCESS:
        if (firstime)
        {
            GD.clean_partial_flt();
            GD.alabel = "  dist? " + String(distance);
            // GD.blabel = "   GK Fw: " + String(FLTdata.tfd.Firm_ver);
            GD.partial_flt();
            firstime = 0;
        }
        if (buttonA)
            buttonA = 0;

        if (buttonB)

        {
            GD.clean_partial_flt();
            distance++;
            if (distance > 76)
            {
                distance = 0;
            }
            if (distance > 10)
            {
                distance = 76;
            }
            GD.alabel = "  dist? " + String(distance);
            // GD.blabel = "   GK Fw: " + String(FLTdata.tfd.Firm_ver);
            GD.partial_flt();
            buttonB = 0;
        }
        if (buttonC)
        { // xyz
            FLTdata.set_landing_dist(distance);
            GD.clean_partial_flt();
            GD.alabel = "Recording....";
            GD.partial_flt();
            delay(200);
            // Serial.println("updating record");
            save_record_and_update_competition(2);
            // Serial.println("printing tfd");
            FLTdata.print_tfd(FLTdata.tfd);
            // Serial.println("going to prev status?");
            back_to_prev_status();
            // Serial.println("updatED record");

            buttonC = 0;
        }
        break;
    case INVALID:
        if (firstime)
        {
            timedout = 1;

            GD.blabel = " Score=0 ,press sel. ";
            GD.partial_flt();
            flight.boop();
            FLTdata.set_timedout(timedout);
            FLTdata.set_landing_dist(76);
            firstime = 0;
        }
        if (buttonB)
        {
            buttonB = 0;
        }
        if ((buttonC))
        {
            GD.clean_partial_flt();
            GD.alabel = "Recording....";
            GD.partial_flt();
            delay(200);
            save_record_and_update_competition(0);
            back_to_prev_status();
            buttonC = 0;
        }
        break;
    case TIMED_OUT:
        if (firstime)
        {
            GD.clean_partial_flt();
            timedout = 1;
            // flight.boop();
            GD.alabel = " Timeout: dist=0 ";
            GD.blabel = "pls,ack.    Select";
            GD.partial_flt();
            FLTdata.set_timedout(timedout);
            FLTdata.set_landing_dist(0);
            firstime = 0;
        }
        if (buttonB)
        {
            buttonB = 0;
        }
        if ((buttonC))
        {
            GD.clean_partial_flt();
            GD.alabel = "Recording....";
            GD.partial_flt();
            delay(200);
            /// TODO Adjust flight time!!!
            save_record_and_update_competition(1);
            // FLT_mode = PATATA;
            // menupointer = 3;
            // menulevel = GTmenu[menupointer].menulevel;
            // GD.menu_displayUpdate(menupointer);
            back_to_prev_status();
            buttonC = 0;
        }
        break;

    case REPEATED_RECORD:
        if (firstime)
        {
            GD.clean_partial_flt();
            timedout = 1;
            // GD.alabel = " Duplicated rcrd.";
            // GD.blabel = " FLTnum or pilot";
            GD.partial_flt();
            firstime = 0;
        }
        if (buttonB)
        {
            buttonB = 0;
        }

        if ((buttonC))
        {
            // FLT_mode = PATATA;
            // menupointer = 3;
            // menulevel = GTmenu[menupointer].menulevel;
            // GD.menu_displayUpdate(menupointer);
            // buttonC = 0;
            back_to_prev_status();
            buttonC = 0;
        }

        break;
    case PILOT_NOT_FOUND:
        if (firstime)
        {
            GD.clean_partial_flt();
            timedout = 1;
            GD.alabel = "Not eligible";
            GD.blabel = "Compet.full:";
            GD.partial_flt();
            firstime = 0;
        }
        if (buttonB)
        {
            buttonB = 0;
        }

        if ((buttonC))
        {
            // FLT_mode = PATATA;
            // menupointer = 3;
            // menulevel = GTmenu[menupointer].menulevel;
            // GD.menu_displayUpdate(menupointer);
            // buttonC = 0;
            back_to_prev_status();
            buttonC = 0;
        }
        break;
    case NOT_IN_GROUP:
        if (firstime)
        {
            GD.clean_partial_flt();
            timedout = 1;
            GD.alabel = "Not eligible";
            GD.blabel = "NOT IN GROUP";
            GD.partial_flt();
            firstime = 0;
        }
        if (buttonB)
        {
            buttonB = 0;
        }
        if ((buttonC))
        {
            back_to_prev_status();
            buttonC = 0;
        }
        break;
    case COMPETITION_CLOSED:
        if (firstime)
        {
            GD.clean_partial_flt();
            timedout = 1;
            GD.alabel = "Closed";
            GD.blabel = "Competition";
            GD.partial_flt();
            firstime = 0;
        }
        if (buttonB)
        {
            buttonB = 0;
        }

        if ((buttonC))
        {
            // FLT_mode = PATATA;
            // menupointer = 3;
            // menulevel = GTmenu[menupointer].menulevel;
            // GD.menu_displayUpdate(menupointer);
            back_to_prev_status();
            buttonC = 0;
        }
        break;
    case RECORD_EXISTS:
        if (firstime)
        {
            GD.clean_partial_flt();
            GD.alabel = " Rec. Overwrite?";
            timedout = 0;
            // GD.blabel = "this rnd: Fw" + String(FLTdata.tfd.Firm_ver);
            GD.partial_flt();
            firstime = 0;
        }
        if (buttonB)
        {
            buttonB = 0;
        }

        if (buttonC)
        {
            GD.clean_partial_flt();
            GD.alabel = "OverWriting....";
            GD.partial_flt();
            FLT_mode = SUCCESS;
            delay(150);
            firstime = 1;
            buttonC = 0;
        }

        break;
    case UNKNOWN:
        if (firstime)
        {
            GD.display_fltdata(FLTdata.tfd);
            GD.clean_partial_flt();
            // GD.alabel = "Timeout ?: NO";
            // GD.alabel = "OnWtime ?: YES";
            GD.alabel = "OnWorkTime?YES";
            timedout = 0;
            GD.blabel = "scroll and confirm:";
            GD.partial_flt();
            firstime = 0;
        }
        if (buttonB)
        {
            GD.clean_partial_flt();
            timedout = !timedout;
            if (timedout)
            {
                // GD.alabel = "Timeout ?: YES";
                //  GD.alabel = "OnWtime ?: NO ";
                GD.alabel = "OnWorkTime? NO";
            }
            else
            {
                // GD.alabel = "Timeout ?: NO";
                GD.alabel = "OnWorkTime?YES";
            }
            GD.partial_flt();
            buttonB = 0;
        }
        if (buttonC)
        {
            if (timedout)
            {
                FLTdata.set_timedout(timedout);
                GD.clean_partial_flt();
                GD.alabel = "Recording....";
                GD.partial_flt();
                save_record_and_update_competition(1);
                delay(200);
                // FLT_mode = PATATA;
                // menupointer = 3;
                // menulevel = GTmenu[menupointer].menulevel;
                // firstime = 1;
                // GD.menu_displayUpdate(menupointer);
                back_to_prev_status();
            }
            else
            {
                FLTdata.set_timedout(timedout);
                FLT_mode = SUCCESS;
                firstime = 1;
            }
            buttonC = 0;
        }
        break;
    case CONFIRM_WORKING_TIME:
        if (firstime)
        {
            GD.clean_partial_flt();
            GD.alabel = " Run This Flight?";
            // GD.blabel = "   GK Fw: " + String(FLTdata.tfd.Firm_ver);
            GD.partial_flt();
            firstime = 0;
        }
        if (buttonA)
        {
            receiving_working_time = 0;
            back_to_prev_status();
            buttonA = 0;
        }
        if (buttonB)

        {

            buttonB = 0;
        }
        if (buttonC)
        {

            GD.firsttime_run = 1;

            receiving_working_time = 0;
            sinWifi = 1;
            flight.start(0);
            Serial.println("start from button C; confirm working time");
            GT.back_to_prev_status();
            buttonC = 0;
        }
        break;
    }

    if (!firstime)
    {
        GD.display_toggle_name_in_keeper();
    }

    if (buttonA)
    {
        back_to_prev_status();
    }
    if (button_pressed)
    {
        button_pressed = 0;
    }
}

void GTimer::clear_buttons()
{

    if (buttonA)
    {
        buttonA = 0;
    }
    if (buttonB)
    {
        buttonB = 0;
    }
    if (buttonC)
    {
        buttonC = 0;
    }
    if (button_pressed)
    {
        button_pressed = 0;
    }
}

void GTimer::back_to_prev_status()
{
    menupointer = 1;
    menulevel = GTmenu[menupointer].menulevel;
    FLT_mode = PATATA;
    if (flight.in_flight_status)
    {
        Serial.print("I am in flight mode");
        GD.clear_display();
        GD.primeravez = 1;
        GD.last_prenotice = 1;
        firstime = 1;
        GD.manage_timer_display();
    }
    else
    {
        Serial.print("I am in MENU mode");
        GD.clear_display();
        GD.primeravez = 1;
        GD.last_prenotice = 1;
        firstime = 1;
        // if (scheduled){
        //      buttonA = 0;
        //       GD.displayUpdateError("BACKled ON");
        //       return;

        // }

        GD.menu_displayUpdate(menupointer);
    }
    buttonA = 0;
}

void GTimer::save_record_and_update_competition(int landing_case)
{
    if (new_pilot_added)
    {
        Serial.println("newpilot in comp");
        new_pilot_added = 0;
        int pilot_id_contest = new_pilot_in_pilots_table();
        Serial.printf("newpilot in pilots table : %d \n", pilot_id_contest);

        // pilots_table[0].pilot_id = pilots_table[FLTdata.tfd.pilot_no].pilot_id;
        registered_pilots++;
    }
    calculate_raw_score(landing_case);
    // Serial.println("trazando xxx");
    save_record();
}

void GTimer::save_record()
{
    stored_record_num++;
    FLTdata.tfd.record_num = stored_record_num; // update draw data and save
    if (groups_enabled)
    {
        group_draw_data_pointer++;
        if (group_draw_data_pointer == MAX_GROUP_DRAW_DATA)
        {
            group_draw_data_pointer = 1;
        }
        group_draw_data[group_draw_data_pointer] = FLTdata.tfd;
    }
    else
    {
        draw_data[FLTdata.tfd.round_no][FLTdata.tfd.pilot_no] = FLTdata.tfd;
        for (int roundk = 0; roundk < MAX_SELECTION_POINTER_ROUND; roundk++)
        {
            mycompetition.draw_list[roundk][FLTdata.tfd.pilot_no].pilot_id = FLTdata.tfd.pilot_id;
        }
    }
    if (competition.save_record(&FLTdata.tfd)) // FLTdata.tfd is stored in partition
    {
        // Serial.print("tfd Saved with competition  ");
        // Serial.print(stored_competition_no);
        // Serial.print(" ; record#  ");
        // Serial.print(stored_record_num);
        // Serial.print(" and round   ");
        // Serial.println(stored_round_no);
    }
    else
    {
        // Serial.println("Problems: saving record not possible");
    }
    FLTdata.print_tfd(FLTdata.tfd); // TODO esto quitar
    if (!groups_enabled)            // TODO Pending or to be fully deprecated
    {
        mycompetition.calculate_scores();
    }
    // check_for_end_round();
    if (stored_round_no > mycompetition.head.number_of_rounds)
    {
        competition.save_competition(&mycompetition);
    }
}

void GTimer::calculate_raw_score(int landing_case)
{
    FLTdata.tfd.raw_score = (int32_t)(FLTdata.tfd.landing_millis - FLTdata.tfd.take_off_millis) / 1000;
    // Serial.print("Raw Score Calculated");
    // Serial.println(FLTdata.tfd.raw_score);
    uint32_t f5jh = FLTdata.tfd.F5J_height / 10;
    uint32_t substract = 0;
    if (f5jh < 200) // por altura
    {
        substract = (int32_t)(f5jh / 2);
    }
    else
    {
        substract = (int32_t)(3 * ((uint32_t)f5jh - 200) + 100);
    }

    if (landing_case == 2) // 2 is success, 1 is timeout and 0 is out of working time
    {
        if ((FLTdata.tfd.landing_dist < 11) && (FLTdata.tfd.landing_dist > 0)) // por aterrizaje
        {
            FLTdata.tfd.raw_score = FLTdata.tfd.raw_score + 50 - 5 * (FLTdata.tfd.landing_dist - 1);
        }
        // Serial.print("Raw Score Calculated");
        // Serial.println(FLTdata.tfd.raw_score);

        if (FLTdata.tfd.landing_dist > 75) // por fuera de campo
        {
            FLTdata.tfd.raw_score = 0;
        }
    }
    if (FLTdata.tfd.raw_score > substract)
    {
        FLTdata.tfd.raw_score = FLTdata.tfd.raw_score - substract;
    }
    else
    {
        FLTdata.tfd.raw_score = 0;
    }
    if (landing_case == 0)
    {
        FLTdata.tfd.raw_score = 0;
    }
    // Serial.print("Raw Score Calculated");
    // Serial.println(FLTdata.tfd.raw_score);
}

uint32_t GTimer::assign_pilot_and_round(struct datastore tfd)
{
    if (stored_competition_closed)
    {
        return COMPETITION_CLOSED;
    }
    // find the serialnum in the pilots table
    new_pilot_added = 0;
    uint32_t new_candidate_id = find_pilot_in_table(tfd.serialnumber); // piloto en lista
    uint32_t new_candidate_no = 0;
    Serial.printf("looking for pilot in the contest %s ID() was %d \n", tfd.serialnumber, new_candidate_id);
    // find out this serial number´s bib number (dorsal) and in case !groups_enabled add it if not found
    if (new_candidate_id == PILOT_NOT_FOUND_FLAG)
    {
        Serial.printf("pilot not found in the pilot list in the contest %s \n", tfd.serialnumber);
        if (groups_enabled)
        {
            return PILOT_NOT_FOUND;
        }
        else
        {
            if (registered_pilots < mycompetition.head.number_of_pilots)
            {
                new_pilot_added = 1; // se va a añadir piloto si se graba
                // registered_pilots++;
                new_candidate_no = registered_pilots + 1;
                new_candidate_id = pilots_table[0].pilot_id + 1;
                Serial.printf("( New pilot in list & space in comp)pilot can be added to the contest %s \n", tfd.serialnumber);
            }
            else
            {
                return PILOT_NOT_FOUND;
            }
        }
    }
    else
    {
        if (groups_enabled)
        {
            new_candidate_no = find_pilot_ID_in_table(new_candidate_id); // mumero de piloto de la tabla
            // en caso de grupos este numero coincide con el de la tabla de pilotos
        }
        else
        {
            new_candidate_no = find_pilot_ID_in_nongroup_competion(new_candidate_id);
            if (new_candidate_no == 0) /// pilot en lista NO en competicion
            {
                if (registered_pilots < mycompetition.head.number_of_pilots) // si hay sitio en competicion
                {
                    new_candidate_no = registered_pilots + 1;
                    Serial.printf(" NEW KNown pilot can be added to the contest %s \n", FLTdata.tfd.serialnumber);
                }
                else
                {
                    return PILOT_NOT_FOUND;
                }
            }
        }
    }
    // Serial.printf("stored group %d draw group %d grenabled %d\n",stored_group_no,mycompetition.draw_list[stored_round_no][pilot_number].group,groups_enabled);

    if (groups_enabled) /// check if in good group
    {
        // Serial.printf("stored group %d draw group %d grenabled %d\n",stored_group_no,mycompetition.draw_list[stored_round_no][new_candidate_no].group,groups_enabled);

        if (group_draw_list[stored_round_no][new_candidate_no].group != (uint8_t)stored_group_no)
        {
            Serial.println("Pilot does not match Group !!!!!!!!!!!!!!!!!!");
            Serial.printf("round is: %dStored group is: %d pilots group is %d\n", stored_round_no, stored_group_no, mycompetition.draw_list[stored_round_no][new_candidate_no].group);
            GD.alabel = " Pilot not in ";
            GD.blabel = " this group";
            return REPEATED_RECORD;
        }
    }

    FLTdata.tfd.comp_no = stored_competition_no;
    FLTdata.tfd.round_no = stored_round_no;
    FLTdata.tfd.group_no = stored_group_no;
    FLTdata.tfd.pilot_no = new_candidate_no;
    FLTdata.tfd.pilot_id = new_candidate_id;
    // datos del timer
    serialnumber.toCharArray(FLTdata.tfd.timer_serialnumber, 12);
    versionkeeper.toCharArray(FLTdata.tfd.timer_Firm_ver, 12);
    if (!groups_enabled) /// TODO comprobar si se puede hacer en grupos tambien, ya está tomado ese vuelo
    {
        if (draw_data[stored_round_no][new_candidate_no].fltnum != 0)
        {
            GD.alabel = " Duplicated rcrd.";
            GD.blabel = " Record exists";
            return REPEATED_RECORD;
        }
    }

    if (!groups_enabled) /// Ya se ha usa do este vuelo en otra ronda
    {
        for (int j = 1; j < stored_round_no; j++)
        {
            if ((FLTdata.tfd.fltnum == draw_data[j][new_candidate_no].fltnum) && (strcmp(FLTdata.tfd.serialnumber, draw_data[j][new_candidate_no].serialnumber) == 0))
            {
                // Serial.println("return REPEATED_RECORD;");
                GD.alabel = " Duplicated rcrd.";
                GD.blabel = " FLTnumber used";
                return REPEATED_RECORD;
            }
        }
    }
    int landing_result = FLTdata.check_landing_moment();
    return landing_result;
}

uint32_t GTimer::assign_pilot_and_round(struct datastore tfd, uint32_t group_no)
{
}

uint32_t GTimer::find_pilot_in_table(char serial_num[12]) /// look for in pilots_table
{
    uint32_t Serial_NUM = char2_SN(serial_num);
    Serial.println("========================================================");
    Serial.printf("Looking for s/n: %s short: %X \n", serial_num, Serial_NUM);
    Serial.println("========================================================");

    int MAX_pilot = MAX_SELECTION_POINTER_PILOT;
    if (groups_enabled)
    {
        MAX_pilot = GROUP_MAX_SELECTION_POINTER_PILOT;
    }

    for (int table_piloti = 1; table_piloti <= MAX_pilot; table_piloti++)
    {
        uint32_t serialNUMBER;
        uint32_t serialNUMBER2;
        uint32_t serialNUMBER3;
        if (groups_enabled)
        {
            serialNUMBER = group_pilots_table[table_piloti].serialnumber;
            serialNUMBER2 = group_pilots_table[table_piloti].serialnumber2;
            serialNUMBER3 = group_pilots_table[table_piloti].serialnumber3;
        }
        else
        {
            serialNUMBER = char2_SN(pilots_table[table_piloti].serialnumber);
            serialNUMBER2 = char2_SN(pilots_table[table_piloti].serialnumber2);
            serialNUMBER3 = char2_SN(pilots_table[table_piloti].serialnumber3);
        }
        Serial.printf("Pilot in Comp# %d id 1: (%X) id2: (%X)  id3: (%X) \n", table_piloti, serialNUMBER, serialNUMBER2, serialNUMBER3);

        if ((serialNUMBER == Serial_NUM) || (serialNUMBER2 == Serial_NUM) || (serialNUMBER3 == Serial_NUM))
        {
            if (groups_enabled)
            {
                Serial.println("\n######################################################");

                Serial.printf("FOUND!*****, Global ID  %d In This competition %d ***\n###################################################### \n", group_pilots_table[table_piloti].pilot_id, table_piloti);

                return group_pilots_table[table_piloti].pilot_id;
            }
            else
            {
                Serial.printf("FOUND  %d ================================================== \n", pilots_table[table_piloti].pilot_id);
                return pilots_table[table_piloti].pilot_id;
            }
        }
    }
    Serial.println("NOT FOUND  :(========================================================");
    return PILOT_NOT_FOUND_FLAG;
}

int32_t GTimer::find_pilot_ID_in_table(uint32_t checkd_id)
{

    for (uint32_t candidate = 1; candidate < (GROUP_MAX_SELECTION_POINTER_PILOT); candidate++)
    {

        if (checkd_id == group_pilots_table[candidate].pilot_id)
        {
            // Serial.print("pilot found");
            // Serial.println(candidate);
            return candidate; /// pilot number
        }
        else
        {
        }
    }
    Serial.printf(" ID: %d not found in pilots_table \n", checkd_id);
    return -1;
}

uint32_t GTimer::add_pilot_in_table(uint16_t pilot_id) // returns the bib/pilot number of the id if not avalable is stored as new
{
    // for (uint8_t candidate = 1; candidate < (mycompetition.head.number_of_pilots + 1); candidate++)
    // {
    //     // Serial.println("X6");
    //     if (pilot_id_contest[candidate] == pilot_id)
    //     {
    //         return candidate;
    //     }
    //     if (pilot_id_contest[candidate] == 0)
    //     {
    //         pilot_id_contest[candidate] = pilot_id;
    //         return candidate;
    //     }
    // }
    // Serial.println("ERROR TOO MANY PILOTS IN DRAW");
    // return (mycompetition.head.number_of_pilots + 1);
}

void GTimer::check_for_end_round() // this function is deprecated
{
    bool round_complete = 1;
    for (uint8_t candidate = 1; candidate < (mycompetition.head.number_of_pilots + 1); candidate++)
    {
        if (mycompetition.draw_list[stored_round_no][candidate].group == stored_group_no)
        {
            round_complete = round_complete & (draw_data[stored_round_no][candidate].fltnum != 0);
        }
    }
    if (round_complete)
    {
        Serial.print("--Round has got all records, new round: ?? ");

        // Serial.println(stored_round_no);
    }
    if (stored_round_no > mycompetition.head.number_of_rounds)
    {
        mycompetition.close_competition();
        stored_round_no = stored_round_no - 1;
    }
    // else
    // {
    //     stored_round_no = stored_round_no - 1;
    //     // Serial.print("--Round number is now: ");
    //     // Serial.println(stored_round_no);
    // }
}

void GTimer::button_update()
{
    M5.update();
    if (M5.BtnA.wasPressed())
    {
        // Serial.println("Button A was Pressed");
        button_pressed = 1;
        buttonA = 1;
    }
    if (M5.BtnB.wasPressed())
    {
        // Serial.println("Button B was Pressed");
        button_pressed = 1;
        buttonB = 1;
    }
    if (M5.BtnC.wasPressed())
    {
        // Serial.println("Button C was Pressed");
        button_pressed = 1;
        buttonC = 1;
    }
}

void GTimer::print_draw_data()
{

    // Serial.println("Contest Head :  ___________________________");
    // Serial.print("contestNum: ");
    // Serial.println(mycompetition.head.contestNum);
    // Serial.print("number_of_rounds: ");
    // Serial.println(mycompetition.head.number_of_rounds);
    // Serial.print("number_of_pilots: ");
    // Serial.println(mycompetition.head.number_of_pilots);
    // Serial.print("scores_locked: ");
    // Serial.println(mycompetition.head.scores_locked);
    // Serial.print("draw_locked: ");
    // Serial.println(mycompetition.head.draw_locked);
    // Serial.print("landing_millis_allowance: ");
    // Serial.println(mycompetition.head.landing_millis_allowance);
    // Serial.print("Pilot_0_id: ");
    // Serial.print(GT.pilot_id_contest[0]);
    // Serial.print("S/: ");
    // Serial.println(GT.serialnum_contest[0]);
    // Serial.print("Pilot_1_id: ");
    // Serial.print(GT.pilot_id_contest[1]);
    // Serial.print("S/: ");
    // Serial.println(GT.serialnum_contest[1]);
    // Serial.print("Pilot_2_id: ");
    // Serial.print(GT.pilot_id_contest[2]);
    // Serial.print("S/: ");
    // Serial.println(GT.serialnum_contest[2]);
    // Serial.print("Pilot_3_id: ");
    // Serial.print(GT.pilot_id_contest[3]);
    // Serial.print("S/: ");
    // Serial.println(GT.serialnum_contest[3]);
    // Serial.println("Contest DrawDAta :  ___________________________");
    Serial.println("Draw_data<---------------");
    if (groups_enabled)
    {
        for (int i = 1; i < MAX_GROUP_DRAW_DATA; i++)
        {
            if (group_draw_data[i].record_num > 0)
            {
                Serial.print("  Pilot #:  ");
                Serial.print(i);
                Serial.print("  Pilot ID:  ");
                Serial.print(group_draw_data[i].pilot_id);
                Serial.print("  comp_no: ");
                Serial.print(group_draw_data[i].comp_no);
                Serial.print("  record_num: ");
                Serial.print(group_draw_data[i].record_num);
                Serial.print("  round_no: ");
                Serial.print(group_draw_data[i].round_no);
                Serial.print("  group_no: ");
                Serial.print(group_draw_data[i].group_no);
                Serial.print("  fltnum: ");
                Serial.print(group_draw_data[i].fltnum);
                Serial.print("  F5J_height: ");
                Serial.print(group_draw_data[i].F5J_height);
                Serial.print("  serialnumber: ");
                Serial.print(group_draw_data[i].serialnumber);
                Serial.print("  take_off_millis: ");
                Serial.print(group_draw_data[i].take_off_millis);
                Serial.print("  raw_score: ");
                Serial.println(group_draw_data[i].raw_score);
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        for (int i = 1; i <= mycompetition.head.number_of_rounds; i++)
        //  for (int i = 1; i < MAX_SELECTION_POINTER_ROUND; i++)
        {
            Serial.println();
            Serial.print("Round #: ############# ");
            Serial.println(i);

            for (int j = 1; j <= mycompetition.head.number_of_pilots; j++)
            //  for (int j = 1; j < MAX_SELECTION_POINTER_PILOT; j++)
            {
                Serial.print("  Pilot #:  ");
                Serial.print(j);
                Serial.print("  Pilot ID:  ");
                Serial.print(draw_data[i][j].pilot_id);
                Serial.print("  comp_no: ");
                Serial.print(draw_data[i][j].comp_no);
                Serial.print("  record_num: ");
                Serial.print(draw_data[i][j].record_num);
                Serial.print("  round_no: ");
                Serial.print(draw_data[i][j].round_no);
                Serial.print("  group_no: ");
                Serial.print(draw_data[i][j].group_no);
                Serial.print("  fltnum: ");
                Serial.print(draw_data[i][j].fltnum);
                Serial.print("  F5J_height: ");
                Serial.print(draw_data[i][j].F5J_height);
                Serial.print("  serialnumber: ");
                Serial.print(draw_data[i][j].serialnumber);
                Serial.print("  take_off_millis: ");
                Serial.print(draw_data[i][j].take_off_millis);
                Serial.print("  raw_score: ");
                Serial.println(draw_data[i][j].raw_score);
            }
        }
    }
    Serial.println("Draw_data-------------------->");
}
void GTimer::print_draw_list()
{

    // Serial.println("Contest Head :  ___________________________");
    // Serial.print("contestNum: ");
    // Serial.println(mycompetition.head.contestNum);
    // Serial.print("number_of_rounds: ");
    // Serial.println(mycompetition.head.number_of_rounds);
    // Serial.print("number_of_pilots: ");
    // Serial.println(mycompetition.head.number_of_pilots);
    // Serial.print("scores_locked: ");
    // Serial.println(mycompetition.head.scores_locked);
    // Serial.print("draw_locked: ");
    // Serial.println(mycompetition.head.draw_locked);
    // Serial.print("landing_millis_allowance: ");
    // Serial.println(mycompetition.head.landing_millis_allowance);
    // Serial.print("Pilot_0_id: ");
    // Serial.print(GT.pilot_id_contest[0]);
    // Serial.print("S/: ");
    // Serial.println(GT.serialnum_contest[0]);
    // Serial.print("Pilot_1_id: ");
    // Serial.print(GT.pilot_id_contest[1]);
    // Serial.print("S/: ");
    // Serial.println(GT.serialnum_contest[1]);
    // Serial.print("Pilot_2_id: ");
    // Serial.print(GT.pilot_id_contest[2]);
    // Serial.print("S/: ");
    // Serial.println(GT.serialnum_contest[2]);
    // Serial.print("Pilot_3_id: ");
    // Serial.print(GT.pilot_id_contest[3]);
    // Serial.print("S/: ");
    // Serial.println(GT.serialnum_contest[3]);
    // Serial.println("Contest DrawDAta :  ___________________________");
    Serial.println("Draw_LIst<---------------");
    for (int i = 0; i <= mycompetition.head.number_of_rounds; i++)
    //  for (int i = 1; i < MAX_SELECTION_POINTER_ROUND; i++)
    {
        Serial.println();
        Serial.printf("Round #: %d #############grp_en: %d \n", i, groups_enabled);
        for (int j = 1; j <= mycompetition.head.number_of_pilots; j++)
        //  for (int j = 1; j < MAX_SELECTION_POINTER_PILOT; j++)
        {

            // if (groups_enabled)
            // {
            //     Serial.print("  Pilot #:  ");
            //     Serial.print(j);
            //     Serial.print("  Pilot ID:  ");
            //     Serial.print(group_draw_list[i][j].pilot_id);
            //     Serial.print("  pilot_number: ");
            //     Serial.print(group_draw_list[i][j].pilot_number);
            //     Serial.print("  group: ");
            //     Serial.print(group_draw_list[i][j].group);
            //     Serial.println();
            // }
            // else
            // {
            //     Serial.print("  Pilot #:  ");
            //     Serial.print(j);
            //     Serial.print("  Pilot ID:  ");
            //     Serial.print(mycompetition.draw_list[i][j].pilot_id);
            //     Serial.print("  pilot_number: ");
            //     Serial.print(mycompetition.draw_list[i][j].pilot_number);
            //     Serial.print("  group: ");
            //     Serial.print(mycompetition.draw_list[i][j].group);
            //     Serial.print("  FAI_score: ");
            //     Serial.print(mycompetition.draw_list[i][j].FAI_score);
            //     Serial.println();
            //     // Serial.print("  round_no: ");
            //     // Serial.print(draw_data[i][j].round_no);
            //     // Serial.print("  fltnum: ");
            //     // Serial.print(draw_data[i][j].fltnum);
            //     // Serial.print("  F5J_height: ");
            //     // Serial.print(draw_data[i][j].F5J_height);
            //     // Serial.print("  serialnumber: ");
            //     // Serial.print(draw_data[i][j].serialnumber);
            //     // Serial.print("  take_off_millis: ");
            //     // Serial.print(draw_data[i][j].take_off_millis);
            //     // Serial.print("  raw_score: ");
            //     // Serial.println(draw_data[i][j].raw_score);}
            // }
        }
    }
    Serial.println("Draw_LIST-------------------->");
}

void GTimer::report_error(int error_code)
{
    for (uint32_t j = 0; j < (MAX_MENUPOINTER + 1); j++)
    {
        if (GTmenu[j].menulevel == error_code)
        {
            menupointer = j;
            break;
        }
        if (j == MAX_MENUPOINTER)
        {
            menupointer = 1;
            break;
        }
    }
    FLT_mode = PATATA;
    page_to_show = MENU_PAGE;
    menulevel = error_code;
    // Serial.print("JJJJJJJJJmenulevel");
    // Serial.print(menulevel);
    GD.menu_displayUpdate(menupointer);
}

bool GTimer::all_scores_in_round()
{
    if (GT.scheduled)
    {
        return false;
    }
    if (stored_round_no > 0)
    {
        for (int piloti = 1; piloti <= mycompetition.head.number_of_pilots; piloti++)
        {
            if (draw_data[stored_round_no][piloti].group_no == stored_group_no)
            {
                Serial.println(stored_round_no);
                Serial.println(draw_data[stored_round_no][piloti].fltnum);
                if (draw_data[stored_round_no][piloti].fltnum == 0)
                    return false;
            }
        }
    }
    return true;
}

uint32_t GTimer::esplit(String line, String delim)
{
    uint32_t kk = line.length();
    String chunk;
    delims[0] = 0;
    int I;
    for (int i = 1; i < 20; i++)
    {
        fields[i] = 0;
        delims[i] = 0;
        I = line.indexOf(delim);
        // Serial.print(delim);
        // Serial.printf("index: %d \n",I);

        if (I > -1)
        {
            delims[i] = I;
            if (i > 0)
            {
                delims[i] += delims[i - 1] + 1;
            }
            // Serial.println(delims[i]);
            chunk = line.substring(0, I);
            if (isValidNumber(chunk))
            {
                fields[i] = chunk.substring(0, I).toInt();
            }
            line = line.substring(I + 1);
        }
        else
        {
            delims[i] = kk + 2;
        }
        // Serial.printf("delim de i: %d I : %d\n", delims[i],i);
    }
    for (int i = 0; i < 20; i++)
    {
        // Serial.printf("Delimiters i: %d delims[i] :  %d fields[i]:  %d\n", i, delims[i], fields[i]);
    }
    return kk;
}

String GTimer::textxtract(String line, String delim, int pos)
{
    // line must have been split()ed
    // pos >=1
    //  pos==1 means "before first delim"
    if (pos < 1)
        pos = 1;
    // Serial.print("LIne is  :");
    // Serial.println(line);
    // Serial.print("pos is  :");
    // Serial.println(pos);
    // Serial.print("delim[pos-1] :");
    // Serial.println(delims[pos - 1]);
    // Serial.print("delim de pos is  :");
    // Serial.println(delims[pos]);

    // String chunk = line.substring(50, 150);

    String chunk = line.substring(delims[pos - 1], delims[pos] - 1);
    // Serial.print("chunked Value :");
    // Serial.println(chunk);
    return chunk;
}

String GTimer::desplit(uint32_t fields[20])
{
    String line = "";
    return line;
}

bool GTimer::isValidNumber(String str)
{
    bool isNum = false;
    bool dot = 0;
    for (byte i = 0; i < str.length(); i++)
    {
        if (i == 0)
        {
            isNum = isDigit(str.charAt(i)) || str.charAt(i) == '+' || str.charAt(i) == '.' || str.charAt(i) == '-';
        }
        else
        {
            isNum = isDigit(str.charAt(i)) || str.charAt(i) == '.';
        }
        if (str.charAt(i) == '.')
        {
            if (dot)
                return false;
            dot = 1;
        }
        if (!isNum)
            return false;
    }
    return isNum;
}

int GTimer::confirm_for_upload_or_new_launch()
{
    int result;
    int stored_index = 10 * stored_round_no + stored_group_no;
    int tfd_index = 10 * FLTdata.tfd.round_no + FLTdata.tfd.group_no;
    if ((tfd_index > 0) && (stored_index < tfd_index))
    {
        result = CONFIRM_WORKING_TIME;
    }
    else
    {
        if (tfd_index == stored_index)
        {
            result = CONFIRM_TFD_UPLOAD;
        }
        else
        {
            result = COMPETITION_CLOSED;
        }
    }
    return result;
}

void GTimer::update_setting_labels()
{

    //     yellow =
    //  TFT_WHITE;
    // green =

    // dark_blue =

    // background_colorx
    // white_colorx = TFT_WHITE;
    for (int i; i < MAX_MENUPOINTER; i++)
    {
        if (GTmenu[i].menulevel == 25)
        { // 25  is the pointer for the group management setting area
            management_pointer = i;
            break;
        }
    }
    if (groups_enabled)
    {
        strcpy(GTmenu[management_pointer].menulabel, "grps ENABLED");
        GD.white_colorx = GD.yellow;
    }
    else
    {
        GD.white_colorx = TFT_WHITE;
        strcpy(GTmenu[management_pointer].menulabel, "grps DISABLED");
    }
    if (collector)
    {
        strcpy(GTmenu[management_pointer + 1].menulabel, "collector is ON");
        GD.background_colorx = GD.green;
        if (collector == 2)
            GD.background_colorx = GD.black_colorx;
    }
    else
    {
        GD.background_colorx = GD.dark_blue;
        strcpy(GTmenu[management_pointer + 1].menulabel, "collector is OFF"); /* code */
    }
    if (scheduled)
    {
        strcpy(GTmenu[management_pointer + 2].menulabel, "scheduled is ON");
    }
    else
    {
        strcpy(GTmenu[management_pointer + 2].menulabel, "scheduld is OFF"); /* code */
    }
}

void GTimer::set_competition_max_values()
{
    if (!groups_enabled)
    {
        max_round = MAX_SELECTION_POINTER_ROUND;
        max_pilot = MAX_SELECTION_POINTER_PILOT;
        max_group = MAX_SELECTION_POINTER_ROUND;
        stored_group_no = current_group_no = 1;
    }
    else
    {
        max_round = GROUP_MAX_ROUND;
        max_pilot = GROUP_MAX_SELECTION_POINTER_PILOT;
        max_group = GROUP_MAX_GROUP;
    }
}

uint8_t GTimer::GetHexVal(char hex)
{
    int val = (int)hex;
    // For uppercase A-F letters:
    // return val - (val < 58 ? 48 : 55);
    // For lowercase a-f letters:
    // return val - (val < 58 ? 48 : 87);
    // Or the two combined, but a bit slower:
    return val - (val < 58 ? 48 : (val < 97 ? 55 : 87));
}

uint32_t GTimer::char2_SN(char SerialNumber[12])
{

    uint32_t SN = 0;
    for (int i = 0; i < 4; i++)
    {
        uint8_t Hbyte;
        uint8_t Lbyte;
        uint8_t myoffset = 3 * i;
        SN = (uint32_t)(SN << 8);
        Hbyte = (uint8_t)(GetHexVal(SerialNumber[myoffset]));
        // Serial.printf("letraH %c valor %d \n", SerialNumber[myoffset], Hbyte);
        Hbyte = (uint8_t)(Hbyte << 4);
        Lbyte = (uint8_t)(GetHexVal(SerialNumber[myoffset + 1]));
        // Serial.printf("letraL %c valor %d \n", SerialNumber[myoffset + 1], Lbyte);
        SN += Hbyte + Lbyte;
    }
    return SN;
}

uint32_t GTimer::find_pilot_ID_in_nongroup_competion(uint32_t new_candidate_id)
{
    uint32_t new_candidate_no = 0;
    for (int piloti = 1; piloti <= mycompetition.head.number_of_pilots; piloti++)
    {
        // for (int roundj = 1; roundj <= mycompetition.head.number_of_rounds; roundj++)
        // {
        Serial.printf("new_candidate_id %d ID: %d \n", new_candidate_id, (uint32_t)mycompetition.draw_list[0][piloti].pilot_id);
        if ((uint32_t)mycompetition.draw_list[0][piloti].pilot_id == new_candidate_id)
        {
            Serial.printf("found pilot in the contest number %d ID: %d S/N  %s \n", piloti, new_candidate_id, FLTdata.tfd.serialnumber);
            new_candidate_no = piloti;
            break;
        }
        // }
    }
    return new_candidate_no;
}

int GTimer::fill_pilot_data(String line, int curr_pilot)
{
    // curr_pilot = curr_pilot - 1;
    // Serial.print("line: ");
    // Serial.println(line);
    uint32_t kk = esplit(line, ";");
    String name = textxtract(line, ";", 1);
    name.replace("\"", "");
    name = textxtract(line, ";", 2);
    name.replace("\"", "");
    int sys_pilot_id = name.toInt();
    char model_SN[12];
    textxtract(line, ";", 6).toCharArray(model_SN, 12);
    uint32_t model_num_SN = char2_SN(model_SN);
    // Serial.printf("         pilot_no: %d pilot ID: %d Serial_number %d\n", curr_pilot, sys_pilot_id, model_num_SN);

    if (sys_pilot_id == group_pilots_table[curr_pilot].pilot_id)
    {
        if (group_pilots_table[curr_pilot].serialnumber2 == 0)
        {
            group_pilots_table[curr_pilot].serialnumber2 = model_num_SN;
        }
        else
        {
            group_pilots_table[curr_pilot].serialnumber3 = model_num_SN;
        }
    }
    else
    {
        curr_pilot++;
        group_pilots_table[curr_pilot].pilot_id = sys_pilot_id;
        group_pilots_table[curr_pilot].serialnumber = model_num_SN;
        // Serial.printf("STRed pilot_no: %d pilot ID: %d Serial_number %d\n", curr_pilot, group_pilots_table[curr_pilot].pilot_id, group_pilots_table[curr_pilot].serialnumber);
    }
    // Serial.printf("STRed pilot_no: %d pilot ID: %d SN: (%X) SN2: (%X)  SN3: (%X) \n", curr_pilot, group_pilots_table[curr_pilot].pilot_id, group_pilots_table[curr_pilot].serialnumber, group_pilots_table[curr_pilot].serialnumber2, group_pilots_table[curr_pilot].serialnumber3);
    return curr_pilot;
}