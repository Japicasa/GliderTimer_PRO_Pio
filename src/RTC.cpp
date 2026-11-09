#include "Wire.h"
#include "RTC.h"

#define DS3231_I2C_ADDRESS 0x68
// Convert normal decimal numbers to binary coded decimal
uint8_t decToBcd(uint8_t val)
{
  return ((val / 10 * 16) + (val % 10));
}
// Convert binary coded decimal to normal decimal numbers
uint8_t bcdToDec(uint8_t val)
{
  return ((val / 16 * 10) + (val % 16));
}
// void setup()
// {
//   Wire.begin();
//   Serial.begin(9600);
//   // set the initial time here:
//   // DS3231 seconds, minutes, hours, day, date, month, year
//   // setDS3231time(30,42,21,4,26,11,14);
// }
String RTC::SetRTC()
{
  struct tm timeinfo;
  uint32_t Delta_Micros = micros();
  /////////////////////////
  struct timeval localtimeval;
  gettimeofday(&localtimeval, NULL);
  int lastsec = localtimeval.tv_sec;
  int times = 0;
  while (lastsec == localtimeval.tv_sec)
  {
    gettimeofday(&localtimeval, NULL);
    if (times == 0)
    {
      lastsec = localtimeval.tv_sec;
    }
    times++;
  } // aprox 600 microseconds per loop

  /////////////////////////////
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return "";
  }
  // Delta_Micros = micros();

  // sets time and date data to DS3231
  // String text=(const char)(&timeinfo, "%A, %B %d %Y %H:%M:%S").toString;

  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0);                               // set next input to start at the seconds register
  Wire.write(decToBcd(timeinfo.tm_sec));       // set seconds
  Wire.write(decToBcd(timeinfo.tm_min));       // set minutes
  Wire.write(decToBcd(timeinfo.tm_hour));      // set hours
  Wire.write(decToBcd(timeinfo.tm_wday + 1));  // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(timeinfo.tm_mday));      // set date (1 to 31)
  Wire.write(decToBcd((timeinfo.tm_mon + 1))); // set month
  int localyear = timeinfo.tm_year - 100;
  // Serial.print("===LocalYear : ");
  // Serial.println(localyear);
  Wire.write(decToBcd(localyear)); // set year (0 to 99)
  // Wire.endTransmission();

  ////// write this timeinfo in Alarm1 (address 07x)
  Wire.write(decToBcd(timeinfo.tm_sec));  // set seconds
  Wire.write(decToBcd(timeinfo.tm_min));  // set minutes
  Wire.write(decToBcd(timeinfo.tm_hour)); // set hours
  Wire.write(decToBcd(timeinfo.tm_mday)); // set date (1 to 31)
  Wire.write(old_drift);
  Wire.write(decToBcd((timeinfo.tm_mon + 1)));
  Wire.write(alarm2_date);

  if (Wire.endTransmission() != 0)
  {
    Serial.print("mal I2C de RTC en RTC set");
    return "RTCset FAILED";
  }
  else
  {
    write_status_reg();
  }
  // Delta_Micros = micros() - Delta_Micros;
  // Serial.print("RTC_grabado en micros: ");
  // Serial.println(Delta_Micros);
  String text = asctime(&timeinfo);
  text = text + "\n Stored in :" + String(Delta_Micros) + " microseconds";

  return text;
}

void RTC::ZeroRTC()
{

  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0); // set next input to start at the seconds register
  Wire.write(0);

  Wire.endTransmission();
  Serial.println("Successfull RTC time Zeroed");
}

struct tm *RTC::ReadRTC()
{
  uint32_t Delta_Micros = micros();
  for (int i = 0; i < 5; i++)
  {
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0); // set DS3231 register pointer to 00h
    if (Wire.endTransmission() != 0)
    {
      Serial.print("mal I2C de RTC");
      /// disp_top_line ="NOGKS F#"; ///TODO only keeper
      delay(5);
    }
    else
    {
      Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
      // request seven bytes of data from DS3231 starting from register 00h
      RTCtime.tm_sec = bcdToDec(Wire.read());
      // myRTCdata.second = bcdToDec(Wire.read() & 0x7f);
      RTCtime.tm_min = bcdToDec(Wire.read());
      RTCtime.tm_hour = bcdToDec(Wire.read() & 0x3f);
      RTCtime.tm_wday = (bcdToDec(Wire.read()) - 1);
      RTCtime.tm_mday = bcdToDec(Wire.read());
      RTCtime.tm_mon = (bcdToDec(Wire.read()) - 1);
      RTCtime.tm_year = 100 + bcdToDec(Wire.read());
      if (keeper_Synchro_status == 0)
      {
        keeper_Synchro_status = RTC_AVAILABLE;
      }
      RTCtimeval.tv_sec = mktime(&RTCtime); // devuelve el time_t correspondiente a al tm_RTCtime
      RTCtimeval.tv_usec = 0;
      if (RTCtimeval.tv_sec < SECONDS_2021)
      {
        // disp_top_line ="UTC!! F#"; ///TODO only keeper
        power_lost = 1;
      }
      PrintRTC();
    }
    if (keeper_Synchro_status > NO_RTC_AVAILABLE)
      break;
  }
  if (RTCtime.tm_sec > 61)
  {
    // ZeroRTC();
  }
  // PrintRTC();
  return &RTCtime;
}

