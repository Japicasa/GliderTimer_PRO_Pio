#include <arduino.h>
#include "GTdisplay.h"

extern GTimer GT;
extern Timer_flight_data FLTdata;
extern Flight_Manager flight;
extern int FLT_mode;
extern Gthrow gth;
extern RTC local_RTC;
extern String serialnumber;
extern String versionkeeper;

// extern Contests mycompetition;

void GTdisplay::begin()
{

  uint16_t RR = 2;
  uint16_t GG = 64;
  uint16_t BB = 0;
  for (int i = 0; i < NO_OFF_FORMATS; i++)
  {
    for (int j = 0; j < NO_OFF_LINES; j++)
    {
      if (i == 0)
      {
        // offsets[i][j].text_size = text_size0;
      }
      else
      {
        // offsets[i][j].text_size = text_size1;
      }
      if (j == 0)
      {
        offsets[i][j].xoffset = page_format[i].xoffset0;
        offsets[i][j].text_size = page_format[i].text_size0;
        offsets[i][j].yoffset = page_format[i].yoffset00;
      }
      else
      {
        offsets[i][j].xoffset = page_format[i].xoffset1;
        offsets[i][j].text_size = page_format[i].text_size1;
        offsets[i][j].yoffset = page_format[i].yoffset00 + page_format[i].yoffset0 + j * page_format[i].yoffset1;
      }

      // Serial.print("DISPLAY BEGIN textsize0");Serial.print(offsets[i][j].text_size);Serial.print("yoffsett");Serial.print(offsets[i][j].yoffset);Serial.print(" Xoffset");Serial.print(offsets[i][j].xoffset);Serial.print(" ");Serial.println(j);
    }
  }

  RR = 255;
  GG = 200;
  BB = 150;
  RR = RR >> 3;
  GG = GG >> 2;
  BB = BB >> 3;
  RR = RR << 11;
  GG = GG << 5;
  BB = BB;
  yellow = RR + GG + BB;

  RR = 16;
  GG = 32;
  BB = 80;
  RR = RR >> 3;
  GG = GG >> 2;
  BB = BB >> 3;
  RR = RR << 11;
  GG = GG << 5;
  BB = BB;
  dark_blue = RR + GG + BB;

  RR = 2;
  GG = 64;
  BB = 0;
  RR = RR >> 3;
  GG = GG >> 2;
  BB = BB >> 3;
  RR = RR << 11;
  GG = GG << 5;
  BB = BB;
  green = RR + GG + BB;

  RR = 6;
  GG = 100;
  BB = 0;
  RR = RR >> 3;
  GG = GG >> 2;
  BB = BB >> 3;
  RR = RR << 11;
  GG = GG << 5;
  BB = BB;
  green1 = RR + GG + BB;

  RR = 255;
  GG = 10;
  BB = 2;
  RR = RR >> 3;
  GG = GG >> 2;
  BB = BB >> 3;
  RR = RR << 11;
  GG = GG << 5;
  BB = BB;
  red = RR + GG + BB;

  RR = 1;
  GG = 10;
  BB = 255;
  RR = RR >> 3;
  GG = GG >> 2;
  BB = BB >> 3;
  RR = RR << 11;
  GG = GG << 5;
  BB = BB;
  blue = RR + GG + BB;

  RR = 150;
  GG = 150;
  BB = 150;
  RR = RR >> 3;
  GG = GG >> 2;
  BB = BB >> 3;
  RR = RR << 11;
  GG = GG << 5;
  BB = BB;
  grey = RR + GG + BB;

  background_colorx = green;
  white_colorx = TFT_WHITE;
    String myline = "UIv_not found";
    File file = SPIFFS.open("/version.txt", "r+");
    if (!file)
    {
    }
    else
    {
      int xx = 1;
      while (file.available())
      {
        // Lets read line by line from the file
        myline = "";
        myline = file.readStringUntil('\n');
        myline.replace("\n", "");

        fw_ui_version ="<strong>"+ fw_ui_version + "</strong>    UI: <strong>" + myline+"</strong>";
        // fw_ui_version ="xxx3t";
        yield();
        xx++;
        if (xx > 1)
          break;
      }
      file.close();
    }
}
void GTdisplay::clear_display()
{
  M5.Lcd.setTextColor(white_colorx, background_colorx);
  M5.Lcd.fillScreen(background_colorx);
}

void GTdisplay::GDsetTextSize(int size)
{
  M5.Lcd.setTextSize(1);

  switch (size)
  {
  case 2:
    M5.Lcd.setFreeFont(FMB9);
    break;
  case 3:
    M5.Lcd.setFreeFont(FSS18);
    break;
  case 4:
    // M5.Lcd.setFreeFont(FSS18);
    M5.Lcd.setFreeFont(FSSB24);
    break;
  case 5:
    // M5.Lcd.setFreeFont(FMB18);
    //  M5.Lcd.setTextFont(FONT4);
    M5.Lcd.setFreeFont(FMB12);

    break;
  case 6:
    // M5.Lcd.setTextFont(6);
    M5.Lcd.setFreeFont(FMB12);
    break;
  }
}

///////////////////////////////////////////
// Funcion Mostrar pantalla Medidas
////////////////////////////////////////
void GTdisplay::cursor(uint32_t line_no, uint32_t format_no = 2)
{
  xpos = offsets[format_no][line_no].xoffset;
  ypos = offsets[format_no][line_no].yoffset;
  M5.Lcd.setCursor(xpos, ypos);
  // M5.Lcd.setTextSize(offsets[format_no][line_no].text_size);
  GDsetTextSize(offsets[format_no][line_no].text_size);

  // Serial.print("line no. ");
  // Serial.print(line_no);
  // Serial.print(" Format  ");
  // Serial.print(format_no);
  // Serial.print(" textSize ");
  // Serial.println(offsets[format_no][line_no].text_size);
}

void GTdisplay::string_to_disp(String TC, uint32_t line, uint32_t format)
{
  cursor(line, format);
  M5.Lcd.print(TC);
}

void GTdisplay::string_to_disp(char TC[25], uint32_t line, uint32_t format)
{
  cursor(line, format);
  M5.Lcd.print(TC);
}
// void GTdisplay::displaymenu()
// {
// }
void GTdisplay::displaywelcomescreen() // Supongo que lo usaremos en algún momento
{
  //   Serial.print("local green????");Serial.println(background_colorx);
  //   Serial.print("GTdisp green????");Serial.println(GTdisp.background_colorx);
  //  background_colorx=992;

  M5.Lcd.fillScreen(background_colorx);
  // Serial.print("FIlled in green?????????????????????????????????????");Serial.println(background_colorx);
  M5.Lcd.setTextColor(white_colorx, background_colorx);
  // Serial.print("FIlled in green?????????????????????????????????????");Serial.println(white_colorx);
  int format = 0;
  for (int j = 0; j < 7; j++)
  {
    cursor(j, format);
    // Serial.print("[Yoffset]");Serial.print(GTdisp.offsets[format][j].yoffset);Serial.print(" [Xoffset]");Serial.println(offsets[format][j].xoffset);
    // Serial.print("Yoffset");Serial.print(ypos);Serial.print(" Xoffset");Serial.println(xpos);

    // M5.Lcd.setCursor(xpos, ypos);
    M5.Lcd.print("Glider");
    M5.Lcd.print("");
    M5.Lcd.print("Timer");
    M5.Lcd.print(j);
  }
}

void GTdisplay::displaymenu()
{

  for (int i = 0; i < MAX_MENUPOINTER; i++)
  {
    // Serial.print("MenuPointer ");
    // Serial.print(i);
    // Serial.print(" lastmenulabelx ");

    // String TC(GT.GTmenu[i].menulabel);

    // Serial.print(TC);
    // Serial.print(" next_menulevel ");
    // Serial.println(GT.GTmenu[i].new_menulevel);
  }
}
void GTdisplay::menu_display_final_round(uint32_t round_number)
{
  Serial.println(" aqui va la final");
  uint32_t header_pointer = 0;
  int formatNo = 4; // display list
  clear_display();
  sprintf(tc, "Cmp# %u Final Score", GT.current_competition_no);
  string_to_disp(tc, 0, formatNo);
  if (zoom_level == 0)
  {
    formatNo = 6; // was 4
    sprintf(tc, "  Id | Tot| R#1| R#2");
  }
  if (zoom_level == 1)
  {
    formatNo = 6;
    sprintf(tc, "  Id | Tot| R#3| R#4");
  }
  if (zoom_level == 2)
  {
    formatNo = 6;
    sprintf(tc, "  Id | Tot| R#5| R#6");
  }
  string_to_disp(tc, 1, formatNo);
  if (GT.groups_enabled)
  {
    strcpy(tc, "Report not available");
    string_to_disp(tc, 1, formatNo);
    strcpy(tc, "in groups mode");
    string_to_disp(tc, 2, formatNo);
    return;
  }
  for (int piloti = 2; piloti < (GT.mycompetition.head.number_of_pilots + 2); piloti++)
  {
    int pilotj = GT.rank_list[0][0][piloti - 1];
    int pilot_id = GT.mycompetition.draw_list[round_number][pilotj].pilot_id;
    if (pilotj > 0)
    {
      if (pilotj > 0)
      {
        sprintf(tc, "%5s", GT.pilots_table[pilot_id].nick_name);
        if (strcmp(GT.pilots_table[pilot_id].nick_name, "    ") == 0)
        {
          sprintf(tc, "%5s", GT.pilots_table[pilot_id].serialnumber);
        }
      }
      else
      {
        sprintf(tc, "%5s", "?????");
      }
      sprintf(tc, "%s|%4d", tc, GT.mycompetition.draw_list[0][pilotj].FAI_score);
    }
    if (GT.mycompetition.draw_list[0][pilotj].FAI_score == 0)
    {
      break;
    }
    int rd_seg = 1; // zoom level =0
    if (zoom_level == 0)
    {
      rd_seg = 1;
    }
    if (zoom_level == 1)
    {
      rd_seg = 3;
    }
    if (zoom_level == 2)
    {
      rd_seg = 5;
    }
    char asteris[5];
    char asteris1[5];
    if (GT.mycompetition.draw_list[0][pilotj].discarded_round == rd_seg)
    {
      sprintf(asteris, "*%3d", GT.mycompetition.draw_list[rd_seg][pilotj].FAI_score);
    }
    else
    {
      if (GT.mycompetition.draw_list[rd_seg][pilotj].FAI_score > 999)
      {
        sprintf(asteris, "%d", GT.mycompetition.draw_list[rd_seg][pilotj].FAI_score);
      }
      else
      {
        sprintf(asteris, " %3d", GT.mycompetition.draw_list[rd_seg][pilotj].FAI_score);
      }
    }
    if (GT.mycompetition.draw_list[0][pilotj].discarded_round == (rd_seg + 1))
    {
      sprintf(asteris1, "*%3d", GT.mycompetition.draw_list[rd_seg + 1][pilotj].FAI_score);
    }
    else
    {
      if (GT.mycompetition.draw_list[rd_seg + 1][pilotj].FAI_score > 999)
      {
        sprintf(asteris1, "%d", GT.mycompetition.draw_list[rd_seg + 1][pilotj].FAI_score);
      }
      else
      {
        sprintf(asteris1, " %3d", GT.mycompetition.draw_list[rd_seg + 1][pilotj].FAI_score);
      }
    }
    sprintf(tc, "%5s|%s|%s", tc, asteris, asteris1);
    string_to_disp(tc, piloti, formatNo);
  }
}

