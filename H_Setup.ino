

//#include "QueueRegister.h"
//#include "StructSavedRW.h"
#include "src/CompetitionSavedRW.h"
#include "DebugPrint.h"

void setup()
{
  M5.begin(true, false, true, true);

  // M5Stack::begin(bool LCDEnable, bool SDEnable, bool SerialEnable, bool I2CEnable)

  Serial.printf("WW-> empieza setup : %d \n", millis());
  struct Contests *fltdata = NULL;
  int maxpriority = configMAX_PRIORITIES;
  TaskHandle_t Task0, Task1;

  // Serial.begin(115200); // comentar esto si queremos que no salga nada por el usb

  //.... D_funciones_Display Linea 10 y 12
  Wire.begin(GPIO_NUM_21, GPIO_NUM_22); // M%STACK uses std wire /I2C pins
  // Wire.begin(GPIO_NUM_16, GPIO_NUM_17); // SDA, SCL Usado por el barometro y el OLED  del KEEPER
 Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  wifiDisconnect();
  /*
    Power chip connected to gpio21, gpio22, I2C device
    Set battery charging voltage and current
    If used battery, please call this function in your project
  */
  M5.Power.begin();
  M5.Power.setPowerBoostOnOff(1);

  bat_level = M5.Power.getBatteryLevel();
  Serial.printf("Battery Level = %d Can control %d  \n", bat_level, M5.Power.canControl());

  // M5.Lcd.setBrightness(250);
  M5.Speaker.setVolume(0);
  M5.Speaker.update();
  //Serial.printf("WW-> Fin de M5 : %d \n", millis());
  M5.Lcd.setFreeFont(FMB9);
  M5.lcd.setTextColor(WHITE);
  M5.Lcd.setCursor(0, 20);
  M5.Lcd.println("Glider Timer " + versionkeeper);
  M5.Lcd.println();

  if (!SPIFFS.begin())
  {
    // report_error(err_code_spiffs);
  } // Start the SPI Flash Files System
  // Serial.println();

  Wire.begin(GPIO_NUM_21, GPIO_NUM_22);
  readGliderKeeperConfig();
  flight.init(); // se leen los settings
  GLOBAL_DURATION=flight.look_for_duration_from_file();
  Serial.printf("duration from file %d \n", GLOBAL_DURATION);

  // FLTnum = competition.init(); // El último vuelo está aquí

  GT.stored_competition_no = competition.init(); //// competition number that was opened
  Serial.print("competition inited ");
  Serial.println(GT.stored_competition_no);
  M5.Lcd.println("Loading index table");

  if (GT.stored_competition_no > 0)
  {
    fltdata = competition.getFlightData(GT.stored_competition_no);

    Serial.println("---------------------------");
    Serial.printf("competion was: %d Data Version was: %d FwData version is %d \n", fltdata->head.contestNum, fltdata->head.data_version, DATA_VERSION);
    Serial.println("---------------------------");
    if (fltdata->head.data_version != DATA_VERSION)
    {
      M5.Lcd.println("MISMATCH DATA");
      M5.Lcd.println("  Formatting...");
      M5.Lcd.println("Unit will reset");
      bool exito = competition.format();
      Serial.printf("<<<<<<<<<<<<<<<<<<<format completed exito %d \n", exito);
      ESP.restart();
    }
    else
    {
      M5.Lcd.println("Dataformat:OK");
      Serial.println("Stored COMPETITION  :");
      Serial.print("Conmp:no ");
      Serial.println(GT.stored_competition_no);
      Serial.print("Rounds: ");
      Serial.println(fltdata->head.number_of_rounds);
      Serial.print("Groups: ");
      Serial.println(fltdata->head.number_of_groups);
      Serial.print("number_of_pilots: ");
      Serial.println(fltdata->head.number_of_pilots);
    }
  }
  else
  {
    M5.Lcd.println("Dataformat:BLANK");
  }
  M5.Lcd.println();  

  set_server_functions();
  M5.Lcd.println("loading fonts");

  GD.begin(); 
  M5.Lcd.println("loading competition");
  M5.Lcd.println();

  GT.begin();
  //Serial.printf("WW-> fin de GT.begin : %d \n", millis());
  M5.Lcd.println("looking for MP3 player");
  M5.Lcd.println();

  MP3.setup();

  //Serial.printf("WW-> fin de MP3.setup : %d \n", millis());

  GT.update_setting_labels();
  FLTdata.begin();

  //Serial.printf("WW-> fin de setting labels : %d \n", millis());

 

  //   //M5.Lcd.drawBmpFile(SPIFFS, "/LogoKeeper2.bmp", 0, 0);
  //   M5.Lcd.setTextColor(BLUE, WHITE);
  //   M5.Lcd.setCursor(55, 197);
  //   //M5.Lcd.setTextSize(2);

  //   M5.Lcd.setFreeFont(FSS9);
  //   //GDsetTextSize(2);

  //   M5.Lcd.setTextSize(1);
  //   M5.Lcd.print("Altimeter");

  //   M5.Lcd.print(" & ");
  //   //M5.Lcd.setTextSize(2);
  //   M5.Lcd.print("Timekeeper");
  //   M5.Lcd.setCursor(21, 236);
  //   M5.Lcd.setFreeFont(FMB9);
  //   //M5.Lcd.setTextSize(2);
  //  // M5.Lcd.print("     Menu         Scroll          Select");
  //   M5.Lcd.print(button_label);

  pinMode(gpio_in_pin, INPUT_PULLUP);
  pinMode(gpio_pin, OUTPUT);
  M5.Lcd.println("Updating Clock");
  M5.Lcd.println();
  //Serial.printf("WW-> principio de reloj : %d \n", millis());

  local_RTC.ReadRTC();
  if (local_RTC.keeper_Synchro_status == RTC_AVAILABLE)
  {
    local_RTC.ConfigLocalTimeFromRTC();
    local_RTC.read_last_sync();
    Serial.println("RTC_AVAILABLE ++++++++++++++++++++++");
  }
  
 // Serial.printf("WW-> fin reloj : %d \n", millis());

  MP3.volume(0);
  // MP3.play_nothing();
  if (flight.panel_type == I2C_IF)
  {
    initMCPs();
  }


  M5.Lcd.println("Setting peripherals");
  M5.Lcd.println();
  M5.Lcd.println("Setting WiFi");
  M5.Lcd.println();
  Serial.printf("WW-> fin setup : %d \n", millis());
  //Serial.printf("WW-> empieza dibujo : %d \n", millis());
  M5.Lcd.drawJpgFile(SPIFFS, "/LogoKeeper2.jpg", 0, 0);
 // Serial.printf("WW-> fin sdibujo : %d \n", millis());
  // M5.Lcd.drawBmpFile(SPIFFS, "/LogoKeeper2.bmp", 0, 0);
  M5.Lcd.setTextColor(BLUE, WHITE);
  M5.Lcd.setCursor(55, 197);
  // M5.Lcd.setTextSize(2);
  M5.Lcd.setFreeFont(FSS9);
  // GDsetTextSize(2);
  M5.Lcd.setTextSize(1);
  M5.Lcd.print("Altimeter");
  M5.Lcd.print(" & ");
  // M5.Lcd.setTextSize(2);
  M5.Lcd.print("Timekeeper");
  M5.Lcd.setCursor(21, 236);
  M5.Lcd.setFreeFont(FMB9);
  // M5.Lcd.setTextSize(2);
  // M5.Lcd.print("     Menu         Scroll          Select");
  M5.Lcd.print(button_label);


}