void RTC::read_last_sync()
{
  uint32_t Delta_Micros = micros();

  for (int i = 0; i < 5; i++)
  {
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0x7); // set DS3231 register pointer to 07h
    if (Wire.endTransmission() != 0)
    {
      Serial.print("mal I2C de RTC");
      RTC_corrupted = 1;
      delay(5);
    }
    else
    {
      Wire.requestFrom(DS3231_I2C_ADDRESS, 7);
      // request seven bytes of data from DS3231 starting from register 00h
      last_sync_time.tm_sec = bcdToDec(Wire.read());
      // myRTCdata.second = bcdToDec(Wire.read() & 0x7f);
      last_sync_time.tm_min = bcdToDec(Wire.read());
      last_sync_time.tm_hour = bcdToDec(Wire.read() & 0x3f);
      // last_sync_time.tm_wday = (bcdToDec(Wire.read()) - 1);
      last_sync_time.tm_mday = bcdToDec(Wire.read());
      old_drift = Wire.read();
      last_sync_time.tm_mon = (bcdToDec(Wire.read()) - 1);
      alarm2_date = Wire.read();
      // RTCtime.tm_year = 100 + bcdToDec(Wire.read());
      if ((last_sync_time.tm_sec < 0) || (last_sync_time.tm_sec > 59))
      {
        RTC_corrupted = 1;
      }
      if ((last_sync_time.tm_min < 0) || (last_sync_time.tm_min > 59))
      {
        RTC_corrupted = 1;
      }
      if ((last_sync_time.tm_hour < 0) || (last_sync_time.tm_hour > 23))
      {
        RTC_corrupted = 1;
      }
      if ((last_sync_time.tm_mday < 1) || (last_sync_time.tm_mday > 31))
      {
        RTC_corrupted = 1;
      }
      if ((last_sync_time.tm_mon < 0) || (last_sync_time.tm_mon > 11))
      {
        RTC_corrupted = 1;
      }
    }
  }
 // Serial.print("Alarm1 is set to: ------> ");
  // Serial.println(&last_sync_time, "%A, %B %d %Y %H:%M:%S");
  last_sync_time.tm_year = RTCtime.tm_year;
  last_sync_val.tv_sec = mktime(&last_sync_time);
  int32_t elapsed_sec = RTCtimeval.tv_sec - last_sync_val.tv_sec;
  if (elapsed_sec > ELAPSED_FOR_SYNC)
  {
    keeper_Synchro_status = RTC_OUT_OF_SYNC_PERIOD;
  }
  Serial.println(&last_sync_time, " %B %d  %H:%M:%S");
}

struct tm *RTC::Read_Until_Sec()
{

  // Check if GKS is slept
  uint8_t control_reg = read_control_reg(); // and update hibernated flag
  // control_reg= control_reg & 0x80;
  if (control_reg > 0x79)
  {
    Serial.println("waking up");
    wake_up(); // writes in ctrl reg
  }

  uint32_t Delta_Micros = micros();
  int lastsec = RTCtime.tm_sec;
  int times = 0;
  while (lastsec == RTCtime.tm_sec)
  {
    delay(1);
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0); // set DS3231 register pointer to 00h
    if (Wire.endTransmission() != 0)
    {
      Serial.print("mal I2C de RTC read until sec");
      delay(5);
      //return &RTCtime;
    }
    Wire.requestFrom(DS3231_I2C_ADDRESS, 1);
    // request seven bytes of data from DS3231 starting from register 00h
    RTCtime.tm_sec = bcdToDec(Wire.read());
    if (times == 0)
    {
      lastsec = RTCtime.tm_sec;
    }
    times++;
    // yield();
  } // aprox 600 microseconds per loop

  // Delta_Micros = micros();
  ReadRTC();
  // Delta_Micros = micros() - Delta_Micros;
  //  Serial.print("secs o'clock  read in micros: ");
  //  Serial.println(Delta_Micros);
  return &RTCtime;
}