void GTdisplay::menu_display_list_round(uint32_t round_number)
{
  Serial.println(" aqui va la lista");
  //// Forzamos group_number = 1  22/01/23
  if (round_number == 0)
  {
    menu_display_final_round(round_number);
    Serial.println("ROUND NUMBER==0");
  }
  else
  {
    int group_number = 1;
    uint32_t header_pointer = 0;
    int formatNo = 4; // display list
    clear_display();
    int pilots_to_show = 0;
    if (GT.groups_enabled)
    {
      snprintf(tc, 24, "Round# %u Group# %u", round_number, group_number);
      pilots_to_show = GT.mycompetition.head.pilots_per_round + 2;
    }
    else
    {
      snprintf(tc, 24, "Comp# %u Round# %u", GT.current_competition_no, round_number);
      pilots_to_show = GT.mycompetition.head.number_of_pilots + 2;
    }
    string_to_disp(tc, 0, formatNo);

    if (zoom_level == 0)
    {
      formatNo = 4;
      sprintf(tc, "  Id | H | t |m|raw| FAI|p");
    }
    if (zoom_level == 1)
    {
      formatNo = 6; // was 5
      sprintf(tc, "  Id | H | t |m");
    }
    if (zoom_level == 2)
    {
      formatNo = 6;
      sprintf(tc, "  Id |raw| FAI|p");
    }
    string_to_disp(tc, 1, formatNo);
    if (GT.groups_enabled)
    {
      strcpy(tc, "Report not available");
      string_to_disp(tc, 1, formatNo);
      strcpy(tc, "in groups mode");
      string_to_disp(tc, 2, formatNo);
      return;
    }
    for (int piloti = 2; piloti < (pilots_to_show); piloti++)
    {
      // id
      int pilotj = GT.rank_list[round_number][group_number][piloti - 1];
      int pilot_id = GT.mycompetition.draw_list[round_number][pilotj].pilot_id;

      if (pilotj > 0)
      {
        sprintf(tc, "%5s", GT.pilots_table[pilot_id].nick_name);
        if (strcmp(GT.pilots_table[pilot_id].nick_name, "    ") == 0)
        {
          sprintf(tc, "%5s", GT.pilots_table[pilot_id].serialnumber);
        }
      }
      else
      {
        sprintf(tc, "%5s", "?????");
      }

      // F5Jh
      uint32_t hTC = (int)(GT.draw_data[round_number][pilotj].F5J_height / 10);

      // time
      uint32_t timeTC = GT.draw_data[round_number][pilotj].flight_mins * 100;
      timeTC = timeTC + GT.draw_data[round_number][pilotj].flight_secs;

      // dist
      uint32_t dTC = GT.draw_data[round_number][pilotj].landing_dist;
      if (dTC == 10)
      {
        dTC = 10;
      }
      if (dTC > 10)
      {
        dTC = 15;
      }

      // raw score
      uint32_t rTC = GT.draw_data[round_number][pilotj].raw_score;

      if (GT.mycompetition.draw_list[0][pilotj].FAI_score == 0)
      {
        Serial.print("END OF USEFUL DATA");
        Serial.printf("pilotj = %d\n", pilotj);
        break;
      }
      if (zoom_level == 0)
      {
        formatNo = 4;
        sprintf(tc, "%s|%3d|%3d|%X|%3d|%4d|", tc, hTC, timeTC, dTC, rTC, GT.mycompetition.draw_list[round_number][pilotj].FAI_score);
        Serial.print(tc);
      }
      if (zoom_level == 1)
      {
        formatNo = 6;
        sprintf(tc, "%s|%3d|%3d|%X|", tc, hTC, timeTC, dTC);
        Serial.print(tc);
      }
      if (zoom_level == 2)
      {
        formatNo = 6;
        sprintf(tc, "%s|%3d|%4d|", tc, rTC, GT.mycompetition.draw_list[round_number][pilotj].FAI_score);
        Serial.print(tc);
      }
      string_to_disp(tc, piloti, formatNo);
    }
  }
}

void GTdisplay::menu_display_list_pilot(uint32_t pilot_number)
{
  uint32_t header_pointer = 0;
  int formatNo = 7; // display list
  clear_display();
  int pilot_id = GT.mycompetition.draw_list[0][pilot_number].pilot_id;
  sprintf(tc, "C#%d P#%d ID:%s", GT.current_competition_no, pilot_number, GT.pilots_table[pilot_id].serialnumber);
  string_to_disp(tc, 0, formatNo);
  if (zoom_level == 0)
  {
    formatNo = 7;
    sprintf(tc, "Rk| H | t |m|raw| FAI|p");
  }
  if (zoom_level == 1)
  {
    formatNo = 6; // was 5
    sprintf(tc, "Rk| H | t |m");
  }
  if (zoom_level == 2)
  {
    formatNo = 6;
    sprintf(tc, "Rk|raw| FAI|p");
  }
  string_to_disp(tc, 1, formatNo);
  if (GT.groups_enabled)
  {
    strcpy(tc, "Report not available");
    string_to_disp(tc, 1, formatNo);
    strcpy(tc, "in groups mode");
    string_to_disp(tc, 2, formatNo);
    return;
  }
  for (int roundi = 2; roundi < (GT.mycompetition.head.number_of_rounds + 2); roundi++)
  {
    int round_number = roundi - 1;
    int pilotj = pilot_number;

    int rrank = GT.mycompetition.draw_list[round_number][pilot_number].round_rank;

    // F5Jh
    int f5jh = (int)(GT.draw_data[round_number][pilotj].F5J_height / 10);
    int fdist = GT.draw_data[round_number][pilotj].landing_dist;
    if (fdist > 10)
    {
      fdist = 15;
    }
    uint32_t timeTC = GT.draw_data[round_number][pilotj].flight_mins * 100;
    timeTC = timeTC + GT.draw_data[round_number][pilotj].flight_secs;
    int fraw = GT.draw_data[round_number][pilotj].raw_score;
    int ffai = GT.mycompetition.draw_list[round_number][pilotj].FAI_score;
    if (zoom_level == 0)
    {
      formatNo = 7;
      sprintf(tc, "%2d|%3d|%3d|%X|%3d|%4d| ", rrank, f5jh, timeTC, fdist, fraw, ffai);
    }
    if (zoom_level == 1)
    {
      formatNo = 6;
      sprintf(tc, "%2d|%3d|%3d|%X", rrank, f5jh, timeTC, fdist);
    }
    if (zoom_level == 2)
    {
      formatNo = 6;
      sprintf(tc, "%2d|%3d|%4d| ", rrank, fraw, ffai);
    }
    string_to_disp(tc, roundi, formatNo);
  }
  sprintf(tc, "%s/%s", GT.pilots_table[pilot_id].nick_name, GT.pilots_table[pilot_id].name);
  if (zoom_level < 1)
  {
    formatNo = 7;
  }
  else
  {
    formatNo = 6;
  }

  string_to_disp(tc, (GT.mycompetition.head.number_of_rounds + 2), formatNo);
}

