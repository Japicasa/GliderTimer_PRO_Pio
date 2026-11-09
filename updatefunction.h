//  updatefunction.h copio aqui la funcion /update de wifi para leerlo mejor
httpserver.on(
      "/update", HTTP_POST, [&](AsyncWebServerRequest *request)
      {
        // the request handler is triggered after the upload has finished...
        // create the response, add header, and send response
        AsyncWebServerResponse *response = request->beginResponse((Update.hasError()) ? 500 : 200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        response->addHeader("Connection", "close");
        response->addHeader("Access-Control-Allow-Origin", "*");
        request->send(response);
      },
      [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
      {
        //Upload handler chunks in data
        extern int update_counter;
        bool mycmd;
        if (!index)
        {
          // Iamupdating = 1;

          //vTaskDelay(700);
          uint8_t cmd = (filename.indexOf("Timer_Fw") > -1) || (filename.indexOf("Fw_PRO") > -1) || (filename.indexOf("Fw_mini") > -1) ? U_FLASH : U_SPIFFS;
          mycmd=(cmd<1);
          if (cmd)
            nvs.write_updateInProgress(cmd);
          if (!cmd)
          {
            Serial.print("update Flash");
            GD.updating_flash = 1;
            FLT_mode = UPDATE_IN_PROGRESS;
            //nvs.write_updateInProgress(cmd);
            if (!Update.begin())
            { // Start with max available size

              Update.printError(Serial);
            }
          }
          else
          {
            Serial.print("update SPIFFS");
            GD.updating_flash = 0;
            FLT_mode = UPDATE_IN_PROGRESS;
            if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd))
            { // Start with max available size
              Serial.print("update begin");
              Update.printError(Serial);
            }
          }
        }

        // Write chunked data to the free sketch space
        int kk = Update.write(data, len);
        if (kk != len)
        {
          Update.printError(Serial);
        }
        else
        {
          // Serial.print("len ");
          // Serial.println(kk);
        }

        if (final)
        { // if the final flag is set then this is the last frame of data
          if (Update.end(true))
          { //true to set the size to the current progress
             Serial.println("imprimiento para ver...");
            nvs.delete_updateInProgress();
            vTaskDelay(2000);
            // if((mycmd)&&(GT.stored_competition_no > 0)){ ///////////////format if there is data
            //   Serial.printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>formatting \n");
            //   bool exito = competition.format();
            //   Serial.printf("<<<<<<<<<<<<<<<<<<<format completed exito %d \n", exito);
            // }else{
            //   Serial.printf(">>>>>>>>>>>>>>>>>no format needed data was blank \n");
            // }

            ESP.restart();
          }
        }
      });