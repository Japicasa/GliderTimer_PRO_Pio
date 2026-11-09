

#include "MP3player.h"

/*
 * There are three serial ports on the ESP known as U0UXD, U1UXD and U2UXD.
 *
 * U0UXD is used to communicate with the ESP32 for programming and during reset/boot.
 * U1UXD is unused and can be used for your projects. Some boards use this port for SPI Flash access though
 * U2UXD is unused and can be used for your projects.
 *
 */

void MP3player::setup()
{
  // Note the format for setting a serial port is as follows: Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  // Serial.begin(115200);
  // Serial1.begin(9600, SERIAL_8N1, RXD2, TXD2);
  //////////////////////////////////////////////Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  // Serial.println("Serial Txd is on pin: " + String(TXD2));
  // Serial.println("Serial Rxd is on pin: " + String(RXD2));
  // Serial.printf("WW-> fin Serial2 begin : %d \n", millis());
  // if (!myDFPlayer.begin(FPSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
  printDetail(myDFPlayer.readType(), myDFPlayer.read()); // Print the detail message from DFPlayer to handle different errors and states.
  boolean initOK = (myDFPlayer.begin(Serial2, 1, 1));
  if (!initOK)
  {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    delay(100);
    initOK = (myDFPlayer.begin(Serial2, 1, 1));
    if (!initOK)
    {
      Serial.println(F("Unable to begin:"));
      Serial.println(F("1.Please recheck the connection!"));
      Serial.println(F("2.Please insert the SD card!"));
    }
    else
    {
      Serial.println(F("xxxxxxxxxxx==========  MP3 PLAYER  INIT 2nd attempt"));
    }
  }
  else
  {
    Serial.println(F("xxxxxxxxxxx==========  MP3 PLAYER  INIT"));
  }

  // Serial.printf("WW-> myDFPlayer.started : %d \n", millis());
  if ((music_vol > 30) || (music_vol < 1))
  {
    music_vol = MUSIC_VOL;
  }
  if ((adv_vol > 30) || (adv_vol < 1))
  {
    adv_vol = ADV_VOL;
  }
  actual_vol = music_vol;
  file_to_play = 0;
  delay(300);
  if (myDFPlayer.available())
  {

    printDetail(myDFPlayer.readType(), myDFPlayer.read()); // Print the detail message from DFPlayer to handle different errors and states.
  }
  else
  {
    Serial.println(F("WWWWWWWWWWW========== NO MP3 PLAYER"));
    printDetail(myDFPlayer.readType(), myDFPlayer.read()); // Print the detail message from DFPlayer to handle different errors and states.
  }
  // file_to_play = random(0, FILES_IN_FOLDER);
}