void RTC::PrintRTC()
{
  Serial.print("RTCtime: ------> ");
  Serial.println(&RTCtime, "%A, %B %d %Y %H:%M:%S");
}
void RTC::PrintRTCval()
{
  Serial.print("RTCtime: ;");
  Serial.print(RTCtimeval.tv_sec);
  Serial.print("; micros: ; ");
  Serial.print(RTCtimeval.tv_usec);
}
void RTC::PrintLocalTimeval()
{
  struct timeval localtimeval;
  gettimeofday(&localtimeval, NULL);
  Serial.print(" LocalTime: ;");
  Serial.print(localtimeval.tv_sec);
  Serial.print("; micros: ;");
  Serial.print(localtimeval.tv_usec);
}
void RTC::printLocalTime()
{

  struct tm timeinfo;
  Serial.print(" LocalTime: ; ");
  if (!getLocalTime(&timeinfo))
  {
    //Serial.println("Failed to obtain time");
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.println();
}

void RTC::printLocalTimeMicros()
{
  struct timeval localtimeval;
  struct tm timeinfo;
  gettimeofday(&localtimeval, NULL);  
  getLocalTime(&timeinfo);  
  Serial.printf(" Local Time is min:%d sec: %d micros: %d\n",timeinfo.tm_min,timeinfo.tm_sec,localtimeval.tv_usec);
}

long RTC::diff_timevals(struct timeval old, struct timeval newt)
{
  long deltamicros;
  if ((newt.tv_sec - old.tv_sec) >= 0)
  {
    deltamicros = newt.tv_usec - old.tv_usec;
  }
  else
  {
    deltamicros = newt.tv_usec - old.tv_usec - 1000000;
  }
  return deltamicros;
}
void RTC::test_rtc()
{

  Read_Until_Sec();
  long deltamicros;
  struct timeval nowis;
  gettimeofday(&nowis, NULL);
  deltamicros = diff_timevals(RTCtimeval, nowis);
  // Serial.print("; deltamicros (diff entre local y RTC): ;");
  // Serial.println(deltamicros);
}

void RTC::ConfigLocalTimeFromNTP(bool start_sig)
{
  static bool NTP_needed = 0;
  if (start_sig == 1)
  {
    NTP_needed = 1;
    Serial.println("\nStart of NTP checks..NTP.begin..\n");
    NTP.begin(ntpServer);
    NTP.onNTPSyncEvent([](NTPEvent_t event)
                       {
                         extern RTC local_RTC;
                         local_RTC.ntpEvent = event;
                         local_RTC.syncEventTriggered = true; });
  }

  if ((NTP_needed) && (keeper_Synchro_status < RTC_ON_SYNC))
  {
    struct timeval RTClocaltimeval;
    gettimeofday(&RTClocaltimeval, NULL);
    uint32_t deltamicros = micros();

    if (((NTP.syncStatus() == 0) && (event_class == 0 || event_class == 3)))
    // if (((NTP.syncStatus() == 0) && (event_class == 3)))
    {
      Serial.printf("EVENTCLAS : %d\n", event_class);
    }
    else
    {
      keeper_Synchro_status = FAILED_NTP;
      return;
    }
    keeper_Synchro_status = LOCAL_TIME_ON_SYNC;
    String error;
    if (RTC_drfit_file_flag)
    {
      Serial.println("patatin+++++++++++++++++++++");
      error = "patatin";
      keeper_Synchro_status = RTC_SYNC_DISABLED;
    }
    else
    {
      if (!RTC_corrupted && !power_lost && !hibernated)
      {
        eval_drift_from_last_sync(); // and record in GKSync
      }
      else
      {
        Serial.printf("DRIFT NOT EVALUATED due hibernation %d Power loss %d or corruption %d \n", hibernated, power_lost, RTC_corrupted); // TODO
      }
      Serial.println("Setting RTCs....");
      error = SetRTC();

      // if (!SPIFFS.exists("/calFile.csv"))
      // {
      //   Serial.printf("oldrift recorded %d alarm2_date %d \n", old_drift, alarm2_date);
      //    //TODO
      //   //Serial.print("RTC NOT RECORDED by Fw"); //TODO
      //   read_last_sync();
      //   Serial.printf("oldrift recorded %d alarm2_date %d \n", old_drift, alarm2_date);
      // }
      // else
      // {
      //   keeper_Synchro_status = RTC_SYNC_DISABLED;
      // }

      test_rtc();
    }
    if (error == "RTCset FAILED")
    {
      keeper_Synchro_status = RTC_SET_FAILED;
    }
    else
    {
      if (!RTC_drfit_file_flag)
      {
        if (keeper_Synchro_status < RTC_ON_SYNC)
        {
          keeper_Synchro_status = RTC_ON_SYNC;
        }
      }
    }
    struct timeval NTPlocaltimeval;
    gettimeofday(&NTPlocaltimeval, NULL);
    deltamicros = diff_timevals(RTClocaltimeval, NTPlocaltimeval);

    // Serial.print(" difflocaltimeval: ;");
    // Serial.print(NTPlocaltimeval.tv_sec - RTClocaltimeval.tv_sec);
    // Serial.print("; micros: ");
    // Serial.print(NTPlocaltimeval.tv_usec - RTClocaltimeval.tv_usec);
    // Serial.print(";deltamicros: ");
    // Serial.println(String(deltamicros));
    // test_rtc();
    // Serial.println("RTC set from NTP");
  }
}

void RTC::ConfigLocalTimeFromRTC()
{
  struct timeval localtimeval;
  localtimeval.tv_usec = 0;
  struct tm *timeinfo = Read_Until_Sec();
  localtimeval.tv_sec = mktime(timeinfo);
  settimeofday(&localtimeval, NULL);
}

uint8_t RTC::read_control_reg()
{
  uint8_t ctrl_reg = 0;
  uint8_t status_reg;
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x0E);
  // set DS3231 register pointer to 00h
  if (Wire.endTransmission() != 0)
  {
    Serial.print("mal I2C de RTC");
    delay(5);
  }
  else
  {
    Wire.requestFrom(DS3231_I2C_ADDRESS, 2);
    // set next input to start at the control register
    uint8_t ctrl_reg = Wire.read();
    uint8_t status_reg = Wire.read();
    if (status_reg > 0x7F)
    {
      hibernated = 1;
    }

    Wire.endTransmission();
    Serial.print("Control reg = ");
    Serial.print(ctrl_reg, BIN);
    Serial.print("  Staus reg = ");
    Serial.println(status_reg, BIN);
    return ctrl_reg;
  }
}

