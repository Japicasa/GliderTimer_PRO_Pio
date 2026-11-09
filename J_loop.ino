void blink_ontime(bool blink_ontime_flag) ////Blink ontime shloug only be used in PATATA
{
  // if (blink_ontime_flag)
  // {
  //   struct timeval xx;
  //   gettimeofday(&xx, NULL);
  //   digitalWrite(gpio_pin, (bool)(xx.tv_sec % 2));
  //   //Serial.printf(" RTC status %d \n", local_RTC.keeper_Synchro_status);
  // }
  // if (check_for_mp3)
  // {
  //   busy_mp3 = !digitalRead(gpio_in_pin);
  //   if (busy_mp3)
  //   {
  //     uint32_t mp3_lag = millis() - millis_from_cmd;
  //     //Serial.printf("LAG to receive a BUSY Signal %d \n",mp3_lag);
  //     check_for_mp3 = 0;
  //   }
  // }
}

void beginUdp()
{
  // clear_Myudp();

  // tc.toCharArray(replyPacket, 50);

  // Udp.begin(localUdpPort);
  if (udp.listen(myudp.localUdpPort))
  {
    Serial.print("UDP Listening on IP: ");
    Serial.println(WiFi.localIP());
    udp.onPacket([](AsyncUDPPacket packet)
                 {
                   //handleUdp(packet);
                   // Serial.print("UDP Packet Type: ");
                   // Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast"
                   //                                                                        : "Unicast");
                   // Serial.print(", From: ");
                   // Serial.print(packet.remoteIP());
                   // Serial.print(":");
                   // Serial.print(packet.remotePort());
                   // Serial.print(", To: ");
                   // Serial.print(packet.localIP());
                   // Serial.print(":");
                   // Serial.print(packet.localPort());
                   // Serial.print(", Length: ");
                   // Serial.print(packet.length());
                   //Serial.print(",raw  Data: ");
                   //Serial.write(packet.data(), packet.length());
                   // Serial.println();
                   //reply to the client
                   //packet.printf("Got %u bytes of data", packet.length());
                   // memcpy(incomingPacket, packet.data(), packet.length());
                   //extern char incoming_Pack [100];
                   extern volatile bool packet_received;
                   extern Myudp myudp;
                   myudp.millis_in = millis();
                   //Serial.println(millis());
                   // incoming_package = packet.readString();
                   // incoming_package += ";"+String(millis());

                   memcpy(&myudp.incomingPacket, packet.data(), packet.length());
                   myudp.incomingPacket[packet.length()] = 0; //For string termination
                   myudp.incoming_package = String(myudp.incomingPacket);
                   packet_received = 1;
                   // Serial.print("myudp.incomingPacket ");
                   // Serial.println(myudp.incomingPacket);
                   packet.flush();

                   //sprintf(&incomingPacket, const char *format-string, argument-list); });
    Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), myudp.localUdpPort); });
  }
}