void GTdisplay::menu_displayUpdate(uint32_t menupointer)
{
  // extern GTimer GT;
  //  Serial.print("DISPLAY UPDATE ");
  //  Serial.print(menupointer);
  //  Serial.print(" [menulevel] ");
  //  Serial.println(GT.GTmenu[menupointer].menulevel); // display.clearDisplay();
  uint32_t low_menupointer = 0;
  uint32_t high_menupointer = MAX_MENUPOINTER;
  uint32_t formatNo = 0;
  uint32_t header_pointer = 0;
  if (GT.GTmenu[menupointer].menulevel < 10000) /// choose the format level (from menulevel digits)
  {
    formatNo = 4; // display list
  }
  if (GT.GTmenu[menupointer].menulevel < 1000)
  {
    formatNo = 3; // menu selection
  }
  if (GT.GTmenu[menupointer].menulevel < 100)
  {
    formatNo = 2; // menu 2 digits
  }
  if (GT.GTmenu[menupointer].menulevel < 10)
  {
    formatNo = 1; // menu_1
  }
  if (GT.GTmenu[menupointer].menulevel < 1)
  {
    formatNo = 0; // welcomescreen
  }
  for (int i = 0; i < MAX_MENUPOINTER; i++) // the header/title pointer
  {
    if ((GT.GTmenu[i].new_menulevel == GT.GTmenu[menupointer].menulevel) && (low_menupointer == 0)) // the header/title pointer
    {
      header_pointer = i;
    }
    if ((GT.GTmenu[i].menulevel == GT.GTmenu[menupointer].menulevel) && (low_menupointer == 0)) // low pointer in desired l menulevel
    {
      low_menupointer = i;
    }
    if (GT.GTmenu[i].menulevel > GT.GTmenu[menupointer].menulevel) // high pointer in menulevel menulevel must be ascending
    {
      high_menupointer = i - 1;

      break;
    }
  }
  uint32_t lines_for_menulevel = high_menupointer - low_menupointer + 1;
  // Serial.print("MenuPointer**********low_menupointer ");
  // Serial.println(low_menupointer);
  // Serial.print("MenuPointer**************ACTION ");
  // Serial.println(GT.GTmenu[low_menupointer].action);
  // Serial.print("MenuPointer***********High Menupointer ");
  // Serial.println(high_menupointer);
  switch (GT.GTmenu[low_menupointer].action) // this is not to exceed stored values to minimise clicking

  {
  case ACTION_LIST_ROUND:
    high_menupointer = low_menupointer + GT.mycompetition.head.number_of_rounds;
    break;
  case ACTION_LIST_PILOT:
    high_menupointer = low_menupointer + GT.mycompetition.head.number_of_pilots - 1;
    break;
  case ACTION_SELECT_COMPETITION:
    high_menupointer = low_menupointer + GT.stored_competition_no - 1;
    break;
  case ACTION_SELECT_AP:
    high_menupointer = low_menupointer + 3;
    break;
  }

  if (menupointer > high_menupointer)
  {
    GT.menupointer = low_menupointer;
    menupointer = low_menupointer;
  }
  uint32_t initial_menupointer = low_menupointer;
  if (menupointer > (low_menupointer + (page_format[formatNo].max_lines - 2)))
  {
    initial_menupointer = menupointer - page_format[formatNo].max_lines + 1;
  }
  else
  {
    initial_menupointer = low_menupointer - 1;
  }
  clear_display();
  // M5.Lcd.drawJpgFile(SPIFFS, "/LogoKeeper0.jpg", 0, 0);

  // cabecera
  String TC = String(GT.GTmenu[header_pointer].menulabel);
  // Serial.print("label head ");
  // Serial.println(GT.GTmenu[header_pointer].menulabel);
  string_to_disp(TC, 0, formatNo);

  // for (int j = 1; j < (lines_for_menulevel ); j++)
  for (int j = 1; j < (page_format[formatNo].max_lines); j++)

  {
    int pointer;
    pointer = initial_menupointer + j;
    String TC = " ";
    if (pointer == menupointer)
    {

      if ((j == (page_format[formatNo].max_lines - 1)) && (pointer < high_menupointer))
      {
        TC = ">>";
      }
      else
      {
        TC = ">";
      }
    }
    if (pointer > high_menupointer)
    {
      break;
    }

    TC = TC + String(GT.GTmenu[pointer].menulabel);
    // Serial.print("label line ");
    // Serial.println(TC);
    string_to_disp(TC, j, formatNo);
  }
  sched_adv_first_time = 1;
  displayStatus();
}
void GTdisplay::manage_timer_display()
{
  if (primeravez)
  {

    clear_display();

    M5.Lcd.setTextColor(white_colorx);
    cursor(0, 0);
    // M5.Lcd.setTextSize(offsets[format_no][line_no].text_size);
    M5.Lcd.print("Glider");
    M5.Lcd.print("");
    M5.Lcd.print("Timer");

    // last_time_millis = millis();

    display_timer();

    // flight.bip();
    clean_partial_flt();
    alabel = status_label();
    // alabel = "  C#" + String(GT.current_competition_no) + "   R#" + String(GT.current_round_no);
    blabel = "    To Launch:";
    blabel = "  Prepare time";
    partial_flt();
    primeravez = 0;
    last_prenotice = 1;
  }
  else
  {
    if ((last_secs_to_land - flight.secs_to_land) > 0)
    {
      last_secs_to_land = flight.secs_to_land;
      erasetimer();    /// this updates display
      display_timer(); ///
    }
  }
}

void GTdisplay::display_timer()
{
  int str_minutes;
  int seconds;
  if (flight.pre_notice_status)
  {
    str_minutes = (flight.secs_to_launch / 60);
    // str_minutes = String(flight.secs_to_launch / 60);
    // if (str_minutes.length() < 2)
    //   str_minutes = " " + str_minutes;
    // int seconds = (flight.secs_to_launch % 60);
    seconds = (flight.secs_to_launch % 60);
    // if (seconds < 10)
    // {
    //   str_seconds = "0" + String(seconds);
    // }
    // else
    // {
    //   str_seconds = String(seconds);
    // }
    // timetodisp = " " + str_minutes + ":" + str_seconds;

    snprintf(timetodisp, 7, " %2d:%.2d", str_minutes, seconds);
  }
  else
  {
    str_minutes = (flight.secs_to_land / 60);
    // str_minutes = String(flight.secs_to_land / 60);
    // if (str_minutes.length() < 2)
    //   str_minutes = " " + str_minutes;
    // int seconds = (flight.secs_to_land % 60);
    seconds = (flight.secs_to_land % 60);
    if (seconds < 0)
    {
      snprintf(timetodisp, 7, "  %3.2d", seconds);
    }
    else
    {
      snprintf(timetodisp, 7, " %2d:%.2d", str_minutes, seconds);
    }

    if (last_prenotice)
    {
      clean_partial_flt();
      alabel = status_label();
      // alabel = "  C#" + String(GT.current_competition_no) + "   R#" + String(GT.current_round_no);
      // blabel = "    To LAND:";
      // blabel = "  Preparation:";
      blabel = "Remaining time";

      partial_flt();
      last_prenotice = 0;
    }
  }

  M5.lcd.setTextColor(white_colorx);
  M5.Lcd.setTextFont(7);
  M5.Lcd.setTextSize(2);
  if (str_minutes < 10)
  {
    M5.Lcd.setCursor(20, 60);
  }
  else
  {
    M5.Lcd.setCursor(-20, 60);
  }
  if ((flight.secs_to_launch < 11) && (flight.secs_to_launch > -1))
  {
    Serial.print(flight.secs_to_launch);
    Serial.print(" On digit change ");
    local_RTC.printLocalTimeMicros();
  }
  M5.lcd.print(timetodisp);
  M5.Lcd.setTextFont(1);
  if ((flight.secs_to_launch < 11) && (flight.secs_to_launch > -1))
  {
    Serial.print("On digit changeD ");
    local_RTC.printLocalTimeMicros();
  }
}

void GTdisplay::erasetimer()
{

  M5.lcd.setTextColor(background_colorx);
  // M5.Lcd.setTextSize(6);
  M5.Lcd.setTextFont(7);
  M5.Lcd.setTextSize(2);
  M5.Lcd.fillRoundRect(0, 60, 320, 100, 2, background_colorx);
  // int str_minutes = (flight.secs_to_land / 60);
  // if (flight.pre_notice_status)
  //   str_minutes = 1;

  // if (str_minutes < 10)
  // {
  //   M5.Lcd.setCursor(20, 60);
  // }
  // else
  // {
  //   M5.Lcd.setCursor(0, 60);
  // }

  // M5.lcd.print(timetodisp);
  M5.Lcd.setTextFont(1);
}

void GTdisplay::about_screen()
{
  char tc[20];
  int formatNo = 3; // display list
  String line;
  struct tm *mytime;
  static int last_seconds = 0;
  // static bool firstime = 1;

  mytime = local_RTC.ReadRTC();
  Serial.println(mytime->tm_sec);

  if (firsttime_settings)
  {
    firsttime_settings = 0;
    clear_display();
    sprintf(tc, "GliderTimer");
    string_to_disp(tc, 0, formatNo);
    char tc1[20];
    serialnumber.toCharArray(tc1, 20);
    sprintf(tc, "ID: %s", tc1);
    GDsetTextSize(5);
    M5.Lcd.setCursor(18, 70);
    M5.Lcd.print(tc);
    versionkeeper.toCharArray(tc1, 20);
    sprintf(tc, "%s", tc1);
    string_to_disp(tc, 2, formatNo);

    line = "UIv_not found";
    File file = SPIFFS.open("/version.txt", "r+");
    if (!file)
    {
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
        yield();
        xx++;
        if (xx > 1)
          break;
      }
      file.close();
    }

    line.toCharArray(tc1, 20);
    sprintf(tc, "UI %s", tc1);
    string_to_disp(tc, 3, formatNo);

    sprintf(tc, "%02u:%02u:%02u UTC", mytime->tm_hour + 0, mytime->tm_min + 0, mytime->tm_sec + 0);
    string_to_disp(tc, 4, formatNo);
    switch (mytime->tm_wday)
    {
    case 0:
      sprintf(tc1, "Sunday", 20);
      break;
    case 1:
      sprintf(tc1, "Monday", 20);
      break;
    case 2:
      sprintf(tc1, "Tuesday", 20);
      break;
    case 3:
      sprintf(tc1, "Wednesday", 20);
      break;
    case 4:
      sprintf(tc1, "Thursday", 20);
      break;
    case 5:
      sprintf(tc1, "Friday", 20);
      break;
    case 6:
      sprintf(tc1, "Saturday", 20);
      break;
    }
    sprintf(tc, "%s,", tc1);
    string_to_disp(tc, 5, formatNo);
    sprintf(tc, "20%02u/%02u/%02u", mytime->tm_year - 100, mytime->tm_mon + 1, mytime->tm_mday);
    string_to_disp(tc, 6, formatNo);
  }
  else
  {
    if (last_seconds != mytime->tm_sec)
    {
      last_seconds = mytime->tm_sec;
      M5.Lcd.fillRoundRect(30, 139, 310, 30, 2, background_colorx);
      sprintf(tc, "%2u:%2u:%2u UTC", mytime->tm_hour, mytime->tm_min, mytime->tm_sec);
      string_to_disp(tc, 4, formatNo);
    }
  }
}

void GTdisplay::confirm_run_flight()
{

  int formatNo = 3; // display list
  clear_display();

  string_to_disp("GliderTimer", 0, formatNo);
  GDsetTextSize(5);
  if (GT.scheduled)
  {
    string_to_disp("SCHEDULED!!", 1, formatNo);
    string_to_disp("OVERWRITE", 2, formatNo);
    string_to_disp("TABLE?:'Select'", 3, formatNo);
  }
  else
  {
    string_to_disp("Not all pilots", 1, formatNo);
    string_to_disp("scored in this", 2, formatNo);
    if (GT.groups_enabled)
    {
      string_to_disp("group 'Select'", 3, formatNo);
    }
    else
    {
      string_to_disp("round 'Select'", 3, formatNo);
    }
  }
  string_to_disp("to run new", 4, formatNo);
  clean_partial_flt();
  alabel = status_label();
  // alabel = "  C#" + String(GT.current_competition_no) + "   R#" + String(GT.current_round_no);
  blabel = "";
  partial_flt();
  return void();
}

void GTdisplay::displayStatus()
{
  clean_partial_flt();
  if (GT.GTmenu[GT.menupointer].menulevel == 20)
  {
    alabel = ("Batt. Level= ") + String(M5.Power.getBatteryLevel()) + "%";
  }
  else
  {
    alabel = status_label();
    // alabel = "  C#" + String(GT.current_competition_no) + "   R#" + String(GT.current_round_no);
  }
  blabel = "";
  partial_flt();
}

void GTdisplay::displayUpdate()
{
  clean_partial_flt();
  alabel = "   Updating....";
  blabel = "      ";
  partial_flt();
}

void GTdisplay::displayUpdateError(String msg)
{
  clean_partial_flt();
  alabel = msg;
  partial_flt();
  FLT_mode = PATATA;
}