void RTC::write_status_reg()
{

  uint8_t status_reg;
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x0f);
  // set DS3231 register pointer to 00h
  if (Wire.endTransmission() != 0)
  {
    Serial.print("mal I2C de RTC");
    delay(5);
  }
  else
  {
    Wire.requestFrom(DS3231_I2C_ADDRESS, 1);
    uint8_t status_reg = Wire.read();
    Wire.endTransmission();
    delay(5);
    if (status_reg > 0x7F)
    {
      hibernated = 1;
      status_reg = status_reg & 0x7F;
      Wire.beginTransmission(DS3231_I2C_ADDRESS);
      Wire.write(0x0F);
      delay(5);
      Wire.write(status_reg);
      Wire.endTransmission();
    }
    else
    {
      Serial.print("NOT NEEDED DESHIBERNATION ");
    }
    Serial.print("DESHIBERNATED  Status reg = ");
    Serial.println(status_reg, BIN);
  }
}

void RTC::sleep()
{
  byte ctrl_reg = read_control_reg();
  ctrl_reg = ctrl_reg | 0x80;
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x0E);
  delay(5);
  Wire.write(0x9C);
  byte kk = Wire.endTransmission();
  if (kk != 0)
  {
    Serial.print("error I2C from RTC: ");
    Serial.println(kk);
    delay(5);
  }
  else
  {
    // Serial.println("=============== = ");
    ctrl_reg = read_control_reg();
  }
}

void RTC::wake_up()
{
  byte ctrl_reg = read_control_reg();
  ctrl_reg = ctrl_reg & 0x7f;
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x0E);
  delay(5);
  Wire.write(ctrl_reg);
  byte kk = Wire.endTransmission();
  if (kk != 0)
  {
    Serial.print("mal I2C de RTC: ");
    Serial.println(kk);
    delay(5);
  }
  else
  {
    // Serial.println("=============== = ");
    ctrl_reg = read_control_reg();
  }
}

void RTC::setAgingOffset(int8_t offset)
{

  if (offset < 0)
    offset += 256;
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x10);
  Wire.write(offset);
  Wire.endTransmission();
}