void MP3player::printDetail(uint8_t type, int value)
{
  Serial.println(F("WWWWWWWWWWW========== Printing detail"));
  switch (type)
  {
  case TimeOut:
    Serial.println(F("Time Out!"));
    break;
  case WrongStack:
    Serial.println(F("Stack Wrong!"));
    break;
  case DFPlayerCardInserted:
    Serial.println(F("Card Inserted!"));
    break;
  case DFPlayerCardRemoved:
    Serial.println(F("Card Removed!"));
    break;
  case DFPlayerCardOnline:
    Serial.println(F("Card Online!"));
    break;
  // case DFPlayerUSBInserted:
  //   Serial.println("USB Inserted!");
  //   break;
  // case DFPlayerUSBRemoved:
  //   Serial.println("USB Removed!");
  //   break;
  case DFPlayerPlayFinished:
    Serial.print(F("Number:"));
    Serial.print(value);
    Serial.println(F(" Play Finished!"));
    break;
  case DFPlayerError:
    Serial.print(F("DFPlayerError:"));
    switch (value)
    {
    case Busy:
      Serial.println(F("Card not found"));
      break;
    case Sleeping:
      Serial.println(F("Sleeping"));
      break;
    case SerialWrongStack:
      Serial.println(F("Get Wrong Stack"));
      break;
    case CheckSumNotMatch:
      Serial.println(F("Check Sum Not Match"));
      break;
    case FileIndexOut:
      Serial.println(F("File Index Out of Bound"));
      break;
    case FileMismatch:
      Serial.println(F("Cannot Find File"));
      break;
    case Advertise:
      Serial.println(F("In Advertise"));
      break;
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void MP3player::play_something(int filetoplay_request)
{ // Choose Serial1 or Serial2 as required
  // Serial2.println("HELLO WORlD FROM SERIAL2");
  // delay(100);
  // while (Serial2.available())
  // {
  //   Serial.print(char(Serial2.read()));
  // }
  // myDFPlayer.volume(music_vol);
  uint8_t folders_to_play = 36; /// Poner aqui los folders de musica
  if (filetoplay_request > 0)
  {
    file_to_play = filetoplay_request;
    while (file_to_play > folders_to_play)
    {
      file_to_play = file_to_play - folders_to_play;
    }
  }
  else
  {
    file_to_play++;
    if (file_to_play > folders_to_play)
    {
      file_to_play = 1;
    }
  }

  delay(100);
  if (firstime)
  {
    myDFPlayer.loopFolder(file_to_play);
    firstime = 0;

    // Serial.print("loopfolder1");
    Serial.printf("loopfolder: %d \n", file_to_play);
  }
  else
  {
    boolean initOK = (myDFPlayer.begin(Serial2));
    Serial.printf("loopfolder: INITED\n");
    delay(200);
    //  volume(0);
    //  delay(100);

    // myDFPlayer.playLargeFolder(1, file_to_play);
    Serial.printf("loopfolder: %d \n", file_to_play);
    myDFPlayer.loopFolder(file_to_play);
  }
}
void MP3player::play_nothing()
{ // Choose Serial1 or Serial2 as required
  //  Serial2.println("HELLO WORlD FROM SERIAL2");
  //  delay(100);
  //  while (Serial2.available())
  //  {
  //    Serial.print(char(Serial2.read()));
  //  }
  //  myDFPlayer.volume(music_vol);

  myDFPlayer.loopFolder(37);
  Serial.println("loop Folder 37");
}

void MP3player::ad_file(int filetoplay)
{
  // filetoplay=10;
  // Serial.printf("adv_vol : %d \n", adv_vol);
  // myDFPlayer.volume(adv_vol);
  Serial.printf("ADVERTISING-----actual_vol : %d filetoplay %d\n", actual_vol, filetoplay);
  // Serial.printf("NOW advertise: %d \n", filetoplay);
  if (filetoplay > 2400)
  {
    for (int i = 0; i < 2; i++)
    {
      Serial.printf("NOW advertise: %d \n", filetoplay);
      delay(100);
    }
  }

  myDFPlayer.advertise(filetoplay);
}

void MP3player::stop()
{
  // filetoplay=10;
  myDFPlayer.stop();
}
void MP3player::play(int filetoplay)
{
  // filetoplay=10;
  myDFPlayer.start();
  myDFPlayer.playFolder(2, filetoplay);
}

void MP3player::volume_decr()
{
  actual_vol = actual_vol - 1 - (music_vol) / 6;
  if (actual_vol < 0)
    actual_vol = 0;
  myDFPlayer.volume(actual_vol);
  Serial.printf("volincr : %d\n", actual_vol);
}
void MP3player::volume(int vol_dem)
{
  actual_vol = vol_dem;
  myDFPlayer.volume(actual_vol);
  Serial.printf("vol set to------------> : %d\n", actual_vol);
  delay(100);
}
void MP3player::volume_incr()
{
  actual_vol = actual_vol + 1 + (music_vol) / 4;
  if (actual_vol > music_vol)
    actual_vol = music_vol;
  myDFPlayer.volume(actual_vol);
}