void GTdisplay::refresh_disp(String msg)
{

  M5.Lcd.fillScreen(background_colorx);
  M5.Lcd.setTextColor(white_colorx);
  M5.Lcd.setCursor(0, 0);
  // M5.Lcd.setTextSize(3);
  GDsetTextSize(3);
  M5.Lcd.print("Glider");
  M5.Lcd.print("");
  M5.Lcd.println("Timer");
  M5.Lcd.println("");
  M5.Lcd.println(msg);
  M5.Lcd.println("");
  // M5.Lcd.setTextSize(3);
}

void GTdisplay::STA_disp()
{
  static bool myfirsttime = 1;
  static bool sync_shown = 0;

  if (myfirsttime)
  {
    clear_display();
    M5.Lcd.setTextColor(white_colorx);
    M5.Lcd.setCursor(25, 40);
    GDsetTextSize(4);
    M5.Lcd.println("GliderTimer");
    GDsetTextSize(2);
    M5.Lcd.setCursor(24, 70);
    M5.Lcd.println("Now connected into:");
    GDsetTextSize(3);
    M5.Lcd.setCursor(30, 108);
    M5.Lcd.println(WiFi.SSID());
    GDsetTextSize(2);
    M5.Lcd.setCursor(25, 150);
    M5.Lcd.println("Type in your browser:");
    GDsetTextSize(3);
    M5.Lcd.setCursor(30, 190);
    M5.Lcd.print(WiFi.localIP().toString());
    GDsetTextSize(2);
    M5.Lcd.setCursor(20, 207);
    M5.Lcd.println("(or: glidertimer.local )");
    M5.Lcd.setCursor(10, 231);

    // M5.Lcd.println("push data-> :rnd     :all)");
    M5.Lcd.println(" <-back   get_table  SEND->");

    myfirsttime = 0;
  }
  else
  {
    if (local_RTC.keeper_Synchro_status > 5)
    {
      if (!sync_shown)
      {
        sync_shown = 1;
        GDsetTextSize(2);
        M5.Lcd.fillRoundRect(150, 118, 190, 20, 1, background_colorx);

        M5.Lcd.setCursor(180, 130);
        if (local_RTC.keeper_Synchro_status > 6)
        {
          M5.Lcd.setCursor(80, 130);
          M5.Lcd.println("Calibrated. Full Sync!");
        }
        else
        {
          M5.Lcd.println("Full Sync!!");
        }
      }
    }
    else
    {
      GDsetTextSize(2);
      M5.Lcd.setCursor(150, 130);
      M5.Lcd.println("wait for Sync");
    }
    delay(20);
  }
  // M5.Lcd.setTextSize(3);
}
bool GTdisplay::file_disp(int file_num, String payload)
{
  // int kindex = payload.indexOf(";human_readable"):;tm_year
  // String last_head = ";tm_year";
  // int kindex = payload.indexOf(";tm_year");
  // kindex = kindex + last_head.length();

  int Sindex = payload.indexOf(";tm_year");           // this is the last characters of the header of the file, 8 its legnth
  int kindex = payload.indexOf(";HumanReadableTime"); // this is the last characters of the header of the file, 18 its legnth
  bool gsFormat = 1;
  if (kindex > 0)
  {
    kindex = kindex + 18;
  }
  else
  {
    kindex = Sindex + 8;
    gsFormat = 0;
  }
  String first_line = payload.substring(kindex, kindex + 20);

  first_line.replace("\n", "");
  first_line.replace("\r", "");

  // Serial.print("first line is: ");
  // Serial.println(first_line);
  GT.esplit(first_line, ";");
  // for (int i = 1; i < 6; i++)
  // {
  //   Serial.printf("i= %d; fields= %d \n", i, GT.fields[i]);
  // }
  int rnd = 99;
  int grp = 99;
  int rfy = 99;
  if (gsFormat)
  {
    rnd = GT.fields[2];
    grp = GT.fields[3];
    rfy = GT.fields[4];
  }
  else
  {
    rnd = GT.fields[3];
    grp = GT.fields[4];
    rfy = 0;
  }
  clear_display();
  Serial.println("Clearing display");
  M5.Lcd.setTextColor(white_colorx);
  M5.Lcd.setCursor(25, 40);
  GDsetTextSize(4);
  M5.Lcd.println("GliderTimer");
  GDsetTextSize(2);
  M5.Lcd.setCursor(24, 70);
  M5.Lcd.println("file download:");
  GDsetTextSize(3);
  M5.Lcd.setCursor(30, 108);
  // String file_name;
  char lbl[30];
  switch (file_num)
  {
  case 1:
    snprintf(lbl, 30, "Pilots");
    // label2 = "Pilots";
    break;
  case 2:
    // label2 = "Draw";
    snprintf(lbl, 30, "Draw");
    break;
  case 3:

    snprintf(lbl, 30, "Round: %d", rnd);
    // label2 = "Round: ";
    break;

  default:
    // label2 = "Error_dwld";
    snprintf(lbl, 30, "Error_dwld");
    break;
  }
  // label2 += String(GT.fields[3]);

  // label2.toCharArray(lbl,30);
  M5.Lcd.println(lbl);
  // M5.Lcd.println(label2);
  if (GT.groups_enabled == 0)
  {
    snprintf(lbl, 30, "Group: %d  (Rfly: %d)", 1, 0);
  }
  else
  {
    snprintf(lbl, 30, "Group: %d  (Rfly: %d)", grp, rfy);
  }
  // label2 = "Group: " + String(GT.fields[4]) + " (Rfly: " + String(GT.fields[5])+")";
  M5.Lcd.setCursor(30, 150);
  M5.Lcd.println(lbl);
  GDsetTextSize(3);
  M5.Lcd.setCursor(30, 190);
  M5.Lcd.print("Cancel or Save");
  GDsetTextSize(2);

  M5.Lcd.setCursor(10, 231);

  M5.Lcd.println("   Cancel  :Save    :Save");
  // M5.Lcd.println("push data-> :rnd     :all)");
  buttonA = 0;
  buttonB = 0;
  buttonC = 0;
  delay(100);

  while (true)
  {
    delay(50);
    GT.button_update();
    if (buttonA)
    {
      firsttime_settings = 1;
      GT.back_to_prev_status();
      buttonA = 0;
      break;
    }
    // char label22[30];
    if ((buttonB) || (buttonC))
    {
      switch (file_num)
      {
      case 1:
        label2 = "Pilots_";
        // strcpy(label22, "Pilots_");

        break;
      case 2:
        label2 = "Draw_list";
        // strcpy(label22, "Draw_list");
        break;
      case 3:
        label2 = "/scheduled_rounds.csv";
        // strcpy(label22, "/scheduled_rounds.csv");
        break;

      default:
        label2 = "Error_dwld";
        // strcpy(label22, "Error_dwld");
        break;
      }
      buttonB = 0;
      buttonC = 0;
      bool success_save = 0;
      if (file_num == 3)
      {
        File file = SPIFFS.open(label2, "w+");
        if (!file)
        {
          Serial.println("could not open file");
        }
        else
        {
          file.print(payload);
          // file.print("\n");
          Serial.print(payload);
          file.close();
          success_save = 1;
        }
      }
      else
      {
        Serial.println("No file Available");
      }
      M5.Lcd.fillRect(10, 200, 310, 35, background_colorx);
      M5.Lcd.setCursor(10, 231);

      if (success_save)
      {
        M5.Lcd.println("  Saving file, wait... ");
        firsttime_settings = 1;
        delay(500);
        GT.back_to_prev_status();
        break;
      }
      else
      {
        M5.Lcd.println("Error!! report to CD");
      }
    }
    delay(20);
  }
  return 1;
  // M5.Lcd.setTextSize(3);
}

void GTdisplay::update_screen()
{
  static bool myfirsttime = 1;
  int pct;
  if (updating_flash)
  {
    pct = (280 * update_counter) / FLASH_SIZE; /// place here size of flash
    // if (myfirsttime)
    // {
    //   if (GT.stored_competition_no > 0)
    //   { ///////////////format if there is data
    //     Serial.printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>formatting \n");
    //     bool exito = competition.format();
    //     Serial.printf("<<<<<<<<<<<<<<<<<<<format completed exito %d \n", exito);
    //   }
    //   else
    //   {
    //     Serial.printf(">>>>>>>>>>>>>>>>>no format needed data was blank \n");
    //   }
    // }
  }
  else
  {
    pct = (280 * update_counter) / SPIFFS_SIZE; /// place here size of SPIFFS
  }
  if (myfirsttime)
  {
    clear_display();
    M5.Lcd.setTextColor(white_colorx);
    M5.Lcd.setCursor(25, 40);
    GDsetTextSize(4);
    M5.Lcd.println("GliderTimer");
    // GDsetTextSize(2);
    // M5.Lcd.setCursor(25, 75);
    // M5.Lcd.println("Now connected into:");
    GDsetTextSize(3);
    M5.Lcd.setCursor(30, 100);
    if (updating_flash)
    {
      M5.Lcd.print("UPDATING: Fw");
    }
    else
    {
      M5.Lcd.print("UPDATING: UI");
    }
    M5.Lcd.drawRoundRect(20, 130, 280, 20, 10, white_colorx);
    M5.Lcd.setCursor(30, 200);
    M5.Lcd.println("The unit will reset");
    M5.Lcd.setCursor(30, 235);
    M5.Lcd.print("at end of prog. bar");
    // GDsetTextSize(2);
    // M5.Lcd.setCursor(30, 229);
    // M5.Lcd.println("(or: glidertimer.local )");
    myfirsttime = 0;
  }
  else
  {

    M5.Lcd.fillRoundRect(20, 130, pct, 20, 10, white_colorx);
  }
  // M5.Lcd.setTextSize(3);
}