float RTC::DS3231_get_treg()
{
  //  int rv;  // Reads the temperature as an int, to save memory
  float rv;

  uint8_t temp_msb, temp_lsb;
  int8_t nint;

  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(DS3231_TEMPERATURE_ADDR);
  Wire.endTransmission();

  Wire.requestFrom(DS3231_I2C_ADDRESS, 2);
  temp_msb = Wire.read();
  temp_lsb = Wire.read() >> 6;

  if ((temp_msb & 0x80) != 0)
    nint = temp_msb | ~((1 << 8) - 1); // if negative get two's complement
  else
    nint = temp_msb;

  rv = 0.25 * temp_lsb + nint;

  return rv;
}

void RTC::record_RTC_drift(bool RTC_drift_record_flag) // Corre en loop() cad 3 msec
{
  // static uint32_t last_record_millis = 0;
  // static bool firstime = 1;
  // static int xxcounts = 1;
  // uint xRTC_sec;
  // static uint xxrefRTC_sec;
  // static uint xSUM_sec = 0;
  // static float xSUM_drift = 0;
  // uint xlocal_sec;
  // static uint xreflocal;
  // uint xlocal_usec;
  // String line;
  // float tempRTC;
  // if (RTC_drift_record_flag)
  // {
  //   if (firstime)
  //   {
  //     firstime = 0;
  //     // NTP.stop();
  //     if (!SPIFFS.exists("/logFile.csv"))
  //     {

  //       File logFile = SPIFFS.open("/logFile.csv", "w+");
  //       line = "millis;RTC_sec;local-sec;local_usec;TempRTC;status;";
  //       line = line + "Event_code;from IP; Offset;Delay;retrials;Dispersion;\n";
  //       Serial.print(line);
  //       logFile.print(line);
  //       logFile.close();
  //     }
  //     else
  //     {
  //       load_first_set();
  //       is_final = 1;
  //     }
  //   }
  //   if (xxcounts < NUMBER_OF_RECORDS)
  //   {
  //     if ((millis() - last_record_millis) > RECORD_MILLIS_PERIOD)
  //     {

  //       tempRTC = DS3231_get_treg();
  //       File logFile = SPIFFS.open("/logFile.csv", "a");
  //       struct timeval RTCtimeval;   //timeval del RTC
  //       struct timeval localtimeval; // timeval del sistema
  //       ReadRTC();
  //       struct tm *timeinfo = Read_Until_Sec(); //bucle hasta segundo en punto de RTC y devuelve *timeinfo
  //       gettimeofday(&localtimeval, NULL);
  //       RTCtimeval.tv_sec = mktime(timeinfo);
  //       if (RTCtimeval.tv_sec > 0)
  //       {
  //         // if (NTP.syncStatus() == 0)
  //         if (ntpEvent.event >= 0)
  //         {
  //           RTCtimeval.tv_usec = 0;
  //           line = String(millis()) + ";" +
  //                  String(RTCtimeval.tv_sec) + ";" +
  //                  String(localtimeval.tv_sec) + ";" +
  //                  String(localtimeval.tv_usec) + ";" + String(tempRTC, 2) + ";" + String(NTP.syncStatus()) + ";"; //no  olvidar el CR
  //           line = line + my_event_string;
  //           Serial.print(line);
  //           logFile.print(line);
  //           last_record_millis = millis();
  //           xRTC_sec = RTCtimeval.tv_sec;
  //           xlocal_sec = localtimeval.tv_sec;
  //           xlocal_usec = localtimeval.tv_usec;

  //           if (xxcounts == 1)
  //           {
  //             xxrefRTC_sec = xRTC_sec;
  //             Serial.print(xxrefRTC_sec);
  //             Serial.println("==============Direct from RTC===================");

  //             // Serial.println("===7777===========");
  //             xreflocal = xlocal_sec;
  //           }
  //           int DD = xRTC_sec - xxrefRTC_sec;

  //           xSUM_sec += DD;
  //           // Serial.print("xRTC_sec=");
  //           // Serial.println(xRTC_sec);
  //           // Serial.print("xrefRTC_sec=");
  //           // Serial.println(xxrefRTC_sec);
  //           int delta;
  //           if (xlocal_sec > xRTC_sec)
  //           {
  //             delta = (int)(xlocal_sec - xRTC_sec);
  //           }
  //           else
  //           {
  //             delta = -(int)(xRTC_sec - xlocal_sec);
  //           }

  //           xSUM_drift += (float)(xlocal_usec) + 1000000 * (float)delta;

  //           Serial.print("xx=");
  //           Serial.println(xxcounts);
  //           // Serial.print("xSUM_sec=");
  //           // Serial.println(xSUM_sec);
  //           // Serial.print("Delta=");
  //           // Serial.println(delta);
  //           // Serial.print("xSUM_drift=");
  //           // Serial.println(xSUM_drift);

  //           // Serial.print("Avg_sec=");
  //           // Serial.println(xSUM_sec / xxcounts);
  //           Serial.print("Avg_drift=");
  //           Serial.println(xSUM_drift / xxcounts, 2);
  //           Avg_sec_final = xSUM_sec / xxcounts + xxrefRTC_sec;
  //           Avg_drift_final = xSUM_drift / xxcounts;
  //           xxcounts++;
  //         }
  //       }
  //       else
  //       {
  //         Serial.println("Failed RTC read  !!!!!!!!");
  //       }
  //       logFile.close();
  //     }
  //   }
  //   else
  //   {
  //     if (is_final)
  //     {
  //       Serial.print("drift initial: ");
  //       Serial.println(Avg_drift_init);
  //       Serial.print("drift final: ");
  //       Serial.println(Avg_drift_final);
  //       Serial.print("sec initial: ");
  //       Serial.println(Avg_sec_init);
  //       Serial.print("sec final: ");
  //       Serial.println(Avg_sec_final);
  //       Serial.print("diff secs: ");
  //       Serial.println(Avg_sec_final - Avg_sec_init);

  //       float ppmdrift = (float)(Avg_drift_final - Avg_drift_init) / (float)(Avg_sec_final - Avg_sec_init);
  //       Serial.println("CALCULATED DRIFT !!");
  //       Serial.print(" ppm= ");
  //       Serial.println(ppmdrift);
  //       //eval_drift_from_last_sync();
  //       delay(500000);
  //     }
  //   }
  // }
}