void loop()
{

  static bool STA_connected = 0;
  static uint last_loop_millis = 0;
  static uint loop_counter = 0;
 

  if ((millis() - last_loop_millis) > 10000)
  {
    // Serial.println(digitalRead(gpio_in_pin));
    // Serial.print((millis()-last_loop_millis));
    // Serial.print("Average loop time, msec: ");
    float xx = (float)((millis() - last_loop_millis));
    xx = xx / loop_counter;
    // Serial.println(xx,3);
    loop_counter = 0;
    last_loop_millis = millis();
  }
  else
  {
    loop_counter++;
  }
  if (sinWifi)
  {
    // GD.displaywelcomescreen();
    //  M5.Lcd.println("Setting WiFi");
    //  M5.Lcd.println();
    STA_connected = connecttowifi();
    sinWifi = 0;
    // server.begin();

    if (STA_connected)
    {
      // Blue_led.led_blink(BLINK_NORMAL);
      Debug.println("BLINK NORMAL");
      uint8_t byte3 = WiFi.localIP()[2];
      if (byte3 == 4)
      {
        myudp.begin();
        beginUdp();
      }
      else
      {
        local_RTC.ConfigLocalTimeFromNTP(1);
      }
    }
    else
    {
      // Blue_led.led_blink(BLINK_LONG);
     // Debug.print("BLINK LONG");
      //Serial.printf(" stored : %d\n", GT.stored_record_num);
    }
  }
  else
  {
    // static unsigned long last_wifimillis = 0;
    // //Debug.println("wifi Looop :"+String((millis()-last_wifimillis)));
    // last_wifimillis = millis();
    if (STA_connected)
    {
      // ArduinoOTA.handle();
    }
    else
    {
    }
    if (udp_in_progress)
    {
      // myudp.handleUdp();
    }
    else
    {
      //////////////// TODO quitado captive 
      // if (flight.panel_type == NO_PANEL) /// only no panel units will have DNS requests capability  JHR 08 may 2022
      // {
      //   dnsServer.processNextRequest();
      // }
    }
    // server.handleClient();
    //  myudp.handleUdp();
  }

  GT.button_update();

  switch (FLT_mode)
  {
  case PATATA:
  {
    if (flight.in_flight_status)
    {
      // Serial.print(".");
    }
    else
    {
      switch (page_to_show)
      {
      case MENU_PAGE:
        GT.manage_management();
        break;
      case ROUND_PAGE:
        GT.manage_show_round_data();
        break;
      case PILOT_PAGE:
        GT.manage_show_pilot_data();
        break;
      case ERROR_PAGE:
        GT.manage_show_error_data();
        break;
      }
    }
  }
  break;
  case SCAN_AP:
  {
    scan_wifi();
    FLT_mode = PATATA;
  }
  break;
  case PICK_DATA:
  {
    switch (connect_to_AP(GT.selectionAP)) // with connection result
    {
    case MAIN_STA:
    {
      if (GT.scheduled)
      {
        GT.menupointer = 1;
        GT.menulevel = 1;
        FLT_mode = CONNECTED_AS_STA;  ///3rd file main_sta=1

      }
      else
      {
        GT.receiving_working_time = 1; ////TODO not clear this is the place...
        delay(100);
        Serial.println("PICK_DATA");
        if (FLTdata.str_splitPL(get_last_FLTdata()))
        {
          STA_connected = 1;

          FLT_mode = NEW_KEEPER_TFD;
          packet_received = 0;
          Serial.flush();
          delay(19);
          M5.Speaker.tone(NOTE_DH2, 100);
          delay(100);
          M5.Speaker.tone(NOTE_DL5, 50);
          delay(100); // to allow the bell???
          myudp.syncro_status = SUCCESS_SYCHRONISING;
        }
        else
        {
          Serial.println("Data not a flight");
          GD.alabel = "FAIL: Reset, pls";
          GD.partial_flt();
          FLT_mode = CONFIRM_TFD_UPLOAD;
          GT.firstime = 0;
        }
      }
    }
    break;
    case TIMER_STA:
    {
      GT.receiving_working_time = 1; ////TODO not clear this is the place...
      delay(100);
      Serial.println("TIMER_STA");
      if (FLTdata.str_splitPL(get_last_FLTdata()))
      {
        STA_connected = 1;

        FLT_mode = NEW_KEEPER_TFD;
        packet_received = 0;
        Serial.flush();
        delay(19);
        M5.Speaker.tone(NOTE_DH2, 100);
        delay(100);
        M5.Speaker.tone(NOTE_DL5, 50);
        delay(100); // to allow the bell???
        myudp.syncro_status = SUCCESS_SYCHRONISING;
      }
      else
      {
        Serial.println("Data not a flight");
        GD.alabel = "FAIL: Reset, pls";
        GD.partial_flt();
        FLT_mode = CONFIRM_TFD_UPLOAD;
        GT.firstime = 0;
      }
    }
    break;
    case KEEPER_STA:
    {
      delay(100);
      Serial.println("KEEPER_STA");
      if (FLTdata.str_splitPL(get_last_FLTdata()))
      {
        STA_connected = 1;
        FLT_mode = NEW_KEEPER_TFD;
        packet_received = 0;
        myudp.clear_Myudp();
        M5.Speaker.tone(NOTE_DH2, 100);
        delay(100);
        M5.Speaker.tone(NOTE_DL5, 50);
        if (!FLTdata.check_sychro_needed())
        { // will jump to a mode where no need for synchro
          myudp.syncro_status = SUCCESS_SYCHRONISING;
          delay(100); /// to allow the bell
        }
        else
        {
          myudp.syncro_status = SYCHRONISING;
        }
      }
      else
      {
        // Serial.println("Data not a flight");
        GT.manage_error(ERROR_DATA);
      }
    }
    break;
    case HOME_STA:
    {
      GT.menupointer = 1;
      GT.menulevel = 1;
      // page_to_show=MENU_PAGE;
      // FLT_mode = PATATA;
      // GD.menu_displayUpdate(menupointer);
      // page_to_show = ERROR_PAGE;

      FLT_mode = CONNECTED_AS_STA;
    }
    break;
    case THROW_STA:
    {
      FLT_mode = GLIDERTHROW;
    }
    break;
    case THROW4_STA:
    {
      FLT_mode = GLIDERTHROW;
    }
    break;
    default:
    {
      // Serial.println("timeout of connection");
      GT.manage_error(ERROR_AP);
    }
    break; /// break de selection AP
    }
    break; /// break de FLTmode
  }
  case NEW_KEEPER_TFD: /// Ha llegado algo desde /timer
  {
    udp_in_progress = 1;
    switch (myudp.syncro_status)
    {
    case SYCHRONISING:
      delay(90);
      // Serial.print("entro time shift");Serial.println(millis());
      // Serial.print("Syncronysing");
      myudp.return_timeshift(STA_connected);

      // Serial.print("salgo time shift");Serial.println(millis());
      // Serial.println(" Syncronising return");
      break;
    case SUCCESS_SYCHRONISING:
      myudp.clear_Myudp();
      Serial.println("SUCCESS");
      M5.Speaker.tone(NOTE_DL5, 50);
      delay(50);
      M5.Speaker.tone(NOTE_DH2, 100);
      delay(100);
      FLT_mode = NEW_DATA;
      udp_in_progress = 0;
      break;
    case FAILED_SYCHRONISING:
      flight.boop();
      myudp.clear_Myudp();
      udp_in_progress = 0;
      FLT_mode = UNKNOWN;
      Serial.println("FAILED SYNCHRO");
      break;

    default:
      break;
    }
  }
  break;
  case GLIDERTHROW:
    // gth.manage_throw();//TODO quitando Gthrow
    break;
  case CONNECTED_AS_STA:
    GD.STA_disp();
    delay(10);
    //Serial.printf("ConfigLocalTimeFromNTP status= %d \n",local_RTC.keeper_Synchro_status);
    local_RTC.ConfigLocalTimeFromNTP(0);
    if (local_RTC.syncEventTriggered)
        {
          local_RTC.syncEventTriggered = false;
          local_RTC.processSyncEvent(local_RTC.ntpEvent);
          Serial.print("Event: ");
          Serial.println(local_RTC.my_event_string);
        }
    if (buttonA)
    {
      GD.firsttime_settings = 1;
      GT.back_to_prev_status();
      buttonC = 0;
    }
    if (buttonB)
    {
      // GT.back_to_prev_status();
      download_scheduled_rounds(3, 1);  ///3rd file main_sta=1      
      buttonB = 0;
    }
    if ((buttonC))
    {
      
      send_loop_tfd();
      buttonC = 0;
    }
    break;
  // case RECEIVED_ROUNDS:
  //   GD.received_rounds_disp();
  //   delay(10);
    
  //   if (buttonA)
  //   {
  //     GD.firsttime_settings = 1;
  //     GT.back_to_prev_status();
  //     buttonC = 0;
  //   }
  //   if (buttonB)
  //   {
  //     send_loop_tfd();
  //     buttonB = 0;
  //   }
  //   if ((buttonC))
  //   {
  //     save_file();
  //     buttonC = 0;
  //   }
  //   break;  

  case UPDATE_IN_PROGRESS:
    GD.update_screen();
    if (GT.stored_competition_no > 0)
    {
      if ((GT.stored_competition_no > 0) && (GD.updating_flash))
      { ///////////////format if there is data
        GT.stored_competition_no = 0;
        Serial.printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>formatting \n");
        bool exito = competition.format();
        Serial.printf("<<<<<<<<<<<<<<<<<<<format completed exito %d \n", exito);
      }
      else
      {
        Serial.printf(">>>>>>>>>>>>>>>>>no format needed data was blank \n");
      }
    }
    delay(200);

    break;
  case ACTION_ABOUT_MODE:
  {
    GD.about_screen();
    if (buttonA)
    {
      GD.firsttime_settings = 1;
      GT.back_to_prev_status();
      buttonC = 0;
    }
    if (buttonB)
    {
      buttonB = 0;
    }
    if ((buttonC))
    {
      GT.back_to_prev_status();
      buttonC = 0;
    }
    delay(10);
  }
  break;
  case NEED_FORMAT:
    if (GT.firstime)
    {
      GD.clean_partial_flt();
      flight.boop();
      GD.alabel = "Must Format";
      GD.blabel = "before this change";
      GD.partial_flt();
      GT.firstime = 0;
      // GD.menu_displayUpdate(GT.menupointer);
    }
    if ((buttonA))
    {
      GT.back_to_prev_status();
      buttonA = 0;
    }
    if (buttonB)
    {
      buttonB = 0;
    }

    if ((buttonC))
    {
      GT.back_to_prev_status();
      buttonC = 0;
    }
    break;

    break;
  case RUN_FLIGHT_REQUEST:
    if (GD.firsttime_run == 1)
    {
      GD.confirm_run_flight();
      GD.firsttime_run = 0;
    }
    if (buttonA)
    {
      GD.firsttime_run = 1;
      GT.back_to_prev_status();
      buttonA = 0;
    }
    if (buttonB)
    {
      buttonB = 0;
    }
    if ((buttonC))
    {
      GD.firsttime_run = 1;
      GT.mycompetition.print_contest();
      Serial.printf("Stored status stored round %d Strored group %d stored recno %d GT.GTmenu[GT.menupointer].new_menulevel %d\n", GT.stored_round_no, GT.stored_group_no, GT.stored_record_num, GT.GTmenu[GT.menupointer].new_menulevel);
      flight.start(GT.GTmenu[GT.menupointer].new_menulevel);
      GT.back_to_prev_status();
      buttonC = 0;
    }
    delay(10);

    break;
  /////////////////////
  case CONFIRM_TFD_UPLOAD:
  {
    if (GT.firstime)
    {
      GD.clean_partial_flt();
      GD.alabel = "  Upload records? ";
      // GD.blabel = "   GK Fw: " + String(FLTdata.tfd.Firm_ver);
      GD.partial_flt();
      GT.firstime = 0;
    }
    if (buttonA)
    {
      GT.receiving_working_time = 0;
      GT.back_to_prev_status();
      buttonA = 0;
    }
    if (buttonB)

    {

      buttonB = 0;
    }
    if (buttonC)
    {
      if (GT.tfd_sent_no == 0)
      {
        GD.clean_partial_flt();
        GD.alabel = "Uploading....";
        GD.partial_flt();
        bool kk = send_loop_tfd();
      }
      else
      {
        GT.tfd_sent_no = 0;
        GT.back_to_prev_status();
      }
      buttonC = 0;
    }
    break;
  }
  case RECEIVED_FROM_TIMER:
  {
    // if (true)
    if (GT.firstime)
    {
      GD.clean_partial_flt();
      GD.alabel = "Received " + String(GT.tfd_received) + " rcds";
      GD.partial_flt();
      // GT.save_record();
      GT.firstime = 0;
    }
    if (buttonA)
    {
      GT.tfd_received = 0;
      GT.back_to_prev_status();
      buttonA = 0;
    }
    if (buttonB)
    {
      buttonB = 0;
    }
    if (buttonC)
    {
      GT.tfd_received = 0;
      GT.back_to_prev_status();
      buttonC = 0;
    }
    break;
  }

    //////////////////

  default:
  {
    GT.manage_wifi_input(); // this is like a new list for cases in FLT_mode in GTimer.cpp
  }
  break;
  }

  flight.manage_flight();

  if ((GD.sched_adv_millis_for_partial < millis()) && (GD.sched_adv_millis_for_partial > 0))
  {
    GD.sched_adv_millis_for_partial = 0;
    GD.displayStatus();
  }
  if (FLT_mode == NEW_KEEPER_TFD)
  {
    delay(10);
  }
  if (packet_received)
  {
    myudp.handleUdp();
    packet_received = 0;
  }
  blink_ontime(blink_ontime_flag);
  bool firsttime = 1;
  while (Serial1.available())
  {
    if (firsttime)
    {
      Serial.print("Read from panel bus: ");
      firsttime = 0;
    }
    Serial.print(char(Serial1.read()));
  }
}