void GTdisplay::display_fltdata(struct datastore fltdata)
{
  // uint16_t RGB2TFT(r,g,b);
  uint16_t RR = 2;
  uint16_t GG = 64;
  uint16_t BB = 0;
  RR = RR >> 3;
  GG = GG >> 2;
  BB = BB >> 3;
  RR = RR << 11;
  GG = GG << 5;
  BB = BB;

  // background_colorx = RR + GG + BB;
  M5.Lcd.fillScreen(background_colorx);
  // M5.Lcd.drawJpgFile(SPIFFS, "/LogoKeeper0.jpg", 8, 8);
  M5.Lcd.setTextColor(white_colorx);
  M5.Lcd.setCursor(10, 15);
  // M5.Lcd.setTextSize(2);
  M5.Lcd.setFreeFont(FSSB12);
  // M5.Lcd.print("       Glider");
  // M5.Lcd.print("");
  // M5.Lcd.println("Timer");
  // M5.Lcd.setTextSize(1);

  M5.Lcd.setCursor(60, 30);
  GDsetTextSize(3);
  M5.Lcd.print("C#");
  M5.Lcd.print(GT.stored_competition_no);
  M5.Lcd.print(" R#");
  M5.Lcd.print(GT.stored_round_no + 6 * GT.comp_step);
  if (GT.receiving_working_time)
  {
    M5.Lcd.print(" G#");
    M5.Lcd.println(GT.stored_group_no);
  }
  else
  {
    M5.Lcd.print(" P#");
    M5.Lcd.println(FLTdata.tfd.pilot_no);
  }
  M5.Lcd.setCursor(4, 60);

  GDsetTextSize(2);
  if (GT.receiving_working_time)
  {
    M5.Lcd.print("TO:");
  }
  else
  {
    M5.Lcd.print("from");
  }

  M5.Lcd.setFreeFont(FSSB12);
  M5.Lcd.print(FLTdata.tfd.serialnumber);
  GDsetTextSize(2);
  M5.Lcd.setCursor(185, 60);
  M5.Lcd.print(FLTdata.tfd.Firm_ver);
  // M5.Lcd.setTextSize(3);
  // GDsetTextSize(3);
  M5.Lcd.setCursor(50, 100);
  M5.Lcd.setFreeFont(FSSB18);

  if (GT.receiving_working_time)
  {
    M5.Lcd.print("Round");
    M5.Lcd.println("    Group");
  }
  else
  {
    M5.Lcd.print("F5Jh");
    M5.Lcd.println("      Time");
  }
  // M5.Lcd.setTextSize(2);

  // M5.Lcd.setTextSize(5);
  M5.Lcd.setCursor(45, 152); // era 116
                             //  GDsetTextSize(6);
                             // M5.Lcd.setTextFont(8);

  M5.Lcd.setFreeFont(FSSB24);
  String padeable;

  if (GT.receiving_working_time)
  {
    padeable = "    " + String(FLTdata.tfd.round_no) + "        " + String(FLTdata.tfd.group_no);
    M5.Lcd.println(padeable);
  }
  else
  {
    float F5Jh = (float)(FLTdata.tfd.F5J_height) / 10;
    padeable = "    " + String(F5Jh, 1);
    padeable = padeable.substring(padeable.length() - 5);
    M5.Lcd.print(padeable);
    M5.Lcd.setCursor(190, 152);
    if (fltdata.flight_mins < 99)
    {
      M5.Lcd.print(FLTdata.tfd.flight_mins);
      M5.Lcd.print(":");
      padeable = "00" + String(FLTdata.tfd.flight_secs);
      padeable = padeable.substring(padeable.length() - 2);
    }
    else
    {
      padeable = "**:**";
    }
    M5.Lcd.println(padeable);
  }

  // M5.Lcd.setTextSize(2);
  GDsetTextSize(2);

  M5.Lcd.println("");
  M5.Lcd.drawLine(20, 109, 300, 109, white_colorx);
  M5.Lcd.drawLine(20, 110, 300, 110, white_colorx);
  M5.Lcd.drawLine(160, 120, 160, 155, white_colorx);
  M5.Lcd.drawLine(20, 165, 300, 165, white_colorx);
  M5.Lcd.drawLine(20, 166, 300, 166, white_colorx);

  // M5.Lcd.drawRoundRect(20, 93, 130, 84, 5, white_colorx);
  // M5.Lcd.drawRoundRect(170, 93, 130, 84, 5, white_colorx);
}

void GTdisplay::display_toggle_name_in_keeper()
{
  static uint32_t timefortoggle = millis();
  static bool toggle = 1;
  if ((millis() - timefortoggle) > 3000)
  {
    toggle = !toggle;
    timefortoggle = millis();
    M5.Lcd.fillRect(3, 40, 310, 30, background_colorx);
    M5.Lcd.setCursor(4, 60);
    M5.Lcd.setFreeFont(FSSB12);
    if (toggle)
    {

      M5.Lcd.print("ID= ");
      M5.Lcd.println(String(FLTdata.tfd.serialnumber) + " " + String(FLTdata.tfd.Firm_ver));
    }
    else
    {
      if (GT.groups_enabled)
      {
        M5.Lcd.println(String(FLTdata.name_in_keeper).substring(0, 18));
        // M5.Lcd.println(String(GT.pilots_table[FLTdata.tfd.pilot_no].name).substring(0, 18));
      }
      else
      {
        int sn_intable = GT.find_sn_in_pilots_table(FLTdata.tfd.serialnumber);
        if (sn_intable != 0)
        {
          M5.Lcd.println(String(FLTdata.name_in_keeper));
          // M5.Lcd.println(String(GT.pilots_table[sn_intable].name));
        }
        else
        {
          M5.Lcd.println(String(FLTdata.name_in_keeper));
        }
      }
    }
  }
}

String GTdisplay::status_label()
{
  // String label;
  char bluff[25];
  if (GT.groups_enabled)
  {
    snprintf(bluff, 25, "C#%d R#%d G#%d", GT.current_competition_no, GT.current_round_no + 6 * GT.comp_step, GT.current_group_no);

    //  label ="C#" + String(GT.current_competition_no) + " R#" + String(GT.current_round_no)+" G#"+ String(GT.current_group_no);
  }
  else
  {
    snprintf(bluff, 25, " C#%d   R#%d", GT.current_competition_no, GT.current_round_no);
    //  label ="  C#" + String(GT.current_competition_no) + "   R#" + String(GT.current_round_no);
  }

  if (GT.scheduled)
  {
    struct timeval now;
    gettimeofday(&now, NULL);
    int secs_to_adv = flight.next_launch_epoch - now.tv_sec - SCHEDULED_PRE_NOTICE;
    if ((secs_to_adv > 0) && sched_adv_first_time)
    {
      sched_adv_first_time = 0;
      int minuts = secs_to_adv / 60;
      int secs = secs_to_adv % 60;
      if (minuts > 60)
      {
        snprintf(bluff, 25, "Scheduled ON");
      }
      else
      {
        snprintf(bluff, 25, "Adv in %d:%d", minuts, secs);
      }
      sched_adv_millis_for_partial = millis() + 350;
    }
  }
  if (local_RTC.keeper_Synchro_status < RTC_AVAILABLE)
  // if (!local_RTC.RTC_working)
  {

    snprintf(bluff, 25, "Clock FAILED!!");
  }
  // return label;
  return String(bluff);
}

void GTdisplay::partial_flt()
{
  M5.Lcd.setTextColor(white_colorx);
  M5.Lcd.setCursor(xapos, yapos);
  M5.Lcd.setTextSize(1);
  // M5.Lcd.setFreeFont(FSSO18);
  GDsetTextSize(3);
  M5.Lcd.println(alabel);

  if (alabel.length() < 14)
  {
    print_batt();
  }
  M5.Lcd.setCursor(xbpos, ybpos);
  if (blabel.length() == 0)
  {
    GDsetTextSize(2);
    // M5.Lcd.setFreeFont(FSSB9);
    M5.Lcd.setCursor(21, 236);
    M5.Lcd.println(button_label);
  }
  else
  {
    M5.Lcd.println(blabel);
  }
  // blabel = "";
}

void GTdisplay::print_batt()
{
  int wid = 41;
  int hig = 16;
  int despl = 240;
  int xbat = xapos + despl;
  int ybat = yapos - 20 + 2;
  bat_level = M5.Power.getBatteryLevel();
  int color_bat = white_colorx;

  if (bat_level < 50)
    color_bat = red;

  M5.Lcd.fillRoundRect(xbat, ybat, wid, hig, 4, color_bat);
  M5.Lcd.fillRoundRect(xbat + wid - 1, ybat + hig / 4, 3, hig / 2, 3, color_bat);
  if (bat_level == 50)
    color_bat = YELLOW;
  M5.Lcd.fillRoundRect(xbat + 2, ybat + 2, wid - 4, hig - 4, 2, background_colorx);

  if (bat_level > 0)
    M5.Lcd.fillRoundRect(xbat + 3, ybat + 3, 8, hig - 6, 1, color_bat);

  if (bat_level > 25)
    M5.Lcd.fillRoundRect(xbat + 3 + 9, ybat + 3, 8, hig - 6, 1, color_bat);
  if (bat_level > 50)
    M5.Lcd.fillRoundRect(xbat + 3 + 18, ybat + 3, 8, hig - 6, 1, white_colorx);
  if (bat_level > 75)
    M5.Lcd.fillRoundRect(xbat + 3 + 27, ybat + 3, 8, hig - 6, 1, white_colorx);
}

void GTdisplay::clean_partial_flt()
{
  // M5.Lcd.setTextColor(background_colorx);
  // M5.Lcd.setCursor(xapos, yapos);
  // M5.Lcd.setTextSize(1);
  // GDsetTextSize(3);
  // // M5.Lcd.setFreeFont(FSSO18);

  // M5.Lcd.println(alabel);
  M5.Lcd.fillRect(0, yapos - 30, 320, 270 - yapos, background_colorx);
  // M5.Lcd.fillRect(xapos-30, yapos, 320, 30, background_colorx);

  // M5.Lcd.setCursor(xbpos, ybpos);
  // M5.Lcd.setTextColor(background_colorx);
  // if (blabel.length() == 0)
  // {
  //   GDsetTextSize(2);
  //   // M5.Lcd.setFreeFont(FSSB9);
  //   M5.Lcd.setCursor(21, 236);
  //   M5.Lcd.println(button_label);
  // }
  // else
  // {
  //   M5.Lcd.println(blabel);
  // }
  // M5.Lcd.setTextColor(white_colorx);
}