void RTC::processSyncEvent(NTPEvent_t ntpEvent)
{
  switch (ntpEvent.event)
  {
  case timeSyncd:
    my_event_string = String(ntpEvent2str(ntpEvent));
    break;
  case requestSent:
    my_event_string = "request sent";
    break;
  case partlySync:
    my_event_string = String(ntpEvent2str(ntpEvent));
    break;
  case syncNotNeeded:
    my_event_string = String(ntpEvent2str(ntpEvent));
    break;
  case accuracyError:
    my_event_string = String(ntpEvent2str(ntpEvent));
    break;
  case noResponse:
    my_event_string = "invalid addrees";
    break;
  case responseError:
    my_event_string = "Wrong response received";
    break;
  case syncError:
    my_event_string = "error adjunting time";
    break;
  default:
    my_event_string = "unknown event";
    break;
  }
}

char *RTC::ntpEvent2str(NTPEvent_t e)
{
  const int resultMaxSize = 150;
  static char result[resultMaxSize];
  snprintf(result, resultMaxSize, "%d;%s;%0.3f;%0.3f;%d;%0.3f;\n",
           e.event,
           e.info.serverAddress.toString().c_str(),
           e.info.offset * 1000,
           e.info.delay * 1000,
           e.info.retrials,
           e.info.dispersion * 1000);
  event_class = e.event;
  return (result);
}

void RTC::load_first_set() //// TODO quitado por espacio
{
  // File file = SPIFFS.open("/logFile.csv", "r");
  // int xx = 1;
  // uint xmillis;
  // uint xRTC_sec;
  // uint xrefRTC_sec;
  // uint xSUM_sec = 0;
  // float xSUM_drift = 0;
  // uint xlocal_sec;
  // uint xreflocal;
  // uint xlocal_usec;
  // float xTempRTC;
  // int xstatus;
  // int xevent_code;
  // String xIP;
  // float xoffset;
  // float xdelay;
  // int xretrials;
  // float xdispersion;
  // bool one_deleted = 0;
  // String line;
  // line = file.readStringUntil('\n');
  // line.replace("\n", "");
  // line.replace("\r", "");
  // if (line.indexOf("RTC_sec;") > 0)
  // {
  //   Serial.print("==============From File data");
  //   while (file.available())
  //   {
  //     //Lets read line by line from the file
  //     line = file.readStringUntil('\n');
  //     line.replace("\n", "");
  //     line.replace("\r", "");

  //     int I = line.indexOf(";");

  //     // Serial.println(I);
  //     //  Serial.print("linevalue ");
  //     // Serial.println(line);
  //     // pilots_table[xx].pilot_id = line.substring(0, I).toInt();
  //     xmillis = line.substring(0, I).toInt();
  //     line = line.substring(I + 1);
  //     I = line.indexOf(";");
  //     xRTC_sec = line.substring(0, I).toInt();
  //     line = line.substring(I + 1);
  //     I = line.indexOf(";");
  //     xlocal_sec = line.substring(0, I).toInt();
  //     line = line.substring(I + 1);
  //     I = line.indexOf(";");
  //     xlocal_usec = line.substring(0, I).toInt();
  //     line = line.substring(I + 1);
  //     I = line.indexOf(";");
  //     xTempRTC = line.substring(0, I).toFloat();
  //     line = line.substring(I + 1);
  //     I = line.indexOf(";");
  //     xstatus = line.substring(0, I).toInt();
  //     line = line.substring(I + 1);
  //     I = line.indexOf(";");
  //     xevent_code = line.substring(0, I).toInt();
  //     line = line.substring(I + 1);
  //     I = line.indexOf(";");
  //     xIP = line.substring(0, I);
  //     line = line.substring(I + 1);
  //     I = line.indexOf(";");
  //     xoffset = line.substring(0, I).toFloat();
  //     line = line.substring(I + 1);
  //     I = line.indexOf(";");
  //     xdelay = line.substring(0, I).toFloat();
  //     line = line.substring(I + 1);
  //     I = line.indexOf(";");
  //     xretrials = line.substring(0, I).toInt();
  //     line = line.substring(I + 1);
  //     I = line.indexOf(";");
  //     xdispersion = line.substring(0, I).toFloat();
  //     if (xx == 1)
  //     {
  //       xrefRTC_sec = xRTC_sec;
  //       xreflocal = xlocal_sec;
  //     }
  //     xSUM_sec += xRTC_sec - xrefRTC_sec;
  //     int delta;
  //     if (xlocal_sec > xRTC_sec)
  //     {
  //       delta = (int)(xlocal_sec - xRTC_sec);
  //     }
  //     else
  //     {
  //       delta = -(int)(xRTC_sec - xlocal_sec);
  //     }

  //     xSUM_drift += (float)(xlocal_usec) + 1000000 * (float)delta;

  //     // Serial.print("xx=");
  //     // Serial.println(xx);
  //     // Serial.print("xSUM_sec=");
  //     // Serial.println(xSUM_sec);
  //     // Serial.print("xSUM_drift=");
  //     // Serial.println(xSUM_drift);

  //     // Serial.print("Avg_sec=");
  //     // Serial.println(xSUM_sec / xx);
  //     // Serial.print("Avg_drift=");
  //     // Serial.println(xSUM_drift / xx, 2);
  //     Avg_sec_init = xSUM_sec / xx + xrefRTC_sec;
  //     Avg_drift_init = xSUM_drift / xx;

  //     xx++;
  //     if (xx > (NUMBER_OF_RECORDS - 1)) //// TODO
  //     {
  //       file.close();
  //       Serial.print("Avg_sec=");
  //       Serial.println(Avg_sec_init);
  //       Serial.print("Avg_drift=");
  //       Serial.println(Avg_drift_init, 3);
  //       return;
  //     }
  //   }
  // }
  // else
  // {
  //   Serial.println("Problems with the format of file");
  //   file.close();
  //   return;
  // }
  // file.close();
}