void GTdisplay::display_status()
{

  M5.Lcd.fillScreen(black_colorx);
  M5.Lcd.setFreeFont(FMB9);
  M5.lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(0, 20);
  M5.Lcd.println("Glider Timer " + versionkeeper);
  sprintf(tc, "SETTINGS    GRPS: %d Collctr %d", GT.groups_enabled, GT.collector);
  M5.Lcd.println(tc);
  sprintf(tc, "Sched: %d  Strict: %d  PnelT: %d", GT.scheduled, GT.strict_timing, flight.panel_type);
  M5.Lcd.println(tc);
  sprintf(tc, "AdvertVol: %d BackGVol: %d ", MP3.adv_vol, MP3.music_vol);
  M5.Lcd.println(tc);
  M5.Lcd.println();
  sprintf(tc, "COMPETTION");
  M5.Lcd.println(tc);
  sprintf(tc, "ROUNDS %d GROUPS %d PILOTS %d", GT.mycompetition.head.number_of_rounds, GT.mycompetition.head.number_of_groups, GT.mycompetition.head.number_of_pilots);
  M5.Lcd.println(tc);
  sprintf(tc, "PROGRESS:");
  M5.Lcd.println(tc);
  sprintf(tc, "R#: %d G# %d ", GT.stored_round_no, GT.stored_group_no);
  M5.Lcd.println(tc);
  struct timeval now;
  gettimeofday(&now, NULL);
  int remainin = flight.next_launch_epoch - now.tv_sec;
  if (remainin < 0)
  {
    remainin = 0;
  }
  sprintf(tc, " NEXT LAUNCH in %d min %d sec ", remainin / 60, remainin % 60);
  M5.Lcd.println(tc);

  while (buttonA == 0)
  {
    delay(50);
    GT.button_update();
  }
}

void GTdisplay::clear_thdisplay()
{
  M5.Lcd.setTextColor(white_colorx, background_colorx);
  M5.Lcd.fillScreen(white_colorx);
}

///////////////////////////////////////////
// Funcion Mostrar pantalla Medidas
////////////////////////////////////////

void GTdisplay::displayThrow()
{

  // String payload;
  // int index;
  // M5.Lcd.setRotation(1);
  // switch (gth.throw_status)
  // {
  // case ANGLEVIEW:
  // {

  //   if (gth.last_throw_status != ANGLEVIEW)
  //   {
  //     draw_angleview_template();
  //     gth.last_throw_status = ANGLEVIEW;
  //     draw_angleview();
  //   }
  //   erase_angleview();
  //   draw_angleview();
  // }
  // break;
  // case THROWVIEW:
  // {
  //   if (gth.last_throw_status != THROWVIEW)
  //   {
  //     draw_throwview_template();
  //     gth.last_throw_status = THROWVIEW;
  //     draw_angleview();
  //   }
  //   erase_angleview();
  //   draw_angleview();
  // }
  // break;
  // case ANGLEVIEW3:
  // {

  //   if (gth.last_throw_status != ANGLEVIEW3)
  //   {
  //     draw_angleview3_template();
  //     gth.last_throw_status = ANGLEVIEW3;
  //     draw_angleview();
  //   }
  //   erase_angleview();
  //   draw_angleview();
  // }
  // break;
  // case ANGLEVIEW4:
  // {

  //   if (gth.last_throw_status != ANGLEVIEW4)
  //   {
  //     draw_angleview4_template();
  //     gth.last_throw_status = ANGLEVIEW4;
  //     draw_angleview4();
  //   }
  //   draw_angleview4();
  // }
  // break;
  // case THROWVIEW3:
  // {
  //   if (gth.last_throw_status != THROWVIEW3)
  //   {
  //     draw_throwview3_template();
  //     gth.last_throw_status = THROWVIEW3;
  //     draw_angleview();
  //   }
  //   erase_angleview();
  //   draw_angleview();
  // }
  // break;
  // case THROWVIEW4:
  // {
  //   if (gth.last_throw_status != THROWVIEW4)
  //   {
  //     draw_throwview4_template();
  //     gth.last_throw_status = THROWVIEW4;
  //     draw_angleview4();
  //   }

  //   draw_angleview4();
  // }
  // break;
  // case CHORD_SET_1:
  // {
  //   if (gth.last_throw_status != CHORD_SET_1)
  //   {
  //     gth.client_request();
  //     gth.split_payload(); ///access only first time
  //     draw_chord1_template();
  //     gth.last_throw_status = CHORD_SET_1;
  //   }
  //   erase_chord1();
  //   draw_chord1();
  // }
  // break;
  // case CHORD_SET_2:
  // {
  //   if (gth.last_throw_status != CHORD_SET_2)
  //   {
  //     draw_chord2_template();
  //     gth.last_throw_status = CHORD_SET_2;
  //   }
  //   erase_chord2();
  //   draw_chord2();
  // }
  // break;
  // case CHORD_SET_3:
  // {
  //   if (gth.last_throw_status != CHORD_SET_3)
  //   {
  //     gth.client_request();
  //     gth.split_payload(); ///access only first time
  //     draw_chord3_template();
  //     gth.last_throw_status = CHORD_SET_3;
  //   }
  //   erase_chord1();
  //   draw_chord1();
  // }
  // break;
  // case CHORD_SET_4:
  // {
  //   if (gth.last_throw_status != CHORD_SET_4)
  //   {
  //     draw_chord4_template();
  //     gth.last_throw_status = CHORD_SET_4;
  //   }
  //   erase_chord2();
  //   draw_chord2();
  // }
  // break;
  // case ANGLE_RESET:
  // {
  //   gth.client_request();
  //   gth.throw_status = ANGLEVIEW;
  // }
  // break;
  // }
}

void GTdisplay::draw_angleview_template()
{
  // /////////////////////////////////////////////////////////////////////////
  // ////// AQUI TOCA MARTA
  // /////////////////////////////////////////////////////////////////////////
  // clear_thdisplay();
  // M5.Lcd.setTextColor(black_colorx);
  // M5.Lcd.setTextSize(1);
  // M5.Lcd.setFreeFont(FSSB18);
  // M5.Lcd.setCursor(57, 30);
  // M5.Lcd.println("GliderThrow");
  // M5.Lcd.println();
  // M5.Lcd.setCursor(25, 60);
  // M5.Lcd.print("AngleView (deg)");
  // M5.Lcd.setFreeFont(FMB18);
  // M5.Lcd.fillRoundRect(46, 69, 229, 40, 4, red);
  // M5.Lcd.setTextColor(white_colorx, red);
  // M5.Lcd.setCursor(57, 98);
  // M5.Lcd.print("");
  // M5.Lcd.print("S1 = ");
  // M5.Lcd.setCursor(162, 98);
  // // M5.Lcd.print(label1);
  // M5.Lcd.fillRoundRect(46, 117, 229, 40, 4, green1);
  // M5.Lcd.setTextColor(white_colorx, green1);
  // M5.Lcd.setCursor(57, 147);
  // M5.Lcd.print("S2 = ");
  // M5.Lcd.setCursor(162, 147);
  // // M5.Lcd.print(label2);
  // M5.Lcd.fillRoundRect(46, 167, 229, 40, 4, blue);
  // M5.Lcd.setTextColor(white_colorx, blue);
  // M5.Lcd.setCursor(57, 197);
  // M5.Lcd.print("Diff ");
  // M5.Lcd.setCursor(162, 197);
  // // M5.Lcd.print(label3);
  // M5.Lcd.setCursor(45, 232);
  // M5.Lcd.setTextColor(black_colorx);
  // M5.Lcd.setFreeFont(FSSB9);
  // M5.Lcd.setTextSize(1);
  // //M5.Lcd.print("Quad");
  // M5.Lcd.setCursor(130, 232);
  // M5.Lcd.print("Throw");
  // M5.Lcd.setCursor(221, 232);
  // M5.Lcd.print("Reset Angles");
}

void GTdisplay::erase_angleview()
{
  // M5.Lcd.setTextSize(1);
  // M5.Lcd.setTextColor(red);
  // M5.Lcd.setFreeFont(FMB18);
  // M5.Lcd.setCursor(162, 98);
  // M5.Lcd.print(vlabel1);
  // M5.Lcd.setTextColor(green1);
  // M5.Lcd.setCursor(162, 147);
  // M5.Lcd.print(vlabel2);
  // M5.Lcd.setTextColor(blue);
  // M5.Lcd.setCursor(162, 197);
  // M5.Lcd.print(vlabel3);
}

void GTdisplay::draw_angleview()
{
  // M5.Lcd.setTextSize(1);
  // M5.Lcd.setFreeFont(FMB18);
  // M5.Lcd.setTextColor(white_colorx, red);
  // M5.Lcd.setCursor(162, 98);
  // M5.Lcd.print(label1);
  // M5.Lcd.setTextColor(white_colorx, green1);
  // M5.Lcd.setCursor(162, 147);
  // M5.Lcd.print(label2);
  // M5.Lcd.setTextColor(white_colorx, blue);
  // M5.Lcd.setCursor(162, 197);
  // M5.Lcd.print(label3);
  // // label1_v = label1;
  // // label2_v = label2;
  // label1.toCharArray(vlabel1, 6);
  // label2.toCharArray(vlabel2, 6);
  // label3.toCharArray(vlabel3, 6);
  // // label3_v = label3;
}

void GTdisplay::draw_throwview_template()
{
  // clear_thdisplay();
  // M5.Lcd.setCursor(57, 30);
  // M5.Lcd.setTextColor(black_colorx);
  // M5.Lcd.setTextSize(1);
  // M5.Lcd.setFreeFont(FSSB18);
  // M5.Lcd.println("GliderThrow");
  // M5.Lcd.println();
  // M5.Lcd.setCursor(25, 60);
  // M5.Lcd.print("Throw(mm)View");
  // M5.Lcd.setFreeFont(FMB18);
  // M5.Lcd.fillRoundRect(46, 69, 229, 40, 4, red);
  // M5.Lcd.setTextColor(white_colorx, red);
  // M5.Lcd.setCursor(57, 98);
  // M5.Lcd.print("");
  // M5.Lcd.print("S1 = ");
  // M5.Lcd.setCursor(162, 98);
  // //M5.Lcd.print(label1);
  // M5.Lcd.fillRoundRect(46, 117, 229, 40, 4, green1);
  // M5.Lcd.setTextColor(white_colorx, green1);
  // M5.Lcd.setCursor(57, 147);
  // M5.Lcd.print("S2 = ");
  // M5.Lcd.setCursor(162, 147);
  // //M5.Lcd.print(label2);
  // M5.Lcd.fillRoundRect(46, 167, 229, 40, 4, blue);
  // M5.Lcd.setTextColor(white_colorx, blue);
  // M5.Lcd.setCursor(57, 197);
  // M5.Lcd.print("Diff ");
  // M5.Lcd.setCursor(162, 197);
  // //M5.Lcd.print(label3);
  // M5.Lcd.setCursor(38, 232);
  // M5.Lcd.setTextColor(black_colorx);
  // M5.Lcd.setFreeFont(FSSB9);
  // M5.Lcd.setTextSize(1);
  // M5.Lcd.print("Chord");
  // M5.Lcd.setCursor(130, 232);
  // M5.Lcd.print("Angle");
  // M5.Lcd.setCursor(221, 232);
  // M5.Lcd.print("Reset Angles");
}