float RTC::eval_drift_from_last_sync()
{
  struct timeval local_val_now;

  struct tm timeinfo_now;
  int drift_usec; // miliseconds of drift
  int elapsed_sec;
  float drift;
  struct tm *RTC_time_now = Read_Until_Sec();
  gettimeofday(&local_val_now, NULL);
  localtime_r(&local_val_now.tv_sec, &timeinfo_now);
  RTCtimeval.tv_sec = mktime(RTC_time_now); // devuelve el time_t correspondiente a al tm_RTCtime
  RTCtimeval.tv_usec = 0;

  /// the drift is the difference
  if (RTCtimeval.tv_usec > local_val_now.tv_usec)
  {
    drift_usec = (RTCtimeval.tv_sec - local_val_now.tv_sec) * 1000000;
    // Serial.printf(">>>drift_usec = %d \n", drift_usec);
    drift_usec += (RTCtimeval.tv_usec - local_val_now.tv_usec);
    // Serial.printf("drift_usec = %d \n", drift_usec);
  }
  else
  {
    drift_usec = (RTCtimeval.tv_sec - local_val_now.tv_sec - 1) * 1000000;
    // Serial.printf("<<<drift_usec = %d \n", drift_usec);
    drift_usec += 1000000 + (RTCtimeval.tv_usec - local_val_now.tv_usec);
    // Serial.printf("drift_usec = %d \n", drift_usec);
  }

  Serial.printf("drift_usec = %d \n", drift_usec);
  // last_sync_time.tm_mon = RTC_time_now->tm_mon;

  last_sync_time.tm_year = RTC_time_now->tm_year;
  // Serial.printf(" mes last sync : %d\n", last_sync_time.tm_mon);
  // Serial.printf(" A/*o last sync : %d\n", last_sync_time.tm_year);

  last_sync_val.tv_sec = mktime(&last_sync_time);
  elapsed_sec = RTCtimeval.tv_sec - last_sync_val.tv_sec;
  
  while (elapsed_sec < 0) // corrections if a year roller over
  {

    last_sync_time.tm_year = RTC_time_now->tm_year - 1;
    last_sync_val.tv_sec = mktime(&last_sync_time);
    elapsed_sec = RTCtimeval.tv_sec - last_sync_val.tv_sec;
    Serial.print("ELAPSED SECS WAS NEGATIVE, changing year; Now Is: ");
    // Serial.println(&timeinfo_now, "%A, %B %d %Y %H:%M:%S");
    // Serial.print(" Last sync was : ");
    // Serial.println(&last_sync_time, "%A, %B %d %Y %H:%M:%S");
    // Serial.printf(" ELAPSED SEC : %d \n", elapsed_sec);
    if ((RTC_time_now->tm_year - last_sync_time.tm_year) > 2)
    {
      Serial.printf("Drift from RTC calcs= is corrupted  \n");
      return 100;
    }
  }
  Serial.print(" Now Is: ");
  Serial.println(&timeinfo_now, "%A, %B %d %Y %H:%M:%S");
  Serial.print(" Last sync was : ");
  Serial.println(&last_sync_time, "%A, %B %d %Y %H:%M:%S");
  Serial.printf(" ELAPSED SEC : %d \n", elapsed_sec);
  float _old_drift = (float)drift_usec / (float)elapsed_sec;
  if (drift_usec > 0)
  {
    drift_usec = drift_usec - DOUBLE_SYNC_ERROR * 1000;
    if (drift_usec < 0)
    {
      drift_usec = 0;
    }
  }
  else
  {
    drift_usec = drift_usec + DOUBLE_SYNC_ERROR * 1000; /// arreglado 21 jul 22 (+) antes era (-)
    if (drift_usec > 0)
    {
      drift_usec = 0;
    }
  }

  drift = (float)(drift_usec) / (float)(elapsed_sec);
  int paging = (int)(drift * 10);
  int8_t old_aging = read_aging();
  paging = paging + old_aging;
  if (paging > MAX_AGING)
  {
    paging = MAX_AGING;
  }
  if (paging < -MAX_AGING)
  {
    paging = -MAX_AGING;
  }
  int8_t aging = (int8_t)paging;
  const int resultMaxSize = 250;
  char line[resultMaxSize];
  char buff[100];
  my_event_string.toCharArray(buff, 100); // Note buff will have \n already
  snprintf(line, resultMaxSize, "%d; %d;   %8.4f;   %d;       %d;  %s",
           local_val_now.tv_sec,
           elapsed_sec,
           drift,
           old_aging,
           aging,
           buff);

  // bool decided_to_rewrite = 0;
  // decided_to_rewrite = (elapsed_sec > ELAPSED_FOR_AGING);

  // if (SPIFFS.exists("/calFile.csv"))
  // {
  //   File calFile = SPIFFS.open("/calFile.csv", "a");
  //   calFile.print(line);
  //   calFile.close();
  //   Serial.printf("Successfull aging file recording %d \n", aging);
  // }

  if (elapsed_sec > ELAPSED_FOR_AGING)
  {
    old_drift = (int8_t)_old_drift;
    if (write_aging(aging)) // TODO
    {
      Serial.println("failed aging write in GKSync");
    }
    else
    {
      keeper_Synchro_status = RTC_ON_SYNC_AND_CALIBRATED;
      Serial.print("AGING RECORDED by Fw\n");
    }
    // Serial.print("AGING NOT RECORDED by Fw\n"); //TODO
  }
  else
  {
    Serial.println("WARNING: No Drift write was done, not enough elapsed time");
  }
  Serial.printf("now.tv_sec; elapsec;  drft; old_; aging; event; Ip\n");
  Serial.print(line);
  return drift;
}

bool RTC::write_aging(int8_t aging)
{

  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x10); // set next input to start at the seconds register
  Wire.write(aging);
  if (Wire.endTransmission() != 0)
  {
    Serial.print("mal I2C de RTC en aging set");
    return 1;
  }
  return 0;
}

int8_t RTC::read_aging()
{
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x10); // set next input to start at the seconds register
  if (Wire.endTransmission() != 0)
  {
    Serial.print("mal I2C de RTC en aging read");
    return -127;
  }
  else
  {
    Wire.requestFrom(DS3231_I2C_ADDRESS, 1);
    return Wire.read();
  }
}