void GTdisplay::draw_chord1_template()
{
  // clear_thdisplay();
  // M5.Lcd.setCursor(57, 30);
  // M5.Lcd.setTextColor(black_colorx);
  // M5.Lcd.setTextSize(1);
  // M5.Lcd.setFreeFont(FSSB18);
  // M5.Lcd.println("GliderThrow");
  // M5.Lcd.println();
  // M5.Lcd.setCursor(50, 63);
  // M5.Lcd.print("Chord Config");
  // M5.Lcd.fillRoundRect(8, 82, 206, 47, 4, red);
  // M5.Lcd.setTextColor(white_colorx, red);
  // M5.Lcd.setCursor(19, 114);
  // M5.Lcd.setFreeFont(FSSB12);
  // M5.Lcd.print("S1 Chord (mm)");
  // M5.Lcd.fillRoundRect(229, 82, 69, 47, 4, red);
  // M5.Lcd.setCursor(242, 117);
  // //M5.Lcd.print(label1);
  // M5.Lcd.fillRoundRect(8, 147, 206, 47, 4, grey);
  // M5.Lcd.setTextColor(white_colorx, grey);
  // M5.Lcd.setCursor(19, 179);
  // M5.Lcd.print("S2 Chord (mm)");
  // M5.Lcd.fillRoundRect(229, 147, 69, 47, 4, grey);
  // M5.Lcd.setCursor(242, 182);
  // //M5.Lcd.print(label2);
  // M5.Lcd.setCursor(54, 232);
  // M5.Lcd.setTextColor(black_colorx);
  // M5.Lcd.setFreeFont(FSSB12);
  // M5.Lcd.print("( - )");
  // M5.Lcd.setCursor(147, 232);
  // M5.Lcd.print("( + )");
  // M5.Lcd.setFreeFont(FSSB9);
  // M5.Lcd.setCursor(236, 232);
  // M5.Lcd.setTextSize(1);
  // M5.Lcd.print("Next");
  // M5.Lcd.setFreeFont(FSSB18);
  // draw_chord1();
}

void GTdisplay::erase_chord1()
{
  // M5.Lcd.setTextColor(red, red);
  // M5.Lcd.setCursor(242, 117);
  // M5.Lcd.print(vlabel1);
  // M5.Lcd.setTextColor(grey, grey);
  // M5.Lcd.setCursor(242, 182);
  // M5.Lcd.print(vlabel2);
}

void GTdisplay::draw_chord1()
{
  // M5.Lcd.setTextColor(white_colorx, red);
  // M5.Lcd.setCursor(242, 117);
  // M5.Lcd.print(label1);
  // M5.Lcd.setTextColor(white_colorx, grey);
  // M5.Lcd.setCursor(242, 182);
  // M5.Lcd.print(label2);
  // label1.toCharArray(vlabel1, 6);
  // label2.toCharArray(vlabel2, 6);
  // // label1_v = label1;
  // // label2_v = label2;
}

void GTdisplay::draw_chord2_template()
{
  // clear_thdisplay();
  // M5.Lcd.setCursor(57, 30);
  // M5.Lcd.setTextColor(black_colorx);
  // M5.Lcd.setTextSize(1);
  // M5.Lcd.setFreeFont(FSSB18);
  // M5.Lcd.println("GliderThrow");
  // M5.Lcd.println();
  // M5.Lcd.setCursor(50, 63);
  // M5.Lcd.print("Chord Config");
  // M5.Lcd.fillRoundRect(8, 82, 206, 47, 4, grey);
  // M5.Lcd.setTextColor(white_colorx, grey);
  // M5.Lcd.setCursor(19, 114);
  // M5.Lcd.setFreeFont(FSSB12);
  // M5.Lcd.print("S1 Chord(mm)");
  // M5.Lcd.fillRoundRect(229, 82, 69, 47, 4, grey);
  // M5.Lcd.setCursor(242, 117);
  // //M5.Lcd.print(label1_v);
  // M5.Lcd.fillRoundRect(8, 147, 206, 47, 4, green1);
  // M5.Lcd.setTextColor(white_colorx, green1);
  // M5.Lcd.setCursor(19, 179);
  // M5.Lcd.print("S2 Chord(mm)");
  // M5.Lcd.fillRoundRect(229, 147, 69, 47, 4, green1);
  // M5.Lcd.setCursor(242, 182);
  // //M5.Lcd.print(label2_v);
  // M5.Lcd.setCursor(54, 232);
  // M5.Lcd.setTextColor(black_colorx);
  // M5.Lcd.setFreeFont(FSSB12);
  // M5.Lcd.print("( - )");
  // M5.Lcd.setCursor(147, 232);
  // M5.Lcd.print("( + )");
  // M5.Lcd.setCursor(236, 232);
  // M5.Lcd.setTextSize(1);
  // M5.Lcd.setFreeFont(FSSB9);
  // M5.Lcd.print("Next");
  // M5.Lcd.setFreeFont(FSSB18);
  // draw_chord2();
}

void GTdisplay::erase_chord2()
{
  // M5.Lcd.setTextColor(grey, grey);
  // M5.Lcd.setCursor(242, 117);
  // M5.Lcd.print(vlabel1);
  // M5.Lcd.setTextColor(green1, green1);
  // M5.Lcd.setCursor(242, 182);
  // M5.Lcd.print(vlabel2);
}

void GTdisplay::draw_chord2()
{
  // M5.Lcd.setFreeFont(FSSB18);
  // M5.Lcd.setTextColor(white_colorx, grey);
  // M5.Lcd.setCursor(242, 117);
  // M5.Lcd.print(label1);
  // M5.Lcd.setTextColor(white_colorx, green1);
  // M5.Lcd.setCursor(242, 182);
  // M5.Lcd.print(label2);
  // label1.toCharArray(vlabel1, 6);
  // label2.toCharArray(vlabel2, 6);
  // label1_v = label1;
  // label2_v = label2;
}

void GTdisplay::draw_angleview3_template()
{
  // clear_thdisplay();
  // M5.Lcd.setTextColor(black_colorx);
  // M5.Lcd.setTextSize(1);
  // M5.Lcd.setFreeFont(FSSB18);
  // M5.Lcd.setCursor(57, 30);
  // M5.Lcd.println("GliderThrow");
  // M5.Lcd.println();
  // M5.Lcd.setCursor(25, 60);
  // M5.Lcd.print("AngleView (deg)");
  // M5.Lcd.setFreeFont(FMB18);
  // M5.Lcd.fillRoundRect(46, 69, 229, 40, 4, red);
  // M5.Lcd.setTextColor(white_colorx, red);
  // M5.Lcd.setCursor(57, 98);
  // M5.Lcd.print("");
  // M5.Lcd.print("S3 = ");
  // M5.Lcd.setCursor(162, 98);
  // // M5.Lcd.print(label1);
  // M5.Lcd.fillRoundRect(46, 117, 229, 40, 4, green1);
  // M5.Lcd.setTextColor(white_colorx, green1);
  // M5.Lcd.setCursor(57, 147);
  // M5.Lcd.print("S4 = ");
  // M5.Lcd.setCursor(162, 147);
  // // M5.Lcd.print(label2);
  // M5.Lcd.fillRoundRect(46, 167, 229, 40, 4, blue);
  // M5.Lcd.setTextColor(white_colorx, blue);
  // M5.Lcd.setCursor(57, 197);
  // M5.Lcd.print("Diff ");
  // M5.Lcd.setCursor(162, 197);
  // // M5.Lcd.print(label3);
  // M5.Lcd.setCursor(45, 232);
  // M5.Lcd.setTextColor(black_colorx);
  // M5.Lcd.setFreeFont(FSSB9);
  // M5.Lcd.setTextSize(1);
  // M5.Lcd.print("Quad");
  // M5.Lcd.setCursor(130, 232);
  // M5.Lcd.print("Throw");
  // M5.Lcd.setCursor(221, 232);
  // M5.Lcd.print("Reset Angles");
}

void GTdisplay::draw_throwview3_template()
{
  // clear_thdisplay();
  // M5.Lcd.setCursor(57, 30);
  // M5.Lcd.setTextColor(black_colorx);
  // M5.Lcd.setTextSize(1);
  // M5.Lcd.setFreeFont(FSSB18);
  // M5.Lcd.println("GliderThrow");
  // M5.Lcd.println();
  // M5.Lcd.setCursor(25, 60);
  // M5.Lcd.print("Throw(mm)View");
  // M5.Lcd.setFreeFont(FMB18);
  // M5.Lcd.fillRoundRect(46, 69, 229, 40, 4, red);
  // M5.Lcd.setTextColor(white_colorx, red);
  // M5.Lcd.setCursor(57, 98);
  // M5.Lcd.print("");
  // M5.Lcd.print("S3 = ");
  // M5.Lcd.setCursor(162, 98);
  // //M5.Lcd.print(label1);
  // M5.Lcd.fillRoundRect(46, 117, 229, 40, 4, green1);
  // M5.Lcd.setTextColor(white_colorx, green1);
  // M5.Lcd.setCursor(57, 147);
  // M5.Lcd.print("S3 = ");
  // M5.Lcd.setCursor(162, 147);
  // //M5.Lcd.print(label2);
  // M5.Lcd.fillRoundRect(46, 167, 229, 40, 4, blue);
  // M5.Lcd.setTextColor(white_colorx, blue);
  // M5.Lcd.setCursor(57, 197);
  // M5.Lcd.print("Diff ");
  // M5.Lcd.setCursor(162, 197);
  // //M5.Lcd.print(label3);
  // M5.Lcd.setCursor(38, 232);
  // M5.Lcd.setTextColor(black_colorx);
  // M5.Lcd.setFreeFont(FSSB9);
  // M5.Lcd.setTextSize(1);
  // M5.Lcd.print("Chord");
  // M5.Lcd.setCursor(130, 232);
  // M5.Lcd.print("Angle");
  // M5.Lcd.setCursor(221, 232);
  // M5.Lcd.print("Reset Angles");
}

void GTdisplay::draw_chord3_template()
{
  // clear_thdisplay();
  // M5.Lcd.setCursor(57, 30);
  // M5.Lcd.setTextColor(black_colorx);
  // M5.Lcd.setTextSize(1);
  // M5.Lcd.setFreeFont(FSSB18);
  // M5.Lcd.println("GliderThrow");
  // M5.Lcd.println();
  // M5.Lcd.setCursor(50, 63);
  // M5.Lcd.print("Chord Config");
  // M5.Lcd.fillRoundRect(8, 82, 206, 47, 4, red);
  // M5.Lcd.setTextColor(white_colorx, red);
  // M5.Lcd.setCursor(19, 114);
  // M5.Lcd.setFreeFont(FSSB12);
  // M5.Lcd.print("S3 Chord (mm)");
  // M5.Lcd.fillRoundRect(229, 82, 69, 47, 4, red);
  // M5.Lcd.setCursor(242, 117);
  // //M5.Lcd.print(label1);
  // M5.Lcd.fillRoundRect(8, 147, 206, 47, 4, grey);
  // M5.Lcd.setTextColor(white_colorx, grey);
  // M5.Lcd.setCursor(19, 179);
  // M5.Lcd.print("S4 Chord (mm)");
  // M5.Lcd.fillRoundRect(229, 147, 69, 47, 4, grey);
  // M5.Lcd.setCursor(242, 182);
  // //M5.Lcd.print(label2);
  // M5.Lcd.setCursor(54, 232);
  // M5.Lcd.setTextColor(black_colorx);
  // M5.Lcd.setFreeFont(FSSB12);
  // M5.Lcd.print("( - )");
  // M5.Lcd.setCursor(147, 232);
  // M5.Lcd.print("( + )");
  // M5.Lcd.setFreeFont(FSSB9);
  // M5.Lcd.setCursor(236, 232);
  // M5.Lcd.setTextSize(1);
  // M5.Lcd.print("Next");
  // M5.Lcd.setFreeFont(FSSB18);
  // draw_chord1();
}

void GTdisplay::draw_chord4_template()
{
  // clear_thdisplay();
  // M5.Lcd.setCursor(57, 30);
  // M5.Lcd.setTextColor(black_colorx);
  // //M5.Lcd.setCursor(xapos, yapos);
  // M5.Lcd.setTextSize(1);
  // M5.Lcd.setFreeFont(FSSB18);
  // M5.Lcd.println("GliderThrow");
  // M5.Lcd.println();
  // M5.Lcd.setCursor(50, 63);
  // M5.Lcd.print("Chord Config");
  // M5.Lcd.fillRoundRect(8, 82, 206, 47, 4, grey);
  // M5.Lcd.setTextColor(white_colorx, grey);
  // M5.Lcd.setCursor(19, 114);
  // M5.Lcd.setFreeFont(FSSB12);
  // M5.Lcd.print("S3 Chord(mm)");
  // M5.Lcd.fillRoundRect(229, 82, 69, 47, 4, grey);
  // M5.Lcd.setCursor(242, 117);
  // //M5.Lcd.print(label1_v);
  // M5.Lcd.fillRoundRect(8, 147, 206, 47, 4, green1);
  // M5.Lcd.setTextColor(white_colorx, green1);
  // M5.Lcd.setCursor(19, 179);
  // M5.Lcd.print("S4 Chord(mm)");
  // M5.Lcd.fillRoundRect(229, 147, 69, 47, 4, green1);
  // M5.Lcd.setCursor(242, 182);
  // //M5.Lcd.print(label2_v);
  // M5.Lcd.setCursor(54, 232);
  // M5.Lcd.setTextColor(black_colorx);
  // M5.Lcd.setFreeFont(FSSB12);
  // M5.Lcd.print("( - )");
  // M5.Lcd.setCursor(147, 232);
  // M5.Lcd.print("( + )");
  // M5.Lcd.setCursor(236, 232);
  // M5.Lcd.setTextSize(1);
  // M5.Lcd.setFreeFont(FSSB9);
  // M5.Lcd.print("Next");
  // M5.Lcd.setFreeFont(FSSB18);
  // draw_chord2();
}

void GTdisplay::draw_angleview4_template()
{
  //   clear_thdisplay();

  //   M5.Lcd.setCursor(38, 1);
  //   M5.Lcd.setTextColor(black_colorx);
  //   //M5.Lcd.setCursor(xapos, yapos);
  //   M5.Lcd.setFreeFont(FSSB18);
  //   M5.Lcd.setCursor(57, 30);
  //   M5.Lcd.println("GliderThrow");
  //   M5.Lcd.println();
  //   M5.Lcd.setCursor(25, 60);
  //   M5.Lcd.print("AngleView (deg)");

  //   M5.Lcd.fillRoundRect(8, 69, 146, 40, 4, red);
  //   M5.Lcd.fillRoundRect(160, 69, 148, 40, 4, red);
  //   M5.Lcd.fillRoundRect(8, 117, 146, 40, 4, green1);
  //   M5.Lcd.fillRoundRect(160, 117, 148, 40, 4, green1);
  //   M5.Lcd.fillRoundRect(8, 167, 146, 40, 4, blue);
  //   M5.Lcd.fillRoundRect(160, 167, 148, 40, 4, blue);

  //   M5.Lcd.setFreeFont(FSSB12);
  //   M5.Lcd.setTextColor(white_colorx);
  //   M5.Lcd.setCursor(15, 98);
  //   M5.Lcd.print("");
  //   M5.Lcd.print("S1=");
  //   M5.Lcd.setCursor(168, 98);
  //   M5.Lcd.print("S3=");
  //   M5.Lcd.setCursor(15, 147);
  //   M5.Lcd.print("S2=");
  //   M5.Lcd.setCursor(168, 147);
  //   M5.Lcd.print("S4=");
  //   M5.Lcd.setCursor(15, 197);
  //   M5.Lcd.print("Diff ");
  //   M5.Lcd.setCursor(168, 197);
  //   M5.Lcd.print("Diff ");
  //   M5.Lcd.setFreeFont(FSSB18);
  //   M5.Lcd.setCursor(76, 98);
  //   M5.Lcd.print(label1);
  //   M5.Lcd.setCursor(230, 98);
  //   M5.Lcd.print(label4);
  //   M5.Lcd.setCursor(76, 147);
  //   M5.Lcd.print(label2);
  //   M5.Lcd.setCursor(230, 147);
  //   M5.Lcd.print(label5);
  //   M5.Lcd.setCursor(76, 197);
  //   M5.Lcd.print(label3);
  //   M5.Lcd.setCursor(230, 197);
  //   M5.Lcd.print(label6);

  //   M5.Lcd.setFreeFont(FSSB9);
  //   M5.Lcd.setCursor(15, 232);
  //   M5.Lcd.setTextColor(black_colorx);
  //   M5.Lcd.print("Standard");
  //   M5.Lcd.setCursor(130, 232);
  //   M5.Lcd.print("Throw");
  //   M5.Lcd.setCursor(221, 232);
  //   M5.Lcd.print("Reset Angles");
  //   M5.Lcd.setFreeFont(FSSB18);
}

void GTdisplay::draw_angleview4()
{
  // M5.Lcd.setTextColor(white_colorx);
  // M5.Lcd.fillRoundRect(76, 69, 78, 40, 4, red);
  // M5.Lcd.setCursor(76, 99);
  // M5.Lcd.print(label1);
  // M5.Lcd.fillRoundRect(229, 69, 79, 40, 4, red);
  // M5.Lcd.setCursor(230, 99);
  // M5.Lcd.print(label4);
  // M5.Lcd.fillRoundRect(76, 117, 78, 40, 4, green1);
  // M5.Lcd.setCursor(76, 148);
  // M5.Lcd.print(label2);
  // M5.Lcd.fillRoundRect(229, 117, 79, 40, 4, green1);
  // M5.Lcd.setCursor(230, 148);
  // M5.Lcd.print(label5);
  // M5.Lcd.fillRoundRect(76, 167, 78, 40, 4, blue);
  // M5.Lcd.setCursor(76, 197);
  // M5.Lcd.print(label3);
  // M5.Lcd.fillRoundRect(229, 167, 79, 40, 4, blue);
  // M5.Lcd.setCursor(230, 197);
  // M5.Lcd.print(label6);
}

void GTdisplay::draw_throwview4_template()
{
  // clear_thdisplay();
  // M5.Lcd.setCursor(38, 1);
  // M5.Lcd.setTextColor(black_colorx);
  // //M5.Lcd.setCursor(xapos, yapos);
  // M5.Lcd.setFreeFont(FSSB18);
  // M5.Lcd.setCursor(57, 30);
  // M5.Lcd.println("GliderThrow");
  // M5.Lcd.println();
  // M5.Lcd.setCursor(25, 60);
  // M5.Lcd.print("Throw(mm)View");

  // M5.Lcd.fillRoundRect(8, 69, 146, 40, 4, red);
  // M5.Lcd.fillRoundRect(160, 69, 148, 40, 4, red);
  // M5.Lcd.fillRoundRect(8, 117, 146, 40, 4, green1);
  // M5.Lcd.fillRoundRect(160, 117, 148, 40, 4, green1);
  // M5.Lcd.fillRoundRect(8, 167, 146, 40, 4, blue);
  // M5.Lcd.fillRoundRect(160, 167, 148, 40, 4, blue);

  // M5.Lcd.setFreeFont(FSSB12);
  // M5.Lcd.setTextColor(white_colorx);
  // M5.Lcd.setCursor(15, 98);
  // M5.Lcd.print("");
  // M5.Lcd.print("S1=");
  // M5.Lcd.setCursor(168, 98);
  // M5.Lcd.print("S3=");
  // M5.Lcd.setCursor(15, 147);
  // M5.Lcd.print("S2=");
  // M5.Lcd.setCursor(168, 147);
  // M5.Lcd.print("S4=");
  // M5.Lcd.setCursor(15, 197);
  // M5.Lcd.print("Diff ");
  // M5.Lcd.setCursor(168, 197);
  // M5.Lcd.print("Diff ");
  // M5.Lcd.setFreeFont(FSSB18);
  // M5.Lcd.setCursor(76, 98);
  // M5.Lcd.print(label1);
  // M5.Lcd.setCursor(230, 98);
  // M5.Lcd.print(label4);
  // M5.Lcd.setCursor(76, 147);
  // M5.Lcd.print(label2);
  // M5.Lcd.setCursor(230, 147);
  // M5.Lcd.print(label5);
  // M5.Lcd.setCursor(76, 197);
  // M5.Lcd.print(label3);
  // M5.Lcd.setCursor(230, 197);
  // M5.Lcd.print(label6);

  // M5.Lcd.setFreeFont(FSSB9);
  // M5.Lcd.setCursor(38, 232);
  // M5.Lcd.setTextColor(black_colorx);
  // //M5.Lcd.setTextSize(2);
  // M5.Lcd.print("Chord");
  // M5.Lcd.setCursor(130, 232);
  // M5.Lcd.print("Angle");
  // M5.Lcd.setCursor(221, 232);
  // M5.Lcd.print("Reset Angles");
  // M5.Lcd.setFreeFont(FSSB18);
}