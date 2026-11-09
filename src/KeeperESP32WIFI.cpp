#include "keeper.h"
#include "KeeperESP32WIFI.h"

#include "nvs_flash.h"
#include "RTC.h"
#include "DebugPrint.h"

#include "NVSFunctions.h"

#include <esp_spiffs.h>

#include <SPIFFS.h>

// #include <esp_spiffs.h>
// #include "keeper.h"
// #include "KeeperESP32WIFI.h"
// #include "RTC.h"
// #include "DebugPrint.h"

// #include "NVSFunctions.h"

// #include <esp_spiffs.h>

const char *ssid = "GliderKeeper";
const char *password = "123456789";
const char *ssid1 = "GliderK";
const char *password1 = "123456789";

char localssid[12] = "GliderTimer";
// char *localssid = "GliderTimer";

char *localpassword = "123456789";

const char *host = "glidertimer";
const char *service = "_http";
const char *proto = "_tcp";
uint16_t port = 80;

const char *updateFirm = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update Firmware'></form>";
const char *updateSpiff = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update UI'></form>";
const char *uploadFile = "<form method='POST' action='/uploader' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Upload FILE'></form>";
const char EMERGENCY_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html lang='en'>

<head>
  <title>GliderKeeper</title>
  <meta charset='utf-8'>
  <meta name='viewport' content='width=device-width, initial-scale=1'>
  <meta charset='utf-8'>
  <style>
    div,input{padding:5px;font-size:1em;}
    input{width:95%;}
    body{text-align: left;font-family:verdana;}
    button{border:0;border-radius:0.3rem;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}
</style>
</head>

<body>

  <div >
    <h2>UI Update Error Discovered!</h2>
    <p> It seems there was an Ui update in progress and failed</p>
    <p></p>
    <div id='idOK' style='display: none;'>
      <p style = 'background-color: aquamarine;'>User Interface Update OK</p>
    </div>
    <div id='idError' class='mt-4 row' style='display: none;'>
      <p style = 'background-color: red; color: #fff;'>User Interface Update Error</p>
      <h2>Please Retry</h2>
    </div>

    <div id='idWait' class='mt-4' style='display: none;'>
      <h3>Updating UI...</h3>
        <div class='progress'>
            <div class='progress-bar progress-bar-striped bg-success' role='progressbar' style='width: 0%' id='idProgress'></div>
        </div>
        <p>Check device display</p>

    </div>

    <div id='idForm'>

      <h3 class='mt-2'>User Interface Update</h3>
      <!-- <form action='updateSpiff' enctype='multipart/form-data' method='POST'> -->
        <div class='form-group'>
          <div class='form-group'>
            <div class='custom-file mb-1'>
              <input type='file' class='custom-file-input' id='idUIFile' name='update'>
              <label class='custom-file-label' for='idUIFile'></label>
            </div>
          </div>
          <p>File should be Timer_UI_mini_x.xx.binformat </p>
          <div class='row'>
            <div class='mx-auto mt-3'>
              <input
              type='submit'
              id='btn-save'
              type='button'
              class='btn btn-info'
              value='Update'
            />
            </div>
          </div>

    </div>

  </div>
  <script>
        var xreq = new XMLHttpRequest();
        var interval;
        var devURL;
        var error = false;

        function initialize() {
        document.getElementById('btn-save').addEventListener('click', sendFile);
        devURL = './';
        }

        function sendFile() {
        var formData = new FormData();
        formData.append('update', document.getElementById('idUIFile').files[0]);


        xreq.onloadstart = function (e) {
            document.getElementById('idWait').style.display = 'block';
            document.getElementById('idForm').style.display = 'none';
        };
        xreq.onloadend = function (e) {
            console.log('end');
            if (!error)
                {
                    document.getElementById('idWait').style.display = 'none';
                    document.getElementById('idOK').style.display = 'block';
                }
        };

        xreq.addEventListener('error', function (e) {
            console.log('error');
            error = true;
            document.getElementById('idOK').style.display = 'none'
            document.getElementById('idWait').style.display = 'none';
            document.getElementById('idError').style.display = 'block';
            
        });

        xreq.addEventListener('abort', function (e) {
            console.log('abort');
            // error = true;
            // document.getElementById('idOK').style.display = 'none';
            // document.getElementById('idWait').style.display = 'none';
            // document.getElementById('idError').style.display = 'block';;
            }
        );

        xreq.upload.addEventListener(
            'progress',
            function (evt) {
            if (evt.lengthComputable) {
                document.getElementById('idProgress').innerHTML = ((evt.loaded / evt.total) * 100).toFixed(0) + '%';
                if (evt.loaded == evt.total) {
                document.getElementById('idWait').style.display = 'none';
                document.getElementById('idOK').style.display = 'block';
                xreq.abort();
                }
            }
            },
            false
        );
        xreq.open('POST', devURL + 'update');
        xreq.send(formData);
        
        }

        window.onload = initialize;

  </script>
</body>

</html>
)=====";
extern int FLT_mode;
extern String serialnumber;
extern String versionkeeper;
extern CompetitionSaved competition;
extern GTimer GT;
extern GTdisplay GD;
// extern Gthrow gth; //TODO quitando Gthrow
extern DebugPrint Debug;
extern Myudp myudp;
extern AsyncUDP udp;
extern bool blink_ontime_flag;
extern Flight_Manager flight;
extern Timer_flight_data FLTdata;
extern bool packet_received;
extern NVSClass nvs;
extern String fw_ui_version;
// extern DNSServer dnsServer;

File fsUploadFile;
const byte DNS_PORT = 53;       /// captive
IPAddress apIP(192, 168, 4, 1); /////captive
DNSServer dnsServer;            /// captive
AsyncWebServer httpserver(80);  // Create a webserver object that listens for HTTP request on port 80
void export_presults_csv(AsyncWebServerRequest *request);
void export_fullresults_csv(AsyncWebServerRequest *request);
void export_rresults_csv(AsyncWebServerRequest *request);
void export_fresults_csv(AsyncWebServerRequest *request);
void export_raw_data_csv(AsyncWebServerRequest *request);
void export_draw_results_csv(AsyncWebServerRequest *request);
void erase_keeperPart(AsyncWebServerRequest *request);
void eraseSPIFF(AsyncWebServerRequest *request);
void resetWiFi(AsyncWebServerRequest *request);
void saveWifi(AsyncWebServerRequest *request);
void receive_tfd(AsyncWebServerRequest *request);
void last_flight(AsyncWebServerRequest *request);
bool send_single_tfd();
bool perform_and_report_simple_tfd();
bool send_loop_tfd();
bool send_all_tfd();
void setWifitime(AsyncWebServerRequest *request);
void check_registered_pilots(AsyncWebServerRequest *request);
void savePilots(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total);
void beginUdp();

esp_err_t formatSPIFF();
uint return_export_presults_csv(unsigned char *buffer, size_t maxlen, size_t index);
uint return_export_fullresults_csv(unsigned char *buffer, size_t maxlen, size_t index);
uint return_export_rresults_csv(unsigned char *buffer, size_t maxlen, size_t index);
uint return_export_fresults_csv(unsigned char *buffer, size_t maxlen, size_t index);
uint return_export_raw_data_csv(unsigned char *buffer, size_t maxlen, size_t index);
uint return_export_draw_results_csv(unsigned char *buffer, size_t maxlen, size_t index);
void readGliderKeeperConfig();
void writeGliderKeeperConfig();
int download_scheduled_rounds(int file_num, bool main_sta);
struct gliderkeeperconfig gkConfig;
struct gliderkeeperconfig gkConfig_nvs;

void readGliderConfigNVS();
void writeGliderConfigNVS();
boolean checkNVSvalues();

// static bool STAconnected = 1;
class CaptiveRequestHandler : public AsyncWebHandler
{
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request)
  {
    // request->addInterestingHeader("ANY");
    return true;
  }

//   void handleRequest(AsyncWebServerRequest *request)
//   {
//     AsyncResponseStream *response = request->beginResponseStream("text/html");

//     if (nvs.check_updateInProgress())
//     {
//       request->send(200, "text/html", EMERGENCY_html);
//     }
//     else
//     {
//       request->redirect("http://192.168.4.1/index.html");
//     }
//   }

void handleRequest(AsyncWebServerRequest *request)
  {
    static int myfirsttime = 1;
    static bool update_in_progrress = nvs.check_updateInProgress();

    // if (myfirsttime == 1)
    // {
    //   update_in_progrress = nvs.check_updateInProgress();
    //   myfirsttime++;

    // }
    // Serial.println("X------->Handle request was invoked<-X" + String(millis()));
    if (request->url().length() > 0)
    // if (true)
    {
      // if (true)
      if ((myfirsttime < 4) || (request->url().indexOf('204') < 0))
      {
        myfirsttime++;
        if (update_in_progrress)
        {
          request->send(200, "text/html", EMERGENCY_html);
        }
        else
        {
          // Serial.println("X------->INDEX RETURNED URL was< ");
          // Serial.println("request"+request->url());
          // Serial.println(" >INDEX RETURNED <-X");
          request->redirect("http://192.168.4.1/index.html");
          // request->send(SPIFFS, "/index.html", "text/html", false);
          //  request->send(200, "text/html", "<h1>GliderKeeper Captive Portal, please indicate <br> -stay in this wifi without internet- </h1)"); //" + String(myfirsttime)+"
          // Serial.println("request Terminado"+request->url());
        }
      }
      else
      {
        // Serial.println("X------->Handle request bypassed");
        myfirsttime++;
        request->send(200, "text/html", "<h1>GliderKeeper Captive Portal, please indicate <br> -'stay in this wifi anyway'- in menu above :. <br> Then open your browser and type 192.168.4.1 <br> or gliderkeeper.local </h1)"); //" + String(myfirsttime)+"

        // Serial.println(request->url() + " OK; DARLING " + String(myfirsttime));
        // Serial.println("request extended"+request->url());
      }
    }
    else
    {
      // Serial.println("VOID URL...");
    }
  }
 };

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
  if (!index)
  {
    filename = "/" + filename;
    Serial.println((String) "UploadStart: " + filename);
    // open the file on first call and store the file handle in the request object
    request->_tempFile = SPIFFS.open(filename, "w");
  }
  if (len)
  {
    // stream the incoming chunk to the opened file
    request->_tempFile.write(data, len);
  }
  if (final)
  {
    Serial.println((String) "UploadEnd: " + filename + "," + index + len);
    // close the file handle as the upload is now done
    request->_tempFile.close();
    // removed for size TODO SIZE
    // if (filename == "/settings.txt")
    // {
    //   flight.print_settings();
    // }
    // request->send(200, "text/plain", "File Uploaded !");
  }
}

void savePilots(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
  char *filename = "/pilotos.csv";
  if (!index)
  {

    Serial.println((String) "UploadStart: " + filename);
    // open the file on first call and store the file handle in the request object
    request->_tempFile = SPIFFS.open(filename, "w");
  }
  if (len)
  {
    // stream the incoming chunk to the opened file
    request->_tempFile.write(data, len);
  }
  if (index + len == total)
  {
    Serial.println((String) "UploadEnd: " + filename + "," + index + len);
    // close the file handle as the upload is now done
    request->_tempFile.close();
    request->send(200, "text/plain", "Pilots Uploaded !");
  }
}

void wifiDisconnect()
{
  setCpuFrequencyMhz(80);
  Debug.println("** Stopping WiFi+BT");
  WiFi.mode(WIFI_OFF);
  btStop();
}

bool connecttowifi()
{
  STAconnected = 0;

  WiFi.mode(WIFI_OFF);
  delay(1); // captive tests
  btStop();
  delay(5);
  WiFi.mode(WIFI_AP);
  delay(5);
  // WiFi.softAPConfig(apIP, IPAddress(0, 0, 0, 0), IPAddress(255, 255, 255, 0)); //captive, SML este punto de acceso no debe ofrecer una ruta por defecto SML
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0)); // captive
  delay(1);
  // WiFi.softAP((String(localssid) + String(serialnumber)).c_str(), localpassword);
  if (GT.collector == 0)
  // if (GT.groups_enabled & (GT.collector == 0))
  {
    WiFi.softAP("GliderTmain", localpassword);
  }
  else
  {
    WiFi.softAP(String(localssid).c_str(), localpassword);
  }
  //      dns server is started in CPU 1
  // dnsServer.start(DNS_PORT, "*", apIP); ////// captive
  STAconnected = 0;
  WiFi.setSleep(false);

  // break;
  //   }
  // }
  Debug.println("");
  Debug.print("Connected! to : ");
  Debug.println(WiFi.SSID());
  Debug.print("Connected! IP address: ");
  String localIP = WiFi.localIP().toString();
  Debug.println(localIP);
  // Debug.println("Starting DNSServer");
  dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
  // Debug.println("Finish dnsServer");

  MDNS.begin(host);
  MDNS.addService(service, proto, port);

  // Debug.print("Open http://");
  // Debug.println(host);
  // myudp.begin();
  // beginUdp();
  setCpuFrequencyMhz(160);

  // Debug.println("Starting DNSServer");
  // dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
  // Debug.println("Finish dnsServer");
  delay(100);
  beginUdp(); /////cambiado de sitio parece que aqui va bien
  // httpserver.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); // only when requested from AP captive
  httpserver.begin();
  return STAconnected;
}

void set_server_functions()
{
  httpserver.on(
      "/uploader", HTTP_POST, [](AsyncWebServerRequest *request)
      { request->send(200); },
      handleUpload);

  httpserver.on(
      "/savePilots", HTTP_POST, [](AsyncWebServerRequest *request)
      { request->send(200); },
      NULL, savePilots);

// httpserver.on("/update... está en este archivo, por claridad
#include "updatefunction.h"

  httpserver.on("/pilotos.csv", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(SPIFFS, "/pilotos.csv"); });
  httpserver.on("/scheduled_rounds.csv", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(SPIFFS, "/scheduled_rounds.csv", "text/csv"); });

  httpserver.on("/presults.csv", HTTP_GET, [](AsyncWebServerRequest *request)
                { export_presults_csv(request); });

  // httpserver.on("/fullresults.csv", HTTP_GET, [](AsyncWebServerRequest *request)
  //               { export_fullresults_csv(request); });

  httpserver.on("/rresults.csv", HTTP_GET, [](AsyncWebServerRequest *request)
                { export_rresults_csv(request); });

  httpserver.on("/fresults.csv", HTTP_GET, [](AsyncWebServerRequest *request)
                { export_fresults_csv(request); });

  httpserver.on("/raw_data.csv", HTTP_GET, [](AsyncWebServerRequest *request)
                { export_raw_data_csv(request); });

  httpserver.on("/draw_results.csv", HTTP_GET, [](AsyncWebServerRequest *request)
                { export_draw_results_csv(request); });

  httpserver.on("/erase", HTTP_GET, [](AsyncWebServerRequest *request)
                { erase_keeperPart(request); });

  // httpserver.on("/eraseSpiff", HTTP_GET, [](AsyncWebServerRequest *request)
  //               { eraseSPIFF(request); });

  // httpserver.on("/UI_erase", HTTP_GET, eraseSPIFF);
  httpserver.on("/UI_erase", HTTP_GET, [](AsyncWebServerRequest *request)
                { eraseSPIFF(request); });

  // Guardamos la configuracion WiFi
  httpserver.on("/saveWifi", HTTP_POST, [](AsyncWebServerRequest *request)
                { saveWifi(request); });

  // httpserver.on("/resetwifi", HTTP_GET, resetWiFi);
  httpserver.on("/resetwifi", HTTP_GET, [](AsyncWebServerRequest *request)
                { resetWiFi(request); });

  // httpserver.on("/timer", HTTP_GET, receive_tfd); //trag to have a GET tfd received
  httpserver.on("/timer", HTTP_GET, [](AsyncWebServerRequest *request)
                { receive_tfd(request); });

  // httpserver.on("/timer", HTTP_GET, receive_tfd); //trag to respond with the launch data
  httpserver.on("/last_flight", HTTP_GET, [](AsyncWebServerRequest *request)
                { last_flight(request); });

  // httpserver.on("/id", HTTP_GET, check_registered_pilots);
  httpserver.on("/id", HTTP_GET, [](AsyncWebServerRequest *request)
                { check_registered_pilots(request); });
  // see and correct aging parameter of the RTC
  httpserver.on("/setAging", HTTP_GET, [](AsyncWebServerRequest *request)
                {
                  int8_t aging = 0;
                  if (request->hasArg("aging"))
                  {
                    aging = (int8_t)request->arg("aging").toInt();
                    local_RTC.write_aging(aging);
                  }
                  aging = local_RTC.read_aging();
                  //String label = "Aging is now: " + String(aging) +" Last RTC update was at: "+ String(local_RTC.last_sync_val.tv_sec) ;
                  char lbl[55];
                  snprintf(lbl,55,"Aging is now: %d Last RTC update was at: %d",aging ,local_RTC.last_sync_val.tv_sec  );
                  request->send(200, "text/html", lbl); });

  // create dummy group dat, for testing
  httpserver.on("/dummy", HTTP_GET, [](AsyncWebServerRequest *request)
                {
                  FLTdata.create_dummy_group_data(GT.stored_round_no, GT.stored_group_no);
                  request->send(200, "text/html", "Dummy created"); });
  httpserver.on("/get_fw_version", HTTP_GET, [](AsyncWebServerRequest *request)
                {request->send(200, "text/html", fw_ui_version); });
                // {request->send(200, "text/html", FW_VERSION); });

  //////////////////Abajo, las funciones raras ////////////////////////////
  httpserver.on("/P_update", HTTP_GET, [](AsyncWebServerRequest *request) { /////////////////////// update ESP32
    request->send(200, "text/html", updateFirm);
  });

  httpserver.on("/UI_update", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(200, "text/html", updateSpiff); });

  httpserver.on("/P_upload", HTTP_GET, [](AsyncWebServerRequest *request)
                { request->send(200, "text/html", uploadFile); });

  httpserver.on("/blink", HTTP_GET, [](AsyncWebServerRequest *request)
                {
                  blink_ontime_flag = !blink_ontime_flag;
                  request->send(200, "text/html", "Blink on odd seconds condition has changed"); });
  httpserver.on("/timeSet", HTTP_GET, [](AsyncWebServerRequest *request)
                { setWifitime(request); });
  httpserver.on("/sched", HTTP_GET, [](AsyncWebServerRequest *request)
                {
                  String label;
                  if (request->hasArg("secs"))
                  {
                    flight.schedule_competition((uint32_t)request->arg("secs").toInt());
                    label = "ok. check in scheduled_rounds.csv";
                    AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/scheduled_rounds.csv", "text/csv");
                    request->send(response);
                  }
                  else
                  {
                    label = "missing argument : secs";
                    request->send(200, "text/html", label);
                  } });
  httpserver.on("/collector", HTTP_GET, [](AsyncWebServerRequest *request)
                {
                  // String label;
                  // if (request->hasArg("master"))
                  // {
                  //   GT.collector=((uint32_t)request->arg("master").toInt());
                  //   label = "ok. collector value updated check in settings.txt";
                  // }
                  // else
                  // {
                  //   label = "missing argument : master= 0/1/2";
                  // }
                  request->send(200, "text/html", "OK"); });
  // request->send(200, "text/html", label); });
  // httpserver.on("/play", HTTP_GET, [](AsyncWebServerRequest *request)
  //               {
  //                 String label;
  //                 if (request->hasArg("ad"))
  //                 {
  //                   MP3.ad_file((uint32_t)request->arg("ad").toInt());
  //                   label = "ok. ad file";
  //                 }
  //                 if (request->hasArg("play"))
  //                 {
  //                    MP3.play_something();
  //                   label = "ok. play something";
  //                 }

  //                 request->send(200, "text/html", label); });
  httpserver.on("/setVol", HTTP_GET, [](AsyncWebServerRequest *request)
                {
                  String label = "Syntax is /setVol?music(or advert)=desired_value <br> Status is music= " + String(MP3.music_vol) + " / advert= " + String(MP3.adv_vol);
                  if (request->hasArg("music"))
                  {
                    MP3.music_vol = (int8_t)request->arg("music").toInt();
                    if (MP3.music_vol > 30)
                      MP3.music_vol = 30;
                    if (MP3.music_vol < 0)
                      MP3.music_vol = 0;
                    label = "MUsic Volume set to : " + String(MP3.music_vol);
                  }
                  if (request->hasArg("advert"))
                  {
                    MP3.adv_vol = (int8_t)request->arg("advert").toInt();
                    if (MP3.adv_vol > 30)
                      MP3.adv_vol = 30;
                    if (MP3.adv_vol < 0)
                      MP3.adv_vol = 0;
                    label = "MUsic Volume set to : " + String(MP3.adv_vol);
                  }
                  if (request->hasArg("ad"))
                  {

                  MP3.play_something();

                    // MP3.ad_file((int)request->arg("ad").toInt());
                    label = "ok. ad file";
                  }
                  // if (request->hasArg("play"))
                  // {
                  //    MP3.play_something();
                  //   label = "ok. play something";
                  // }
                  request->send(200, "text/html", label); });

  // httpserver.serveStatic("/", SPIFFS, "/").setCacheControl("max-age=600").setDefaultFile("index.html"); //servir el archivo
  httpserver.onNotFound([](AsyncWebServerRequest *request) { // Servir index si no se encuentra nada
    request->send(SPIFFS, "/index.html");
    // request->send(404);  ///Esto es lo que habia antes, ahora mola más
  });

  if (nvs.check_updateInProgress())
  {
    Serial.println("UI UPDATE IN PROGRESS ABORTED");
    httpserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(200, "text/html", EMERGENCY_html); });
    httpserver.onNotFound([](AsyncWebServerRequest *request)
                          { request->send(200, "text/html", EMERGENCY_html); });
  }
  else
  {
    httpserver.onNotFound([](AsyncWebServerRequest *request)
                          { request->send(SPIFFS, "/index.html"); });

    httpserver.serveStatic("/", SPIFFS, "/").setCacheControl("max-age=10").setDefaultFile("index.html");
  }
}

// format bytes
String formatBytes(size_t bytes)
{
  if (bytes < 1024)
  {
    return String(bytes) + "B";
  }
  else if (bytes < (1024 * 1024))
  {
    return String(bytes / 1024.0) + "KB";
  }
  else if (bytes < (1024 * 1024 * 1024))
  {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  }
  else
  {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

void eraseSPIFF(AsyncWebServerRequest *request)
{
  // static const esp_partition_t *spiffpartition;
  // esp_err_t error;
  // AsyncWebServerResponse *response;

  // error = formatSPIFF();

  // if (error)
  // {
  //   response = request->beginResponse(500, "text/plain", "ERASE FAILED");
  // }
  // else
  // {
  //   response = request->beginResponse(200, "text/plain", "Erase Successfull");
  // }

  // response->addHeader("Access-Control-Allow-Origin", "*");
  // request->send(response);
  // vTaskDelay(1000);

  // ESP.restart();
}

esp_err_t formatSPIFF()
{
  // static const esp_partition_t *spiffpartition;
  // esp_err_t error;

  // Serial.printf("Erasing SPIFF");

  // spiffpartition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, "spiffs");

  // if (!spiffpartition)
  // {
  //   Serial.println("Find Error");
  //   error = true;
  //   return error;
  // }
  // disableCore0WDT();
  // disableCore1WDT();
  // Serial.println("Unmounting SPIFF");
  // SPIFFS.end();
  // Serial.println("Erasing SPIFF");
  // //    error = esp_partition_erase_range(spiffpartition, 0, 512 * 4096);
  // // error = esp_partition_erase_range(spiffpartition, 0, spiffpartition->size);

  // error = spi_flash_erase_range(spiffpartition->address, spiffpartition->size);
  // // SPIFFS.format();
  // enableCore0WDT();
  // enableCore1WDT();
  // if (error != ESP_OK)
  // {
  //   Serial.println("Erase Error: " + String(error));
  //   error = true;
  //   return error;
  // }

  // return false;
}

void setWifitime(AsyncWebServerRequest *request)
{
  AsyncWebServerResponse *response;
  String error = local_RTC.SetRTC();
  if (error == "")
  {
    response = request->beginResponse(500, "text/plain", "RTCset FAILED");
  }
  else
  {
    error = "RTC set Successfull to :" + error;
    response = request->beginResponse(200, "text/plain", error);
  }

  response->addHeader("Access-Control-Allow-Origin", "*");
  request->send(response);
  vTaskDelay(1000);

  // ESP.restart();
}
// TODO borrada por sitio
void export_presults_csv(AsyncWebServerRequest *request)
{

  unsigned char buffer[500];
  size_t index = 0;

  AsyncWebServerResponse *response = request->beginChunkedResponse("text/csv", [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t
                                                                   { return return_export_presults_csv(buffer, 500, index); });

  response->addHeader("Access-Control-Allow-Origin", "*");
  response->addHeader("Cache-Control", "max-age=20");
  request->send(response);
}
uint return_export_presults_csv(unsigned char *buffer, size_t maxlen, size_t index)
{
  static int piloti = 0;
  static int roundi = 0;
  if (index == 0)
  {
    piloti = 1;
    roundi = 1;
    strcpy((char *)buffer, "Comp;P;id;FLTnum;Round;F5Jh;time;ldist;raw_score;FAI_pts;penalty;Round_rank;\n");
    // strcpy((char *)buffer, out.c_str());
    if (GT.groups_enabled)
    {
      strcat((char *)buffer, "NOT ; AVAILABLE ; in GROUPS\n");
    }
    return strlen((char *)buffer);
  }
  if (GT.groups_enabled)
  {
    return 0;
  }
  if (piloti < (GT.mycompetition.head.number_of_pilots + 1))
  {
    // if (roundi < (GT.mycompetition.head.number_of_rounds + 1))
    if (roundi < (7))
    {
      int pilotj = piloti;
      int timeTC = GT.draw_data[roundi][pilotj].flight_mins * 100;
      timeTC = timeTC + GT.draw_data[roundi][pilotj].flight_secs;

      sprintf((char *)buffer, "%d;%d;%s;%d;%d;%d;%d;%d;%d;%d;%d;%d;\n",
              GT.current_competition_no,
              piloti,
              GT.draw_data[roundi][pilotj].serialnumber,
              GT.draw_data[roundi][pilotj].fltnum,
              roundi,
              (((float)GT.draw_data[roundi][pilotj].F5J_height / 10), 1),
              timeTC,
              GT.draw_data[roundi][pilotj].landing_dist,
              GT.draw_data[roundi][pilotj].raw_score,
              GT.mycompetition.draw_list[roundi][pilotj].FAI_score,
              GT.draw_data[roundi][pilotj].penalty_pts,
              GT.mycompetition.draw_list[roundi][pilotj].round_rank);
    }
    roundi++;
    if (roundi > GT.mycompetition.head.number_of_rounds)
    {
      roundi = 1;
      piloti++;
    }
    return strlen((char *)buffer);
  }
  return 0;
}

/// TODO borrada por sitio
// void export_fullresults_csv(AsyncWebServerRequest *request)
// {

//   unsigned char buffer[500];
//   size_t index = 0;

//   AsyncWebServerResponse *response = request->beginChunkedResponse("text/csv", [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t
//                                                                    { return return_export_fullresults_csv(buffer, 500, index); });

//   response->addHeader("Access-Control-Allow-Origin", "*");
//   response->addHeader("Cache-Control", "max-age=20");
//   request->send(response);
// }

/// TODO borrada por sitio
// uint return_export_fullresults_csv(unsigned char *buffer, size_t maxlen, size_t index)
// {
//   static int piloti = 0;
//   static int roundi = 0;
//   String out;
//   if (index == 0)
//   {
//     piloti = 1;
//     roundi = 1;
//     out = "Comp;P;id;FLTnum;Round;F5Jh;time;ldist;raw_score;FAI_pts;penalty;Round_rank;emergency_used;from_NVS;Firm_ver;timedout;take_off_millis;takeoff_local_secs;takeoff_local_microsecs;landing_millis;transmit_local_secs;transmit_local_usecs;local_landing_timeout_sec;local_landing_timeout_usec;\n ";
//     strcpy((char *)buffer, out.c_str());
//     return strlen((char *)buffer);
//   }
//   if (piloti < (GT.mycompetition.head.number_of_pilots + 1))
//   {
//     // if (roundi < (GT.mycompetition.head.number_of_rounds + 1))
//     if (roundi < (7))
//     {
//       int pilotj = piloti;
//       int timeTC = GT.draw_data[roundi][pilotj].flight_mins * 100;
//       timeTC = timeTC + GT.draw_data[roundi][pilotj].flight_secs;

//       out = String(GT.current_competition_no) + ";" +
//             String(piloti) + ";" +
//             String(GT.draw_data[roundi][pilotj].serialnumber) + ";" +
//             String(GT.draw_data[roundi][pilotj].fltnum) + ";" +
//             String(roundi) + ";" +
//             String(((float)GT.draw_data[roundi][pilotj].F5J_height / 10), 1) + ";" +
//             String(timeTC) + ";" +
//             String(GT.draw_data[roundi][pilotj].landing_dist) + ";" +
//             String(GT.draw_data[roundi][pilotj].raw_score) + ";" +
//             String(GT.mycompetition.draw_list[roundi][pilotj].FAI_score) + ";" +
//             String(GT.draw_data[roundi][pilotj].penalty_pts) + ";" +
//             String(GT.mycompetition.draw_list[roundi][pilotj].round_rank) + ";" +
//             String(GT.draw_data[roundi][pilotj].emergency_used) + ";" +
//             String(GT.draw_data[roundi][pilotj].from_NVS) + ";" +
//             String(GT.draw_data[roundi][pilotj].Firm_ver) + ";" +
//             String(GT.draw_data[roundi][pilotj].timedout) + ";" +
//             String(GT.draw_data[roundi][pilotj].take_off_millis) + ";" +
//             String(GT.draw_data[roundi][pilotj].takeoff_local_secs) + ";" +
//             String(GT.draw_data[roundi][pilotj].takeoff_local_microsecs) + ";" +
//             String(GT.draw_data[roundi][pilotj].landing_millis) + ";" +
//             String(GT.draw_data[roundi][pilotj].transmit_local_secs) + ";" +
//             String(GT.draw_data[roundi][pilotj].transmit_local_usecs) + ";" +
//             String(GT.draw_data[roundi][pilotj].local_landing_timeout_sec) + ";" +
//             String(GT.draw_data[roundi][pilotj].local_landing_timeout_usec) + ";" +
//             ";\n";
//     }
//     roundi++;
//     if (roundi > GT.mycompetition.head.number_of_rounds)
//     {
//       roundi = 1;
//       piloti++;
//     }
//     strcpy((char *)buffer, out.c_str());
//     return strlen((char *)buffer);
//   }
//   return 0;
// }

void export_rresults_csv(AsyncWebServerRequest *request)
{
  unsigned char buffer[500];
  size_t index = 0;
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/csv", [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t
                                                                   { return return_export_rresults_csv(buffer, 500, index); });
  response->addHeader("Access-Control-Allow-Origin", "*");
  response->addHeader("Cache-Control", "max-age=20");
  request->send(response);
}

uint return_export_rresults_csv(uint8_t *buffer, size_t maxLen, size_t index)
{
  static int piloti = 0;
  static int roundi = 0;
  static int groupi = 0;
  if (index == 0)
  {
    piloti = 1;
    roundi = 1;
    groupi = 1;
    strcpy((char *)buffer, "Comp;Round;P;id;FLtnum;F5Jh;time;ldist;raw_score;FAI_pts;penalty;Round_rank;\n");
    if (GT.groups_enabled)
    {
      strcat((char *)buffer, "NOT ; AVAILABLE ; in GROUPS\n");
    }
    return strlen((char *)buffer);
  }
  if (GT.groups_enabled)
  {
    return 0;
  }
  if (roundi < (GT.mycompetition.head.number_of_rounds + 1))
  {
    if (piloti < (GT.mycompetition.head.number_of_pilots + 1))
    {
      int pilotj = GT.rank_list[roundi][groupi][piloti];
      int timeTC = GT.draw_data[roundi][pilotj].flight_mins * 100;
      timeTC = timeTC + GT.draw_data[roundi][pilotj].flight_secs;
      sprintf((char *)buffer, "%d;%d;%d;%s;%d;%d;%d;%d;%d;%d;%d;%d;\n",
              GT.current_competition_no,
              roundi,
              pilotj,
              GT.draw_data[roundi][pilotj].serialnumber,
              GT.draw_data[roundi][pilotj].fltnum,
              (((float)GT.draw_data[roundi][pilotj].F5J_height / 10), 1),
              timeTC,
              GT.draw_data[roundi][pilotj].landing_dist,
              GT.draw_data[roundi][pilotj].raw_score,
              GT.mycompetition.draw_list[roundi][pilotj].FAI_score,
              GT.draw_data[roundi][pilotj].penalty_pts,
              GT.mycompetition.draw_list[roundi][pilotj].round_rank);
    }
    piloti++;
    if (piloti > GT.mycompetition.head.number_of_pilots)
    {
      piloti = 1;
      roundi++;
    }
    return strlen((char *)buffer);
  }
  return 0;
}

void export_raw_data_csv(AsyncWebServerRequest *request)
{
  unsigned char buffer[600];
  size_t index = 0;
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/csv", [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t
                                                                   { return return_export_raw_data_csv(buffer, 600, index); });
  response->addHeader("Access-Control-Allow-Origin", "*");
  response->addHeader("Cache-Control", "max-age=20");
  request->send(response);
}

uint return_export_raw_data_csv(uint8_t *buffer, size_t maxLen, size_t index)
{
  static bool found = 0;
  datastore tfd_data;
  char mypayload[600];
  if (index == 0)
  {
    // FLTdata.datastore2String(tfd_data, ";", mypayload);
    snprintf(mypayload, 600, "comp_no;record_num;round_no;group_no;pilot_no;pilot_id;fltnum;take_off_millis;takeoff_local_secs;takeoff_local_microsecs;throttle_shut_millis;F5J_height;F5J_millis;landing_millis;rest_millis;flight_mins;flight_secs;transmit_local_secs;transmit_local_usecs;timer_to_keeper_sec;timer_to_keeper_usec;emergency_used;from_NVS;local_landing_timeout_sec;local_landing_timeout_usec;timedout;landing_dist;penalty_pts;raw_score;serialnumber;Firm_ver;timer_serialnumber;timer_Firm_ver;\n");
    strcpy((char *)buffer, mypayload);
    return strlen((char *)buffer);
  }
  if (!found)
  {
    found = 1;
    strcpy(mypayload, "");
    if (competition.getFirstRecord(GT.mycompetition.head.contestNum, &tfd_data))
    {
      FLTdata.datastore2String(tfd_data, ";", mypayload);
      strcpy((char *)buffer, mypayload);
      return strlen((char *)buffer);
    }
  }
  else
  {
    if (competition.getNextRecord(&tfd_data))
    {
      FLTdata.datastore2String(tfd_data, ";", mypayload);
      strcpy((char *)buffer, mypayload);
      return strlen((char *)buffer);
    }
  }
  return 0;
}
void export_draw_results_csv(AsyncWebServerRequest *request)
{
  unsigned char buffer[600];
  size_t index = 0;
  AsyncWebServerResponse *response = request->beginChunkedResponse("text/csv", [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t
                                                                   { return return_export_draw_results_csv(buffer, 600, index); });
  response->addHeader("Access-Control-Allow-Origin", "*");
  response->addHeader("Cache-Control", "max-age=20");
  request->send(response);
}

uint return_export_draw_results_csv(uint8_t *buffer, size_t maxLen, size_t index)
{
  static bool found = 0;
  static int piloti = 0;
  static int roundi = 0;
  static int groupi = 0;
  char mypayload[600];
  if (index == 0)
  {
    piloti = 1;
    roundi = 1;
    groupi = 1;
    snprintf(mypayload, 600, "CompNo;TaskNo;RoundNo;GroupNo;ReFlightNo;PilotNo;Data1;Data2;Data3;Data4;Data5;Data6;Data7;Penalty;Name;\n");
    strcpy((char *)buffer, mypayload);
    return strlen((char *)buffer);
  }
  // CompNo
  snprintf(mypayload, 600, "%d", GT.mycompetition.head.external_comp_no);
  if (roundi < (GT.mycompetition.head.number_of_rounds + 1))
  // if (roundi < (7))
  {
    if (piloti < (GT.mycompetition.head.number_of_pilots + 1))
    {
      // int pilotj = GT.rank_list[roundi][groupi][piloti];
      int pilotj = piloti;
      int timeTC = GT.draw_data[roundi][pilotj].flight_mins * 100; // tiempo centesimal
      timeTC = timeTC + GT.draw_data[roundi][pilotj].flight_secs;
      // TaskNo
      snprintf(mypayload, 600, "%s;1", mypayload); /// le pongo task=1
      // round
      snprintf(mypayload, 600, "%s;%d", mypayload, roundi);
      // group
      snprintf(mypayload, 600, "%s;%d", mypayload, GT.mycompetition.draw_list[roundi][pilotj].group);
      // ReFlight=00
      snprintf(mypayload, 600, "%s;0", mypayload); /// le pongo reflight 0
      // PilotNo ->Pilot_ID
      snprintf(mypayload, 600, "%s;%d", mypayload, GT.mycompetition.draw_list[roundi][pilotj].pilot_id);
      // data1=1
      snprintf(mypayload, 600, "%s;0", mypayload); /// le pongo data1=0
      // data2 = mss
      snprintf(mypayload, 600, "%s;%d", mypayload, timeTC);
      /// le pongo data3,4,5 =0
      snprintf(mypayload, 600, "%s;0;0;0", mypayload); /// le pongo data3,4,5 =0
      /// data6  F5J height
      snprintf(mypayload, 600, "%s;%d", mypayload, (int)(GT.draw_data[roundi][pilotj].F5J_height / 10));
      /// data7 landing dist
      snprintf(mypayload, 600, "%s;%d", mypayload, GT.draw_data[roundi][pilotj].landing_dist);
      // Penalty= 0
      snprintf(mypayload, 600, "%s;0", mypayload); /// le pongo penalty=0
      // name = "name"
      snprintf(mypayload, 600, "%s;%s", mypayload, "name"); // TODO name ***
      // snprintf(mypayload, 600, "%s;%s", mypayload, GT.pilots_table[pilotj].name);  //TODO name ***
      snprintf(mypayload, 600, "%s\n", mypayload); // y final no medaja poner el ; al final este gliderscore
    }
    piloti++;
    if (piloti > GT.mycompetition.head.number_of_pilots)
    {
      piloti = 1;
      roundi++;
    }
    strcpy((char *)buffer, mypayload);
    return strlen((char *)buffer);
  }
  return 0;
}

void export_fresults_csv(AsyncWebServerRequest *request)
{

  unsigned char buffer[500];
  size_t index = 0;

  AsyncWebServerResponse *response = request->beginChunkedResponse("text/csv", [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t
                                                                   { return return_export_fresults_csv(buffer, 500, index); });

  response->addHeader("Access-Control-Allow-Origin", "*");
  response->addHeader("Cache-Control", "max-age=20");
  request->send(response);
}

uint return_export_fresults_csv(uint8_t *buffer, size_t maxLen, size_t index)
{
  static int piloti = 1;
  static int roundi = 0;
  static int groupi = 0;
  if (index == 0)
  {
    strcpy((char *)buffer, "Comp;Rank;P;id;TotFAI;Round1;Round2;Round3;Round4;Round5;Round6;tPenalty;Discarded;\n");
    if (GT.groups_enabled)
    {
      strcat((char *)buffer, "NOT ; AVAILABLE ; in GROUPS\n");
    }
    piloti = 1;
    roundi = 0;
    return strlen((char *)buffer);
  }
  if (GT.groups_enabled)
  {
    return 0;
  }
  if (piloti < ((GT.mycompetition.head.number_of_pilots + 1)))
  {
    int pilotj = GT.rank_list[0][groupi][piloti];
    uint16_t pilotj_ID = GT.mycompetition.draw_list[roundi][pilotj].pilot_id;
    strcpy((char *)buffer, "");
    sprintf((char *)buffer, "%d;%d;%d;%s;%d;", GT.current_competition_no,
            piloti, pilotj_ID, GT.pilots_table[pilotj_ID].serialnumber,
            GT.mycompetition.draw_list[0][pilotj].FAI_score);
    for (int roundk = 1; roundk < MAX_SELECTION_POINTER_ROUND; roundk++)
    {
      sprintf((char *)buffer, "%s%d;", buffer, GT.mycompetition.draw_list[roundk][pilotj].FAI_score);
    }
    strcat((char *)buffer, "\n");
    if (GT.mycompetition.draw_list[0][pilotj].FAI_score == 0)
    { // no point recorded for these pilos anymore
      return 0;
    }
    piloti++;
    return strlen((char *)buffer);
  }
  return 0;
}

void export_savebin_csv()
{
  // unsigned short maxvuelo = vuelosFlash.init();
  // unsigned short numvuelo;
  // struct datastore *mydata;
  // struct timer_flight_data *fltdata;

  // bool firstime = true;
  // String out;
  // server.enableCORS(true);
  // server.client().println("HTTP/1.1 200 OK");
  // server.client().println("Content-Type: text/csv");
  // server.client().println("Access-Control-Allow-Origin:*"); // Send enableCORS

  // server.client().println("Connection: close"); // the connection will be closed after completion of the response
  // server.client().println();

  // for (numvuelo = 1; numvuelo <= maxvuelo; numvuelo++)
  // {
  //   fltdata = vuelosFlash.getFlightData(numvuelo);
  //   if (fltdata == NULL)
  //   {
  //     return; // A tomar por saco, no ha nada
  //   }
  //   mydata = vuelosFlash.getFirstRecord(numvuelo);
  //   while (mydata != NULL)

  //   {
  //     if (firstime)
  //     {
  //       firstime = false;
  //       out = "FLTnum; record_num; millis; pressure; gzsm; z; tap; altitude; F5J_alt; thrtl_in; thrtl_out; temperature; jsvario";
  //       server.client().println(out);
  //     }
  //     out = String(fltdata->FLTnum) + ";" +
  //           String(mydata->record_num) + ";" +
  //           String(mydata->millis) + ";" +
  //           String(mydata->pressure) + ";" +
  //           String(mydata->gzsm) + ";" +
  //           String(mydata->z) + ";" +
  //           String(mydata->tap) + ";" +
  //           String(mydata->altitude) + ";" +
  //           String(mydata->F5J_alt) + ";" +
  //           String(mydata->thrtl_in) + ";" +
  //           String(mydata->thrtl_out) + ";" +
  //           String(mydata->temperature) + ";" +
  //           String(mydata->jvarioREC) + "\n";
  //     server.client().print(out);
  //     mydata = vuelosFlash.getNextRecord();
  //   }
  // }
  // server.client().stop();
}

void readGliderKeeperConfig()
{

  strcpy(gkConfig.WiFiSSID, localssid); // Por si las moscas
  strcpy(gkConfig.WifiPass, localpassword);
  gkConfig.Training = true;
  gkConfig.FAImode = false;
  gkConfig.EmergencyMotorEnable = true;
  gkConfig.ShowFlightOLED = true;
  gkConfig.AltitudeSwitch = false;
  gkConfig.AltitudeTrigger = 200;
  gkConfig.TimeSwitch = false;
  gkConfig.TimeTrigger = 20;
  gkConfig.GliderLink = false;
  gkConfig.AlwaysOn = true;
  gkConfig.MotorLogging = false;
  gkConfig.AltitudeLogging = false;
  gkConfig.AtitudeLoggingSetting = 5; // Defaults to 5 meters
  gkConfig.GKOrientation = WA_FLAT;   //
  gkConfig.QRreader[0] = 0;           // Null terminate QRreader
  strcpy(gkConfig.WiFiSSID1, localssid);
  strcpy(gkConfig.WifiPass1, localpassword);
  gkConfig.TimeForConnect = 25;
  gkConfig.MillisForReconect = 500;
  gkConfig.TimeForConnect = 25;
  gkConfig.MillisForReconect = 500;
  gkConfig.jerk_thr = 300;
  gkConfig.gzsm_thr = 15;
  gkConfig.landing_pressure_thr = 100;

  File file = SPIFFS.open("/GliderKeeperConfig.txt", "r");
  if (file)
  {
    String line = file.readStringUntil('\n');
    while (file.available())
    {
      short c = line.indexOf(':');
      String key = line.substring(0, c);
      String value = line.substring(c + 1, line.length() - 1);
      if (key.equalsIgnoreCase("WiFiSSID"))
      {
        strncpy(gkConfig.WiFiSSID, value.c_str(), sizeof(gkConfig.WiFiSSID));
        gkConfig.WiFiSSID[sizeof(gkConfig.WiFiSSID) - 1] = 0; // Mark end of string
      }
      else if (key.equalsIgnoreCase("WifiPass"))
      {
        strncpy(gkConfig.WifiPass, value.c_str(), sizeof(gkConfig.WifiPass));
        gkConfig.WifiPass[sizeof(gkConfig.WifiPass) - 1] = 0; // Mark end of string
      }
      else if (key.equalsIgnoreCase("Training"))
      {
        gkConfig.Training = value.equalsIgnoreCase("y");
      }
      else if (key.equalsIgnoreCase("FAImode"))
      {
        gkConfig.FAImode = value.equalsIgnoreCase("y");
      }
      else if (key.equalsIgnoreCase("EmergencyMotorEnable"))
      {
        gkConfig.EmergencyMotorEnable = value.equalsIgnoreCase("y");
      }
      else if (key.equalsIgnoreCase("ShowFlightOLED"))
      {
        gkConfig.ShowFlightOLED = value.equalsIgnoreCase("y");
      }
      else if (key.equalsIgnoreCase("AltitudeSwitch"))
      {
        gkConfig.AltitudeSwitch = value.equalsIgnoreCase("y");
      }
      else if (key.equalsIgnoreCase("TimeSwitch"))
      {
        gkConfig.TimeSwitch = value.equalsIgnoreCase("y");
      }
      else if (key.equalsIgnoreCase("AlwaysOn"))
      {
        gkConfig.AlwaysOn = value.equalsIgnoreCase("y");
      }
      else if (key.equalsIgnoreCase("MotorLogging"))
      {
        gkConfig.MotorLogging = value.equalsIgnoreCase("y");
      }
      else if (key.equalsIgnoreCase("AltitudeLogging"))
      {
        gkConfig.AltitudeLogging = value.equalsIgnoreCase("y");
      }
      else if (key.equalsIgnoreCase("GliderLink"))
      {
        gkConfig.GliderLink = value.equalsIgnoreCase("y");
      }
      else if (key.equalsIgnoreCase("GKOrientation"))
      {
        gkConfig.GliderLink = value.equalsIgnoreCase("flat") ? WA_FLAT : WA_VERTICAL;
      }

      else if (key.equalsIgnoreCase("AltitudeTrigger"))
      {
        gkConfig.AltitudeTrigger = value.toInt();
      }
      else if (key.equalsIgnoreCase("TimeTrigger"))
      {
        gkConfig.TimeTrigger = value.toInt();
      }
      else if (key.equalsIgnoreCase("AtitudeLoggingSetting"))
      {
        gkConfig.AtitudeLoggingSetting = value.toInt();
      }
      else if (key.equalsIgnoreCase("QRreader"))
      {
        strncpy(gkConfig.QRreader, value.c_str(), sizeof(gkConfig.QRreader));
        gkConfig.QRreader[sizeof(gkConfig.QRreader) - 1] = 0; // Mark end of string
      }
      else if (key.equalsIgnoreCase("WiFiSSID1"))
      {
        strncpy(gkConfig.WiFiSSID1, value.c_str(), sizeof(gkConfig.WiFiSSID1));
        gkConfig.WiFiSSID1[sizeof(gkConfig.WiFiSSID1) - 1] = 0; // Mark end of string
      }
      else if (key.equalsIgnoreCase("WifiPass1"))
      {
        strncpy(gkConfig.WifiPass1, value.c_str(), sizeof(gkConfig.WifiPass1));
        gkConfig.WifiPass1[sizeof(gkConfig.WifiPass1) - 1] = 0; // Mark end of string
      }
      else if (key.equalsIgnoreCase("TimeForConnect"))
      {
        gkConfig.TimeForConnect = value.toInt();
      }
      else if (key.equalsIgnoreCase("MillisForReconect"))
      {
        gkConfig.MillisForReconect = value.toInt();
      }
      else if (key.equalsIgnoreCase("jerk_thr"))
      {
        gkConfig.jerk_thr = value.toInt();
      }
      else if (key.equalsIgnoreCase("gzsm_thr"))
      {
        gkConfig.gzsm_thr = value.toInt();
      }
      else if (key.equalsIgnoreCase("landing_pressure_thr"))
      {
        gkConfig.landing_pressure_thr = value.toInt();
      }

      line = file.readStringUntil('\n');
    }

    file.close();
  }
  readGliderConfigNVS();
}

void writeGliderKeeperConfig()
{
  String resp;

  File file = SPIFFS.open("/GliderKeeperConfig.txt", "w");
  file.println("WiFiSSID:" + String(gkConfig.WiFiSSID));
  file.println("WifiPass:" + String(gkConfig.WifiPass));

  resp = gkConfig.Training ? "y" : "n";
  file.println("Training:" + resp);

  resp = gkConfig.FAImode ? "y" : "n";
  file.println("FAImode:" + resp);

  resp = gkConfig.EmergencyMotorEnable ? "y" : "n";
  file.println("EmergencyMotorEnable:" + resp);

  resp = gkConfig.ShowFlightOLED ? "y" : "n";
  file.println("ShowFlightOLED:" + resp);

  resp = gkConfig.AltitudeSwitch ? "y" : "n";
  file.println("AltitudeSwitch:" + resp);

  resp = gkConfig.TimeSwitch ? "y" : "n";
  file.println("TimeSwitch:" + resp);

  resp = gkConfig.GliderLink ? "y" : "n";
  file.println("GliderLink:" + resp);

  resp = gkConfig.AlwaysOn ? "y" : "n";
  file.println("AlwaysOn:" + resp);

  resp = gkConfig.MotorLogging ? "y" : "n";
  file.println("MotorLogging:" + resp);

  resp = gkConfig.AltitudeLogging ? "y" : "n";
  file.println("AltitudeLogging:" + resp);

  file.println("AltitudeTrigger:" + String(gkConfig.AltitudeTrigger));
  file.println("AtitudeLoggingSetting:" + String(gkConfig.AtitudeLoggingSetting));
  file.println("TimeTrigger:" + String(gkConfig.TimeTrigger));
  String orientation = (gkConfig.GKOrientation == WA_FLAT) ? "flat" : "vertical";
  file.println("GKOrientation:" + orientation);
  file.println("QRreader:" + String(gkConfig.QRreader));

  file.println("WiFiSSID1:" + String(gkConfig.WiFiSSID1));
  file.println("WifiPass1:" + String(gkConfig.WifiPass1));
  file.println("TimeForConnect:" + String(gkConfig.TimeForConnect));
  file.println("MillisForReconect:" + String(gkConfig.MillisForReconect));
  file.println("jerk_thr:" + String(gkConfig.jerk_thr));
  file.println("gzsm_thr:" + String(gkConfig.gzsm_thr));
  file.println("landing_pressure_thr:" + String(gkConfig.landing_pressure_thr));

  file.println("KeepAtEnd: DONTDELETE");
  file.close();

  writeGliderConfigNVS(); // Store this data also in NVS
  // evaluate_gkConfig();
}

// Save Wifi data on config file
void resetWiFi(AsyncWebServerRequest *request)
{
  readGliderKeeperConfig();

  strcpy(gkConfig.WiFiSSID, ssid);
  strcpy(gkConfig.WifiPass, password);

  Serial.println("Read: " + String(gkConfig.WiFiSSID) + " " + String(gkConfig.WifiPass));

  writeGliderKeeperConfig();
  String output = "Save OK";

  request->send(200, "text/html", output);
  wifiDisconnect();
  connecttowifi();
}

void RTCstatus(AsyncWebServerRequest *request)
{
  String output;
  switch (local_RTC.keeper_Synchro_status)
  {
  case NO_RTC_AVAILABLE:
    output = "NO GliderSynchro Found";
    break;
  case RTC_AVAILABLE:
    output = "GliderSynchro Operating";
    break;
  case RTC_OUT_OF_SYNC_PERIOD:
    output = "URGENT: Synchronise ASAP";
    break;
  case FAILED_NTP:
    output = "Failed I-net, try a gain";
    break;
  case LOCAL_TIME_ON_SYNC:
    output = "Only Keep is in Sync";
    break;
  case RTC_SET_FAILED:
    output = "GlyderSynchro not working";
    break;
  case RTC_ON_SYNC:
    output = "Successfull Synchor";
    break;

  default:
    break;
  }

  request->send(200, "text/html", output);
}

// Save Wifi data on config file
void saveWifi(AsyncWebServerRequest *request)
{
  readGliderKeeperConfig();

  if (request->hasParam("SSID", true))
  {
    strncpy(gkConfig.WiFiSSID, request->getParam("SSID", true)->value().c_str(), sizeof(gkConfig.WiFiSSID));
    gkConfig.WiFiSSID[sizeof(gkConfig.WiFiSSID) - 1] = 0; // Mke sure to terminate properly the string;
  }
  if (request->hasParam("WifiPass", true))
  {
    strncpy(gkConfig.WifiPass, request->getParam("WifiPass", true)->value().c_str(), sizeof(gkConfig.WifiPass));
    gkConfig.WifiPass[sizeof(gkConfig.WifiPass) - 1] = 0; // Mke sure to terminate properly the string;
  }

  if (request->hasParam("SSID1", true))
  {
    strncpy(gkConfig.WiFiSSID1, request->getParam("SSID1", true)->value().c_str(), sizeof(gkConfig.WiFiSSID1));
    gkConfig.WiFiSSID1[sizeof(gkConfig.WiFiSSID1) - 1] = 0; // Mke sure to terminate properly the string;
  }
  if (request->hasParam("WifiPass1", true))
  {
    strncpy(gkConfig.WifiPass1, request->getParam("WifiPass1", true)->value().c_str(), sizeof(gkConfig.WifiPass1));
    gkConfig.WifiPass1[sizeof(gkConfig.WifiPass1) - 1] = 0; // Mke sure to terminate properly the string;
  }

  if (request->hasParam("TimeForConnect", true))
  {
    gkConfig.TimeForConnect = request->getParam("TimeForConnect", true)->value().toInt();
  }
  if (request->hasParam("MillisForReconect", true))
  {
    gkConfig.MillisForReconect = request->getParam("MillisForReconect", true)->value().toInt();
  }

  Serial.println("Read: " + String(gkConfig.WiFiSSID) + " " + String(gkConfig.WifiPass));
  Serial.println("Read1: " + String(gkConfig.WiFiSSID1) + " " + String(gkConfig.WifiPass1));
  Serial.println("time: " + String(gkConfig.TimeForConnect) + "millis " + String(gkConfig.MillisForReconect));

  writeGliderKeeperConfig();
  String output = "<h1>New WiFi params saved</h1><br><h2>Device will restart, please reconnect</h2>";

  request->send(200, "text/html", output);
  // delay(500);
  vTaskDelay(500);
  wifiDisconnect();
  ESP.restart();
  //  connecttowifi();
}

// Save Keeper data on config file
void saveKeeperConfig(AsyncWebServerRequest *request)
{
  readGliderKeeperConfig();

  if (request->hasParam("idCompMode", true))
  {
    gkConfig.Training = request->getParam("idCompMode", true)->value().equalsIgnoreCase("Training");
    gkConfig.FAImode = request->getParam("idCompMode", true)->value().equalsIgnoreCase("FAIMode");
    gkConfig.TimeSwitch = request->getParam("idCompMode", true)->value().equalsIgnoreCase("TimeSwitch");
    gkConfig.AltitudeSwitch = request->getParam("idCompMode", true)->value().equalsIgnoreCase("AltitudeSwitch");
  }

  if (request->hasParam("idLogMode", true))
  {

    gkConfig.AlwaysOn = request->getParam("idLogMode", true)->value().equalsIgnoreCase("AlwaysOn");
    gkConfig.MotorLogging = request->getParam("idLogMode", true)->value().equalsIgnoreCase("MotorLogging");
    gkConfig.AltitudeLogging = request->getParam("idLogMode", true)->value().equalsIgnoreCase("AltitudeLogging");
  }

  if (request->hasParam("idMotorEnable", true))
  {
    gkConfig.EmergencyMotorEnable = request->getParam("idMotorEnable", true)->value().equalsIgnoreCase("EmergencyMotorEnable");
  }
  else
  {
    gkConfig.EmergencyMotorEnable = false;
  }

  if (request->hasParam("idShowFlightOLED", true))
  {
    gkConfig.ShowFlightOLED = request->getParam("idShowFlightOLED", true)->value().equalsIgnoreCase("ShowFlightOLED");
  }
  else
  {
    gkConfig.ShowFlightOLED = false;
  }

  if (request->hasParam("idAltitudeTrigger", true))
  {
    gkConfig.AltitudeTrigger = request->getParam("idAltitudeTrigger", true)->value().toInt();
  }

  if (request->hasParam("idTimeTrigger", true))
  {
    gkConfig.TimeTrigger = request->getParam("idTimeTrigger", true)->value().toInt();
  }
  if (request->hasParam("idAtitudeLoggingSetting", true))
  {

    gkConfig.AtitudeLoggingSetting = request->getParam("idAtitudeLoggingSetting", true)->value().toInt();
  }
  if (request->hasParam("jerk_thr", true))
  {

    gkConfig.jerk_thr = request->getParam("jerk_thr", true)->value().toInt();
  }
  if (request->hasParam("gzsm_thr", true))
  {

    gkConfig.gzsm_thr = request->getParam("gzsm_thr", true)->value().toInt();
  }
  if (request->hasParam("landing_pressure_thr", true))
  {

    gkConfig.landing_pressure_thr = request->getParam("landing_pressure_thr", true)->value().toInt();
  }

  if (request->hasParam("idGKOrientation", true))
  {
    gkConfig.GKOrientation = WA_FLAT;
  }
  else
  {
    gkConfig.GKOrientation = WA_VERTICAL;
  }

  writeGliderKeeperConfig();
  // Status changes displayed in Oled every 2 secs.
  //  String output = "Save OK";
  //  request->send(200, "text/html", output);
  request->redirect("/KeeperMenu.html");
}

/**
 * This function evaluate gkConfig Struct and set COMP_mode AND REC_mode
 * */

void evaluate_gkConfig()
{
  // Evaluate COMP_mode
  // COMP_mode = COMP_MODE_TRAINING; // Por si las moscas
  // REC_mode = REC_MODE_ALWAYS;

  // if (gkConfig.Training)
  // {
  //   COMP_mode = COMP_MODE_TRAINING;
  // }
  // else if (gkConfig.FAImode)
  // {
  //   COMP_mode = COMP_MODE_FAI;
  // }
  // else if (gkConfig.AltitudeSwitch)
  // {
  //   COMP_mode = COMP_MODE_ALTITUDE;
  // }
  // else if (gkConfig.TimeSwitch)
  // {
  //   COMP_mode = COMP_MODE_TIME;
  // }

  // // Set Emergency Restart
  // EmergencyMotorEnable = gkConfig.EmergencyMotorEnable;

  // // Evaluate REC_mode

  // if (gkConfig.AlwaysOn)
  // {
  //   REC_mode = REC_MODE_ALWAYS;
  // }
  // else if (gkConfig.MotorLogging)
  // {
  //   REC_mode = REC_MODE_THROTTLE;
  // }
  // else if (gkConfig.AltitudeLogging)
  // {
  //   REC_mode = REC_MODE_ALTITUDE;
  // }

  // // Set global variable for Working Axis
  // working_axis = gkConfig.GKOrientation;
}

void erase_keeperPart(AsyncWebServerRequest *request)
{
  competition.format();
  ///// aqui hay que poner algun mensaje de

  /////////////////////////////no se esto hara falta
  request->send(200, "text/plain", "Erased data!");
  // ESP.restart();
}

void getFlashStatus(AsyncWebServerRequest *request)
{
  // Serial.print("millis1");
  // Serial.println(millis());
  // char buffer[500];
  // struct flash_status *flashStatus = competition.getFlashStatus();

  // snprintf(buffer, 500, "{\"FirmwareVersion\": \"%s\", \n\"SerialID\": \"%s\", \n\"FwType\": \"%s\", \n\"totalFlashForRecords\": %d, \n\"currentFlight\": %d, \n\"numberOfRecordsUsed\": %d, \n\"maxFlightsAllowed\": %d, \n\"maxRecordsAllowed\": %d, \n\"totalBlocksUsed\": %d,\n\"totalFlashSize\": %d, \n\"recordsPerPage\": %d, \n\"flightRecordSize\": %d, \n\"maxFlightsAllowed\": %d, \n\"maxRecordsAllowed\": %d, \n\"totalBlocksUsed\": %d}\n",
  //          versionkeeper.c_str(), serialnumber.c_str(), FW_TYPE,
  //          flashStatus->totalFlashForRecords, flashStatus->currentFlight, flashStatus->numberOfRecordsUsed,
  //          flashStatus->maxFlightsAllowed, flashStatus->maxRecordsAllowed, flashStatus->totalBlocksUsed,
  //          flashStatus->totalFlashSize, flashStatus->recordsPerPage, flashStatus->flightRecordSize,
  //          flashStatus->maxFlightsAllowed, flashStatus->maxRecordsAllowed, flashStatus->totalBlocksUsed);
  // Serial.print("millis2");
  // Serial.println(millis());
  // request->send(200, "text/plain", buffer);
  // Serial.print("millis3");
  // Serial.println(millis());
}

/**
 *  Descarga la flash en binario
 * */

size_t returnFlash(uint8_t *buffer, size_t maxlen, size_t index)
{
  // struct flash_status *flashStatus = vuelosFlash.getFlashStatus();
  size_t result;
  if (index == 0)
  {
    // result = competition.getFirstFlashBlock(buffer, maxlen);
  }
  else
  {
    // result = competition.getNextFlashBlock(buffer, maxlen);
  }

  //  memcpy(buffer, bufferi, result);

  return result;
}

void downloadFlash(AsyncWebServerRequest *request)
{
  // // static uint8_t buffer[PAGESIZE*2];
  // // static size_t index = 0;

  // AsyncWebServerResponse *response = request->beginChunkedResponse("application/octet-stream", [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
  //   return returnFlash(buffer, maxLen, index);
  // });

  // response->addHeader("Access-Control-Allow-Origin", "*");
  // response->addHeader("Content-Disposition", "attachment; filename=\"FlightData.bin\"");
  // response->addHeader("Connection", "close");
  // request->send(response);
}

/*!
 * @brief This internal API is used to parse the incoming GET parameters
 * as a new
 * @param[in] the ?params of the GET request.
 * @param[out] FLTdata object updated with them
 *
 */
void receive_tfd(AsyncWebServerRequest *request)
{
  // String out = "OK";
  // server.send(200, "text/plain", out);

  // request->send(200, "text/plain", "OK");
  Serial.println("keeperengaged");
  Serial.print("FLTMODE= ");
  Serial.println(FLT_mode);

  if (true)
  // if ((FLT_mode == PATATA) || (FLT_mode == RECEIVED_FROM_TIMER))
  {

    if (request->hasArg("serialnumber"))
    {
      gettimeofday(&flight.timereceived, NULL);
      // Serial.println("exito de datos");
      FLTdata.begin();
      FLTdata.set_keeperIP(request->arg("keeperIP"));
      if (request->hasArg("FLTnum"))
      {
        FLTdata.set_FLTnum(request->arg("FLTnum"));
      }
      if (request->hasArg("fltnum"))
      {
        FLTdata.set_FLTnum(request->arg("fltnum"));
      }
      FLTdata.set_flight_mins(request->arg("flight_mins"));
      FLTdata.set_flight_secs(request->arg("flight_secs"));
      FLTdata.set_F5J_height(request->arg("F5J_height"));
      FLTdata.set_F5J_millis(request->arg("F5J_millis"));

      FLTdata.set_take_off_millis(request->arg("take_off_millis"));
      FLTdata.set_throttle_shut_millis(request->arg("throttle_shut_millis"));
      FLTdata.set_landing_millis(request->arg("landing_millis"));
      FLTdata.set_takeoff_local_secs(request->arg("takeoff_localsecs"));
      FLTdata.set_takeoff_local_microsecs(request->arg("takeoff_localmillisecs"));
      FLTdata.set_transmit_local_secs(request->arg("transmit_local_secs"));
      FLTdata.set_transmit_local_microsecs(request->arg("transmit_local_microsecs"));
      FLTdata.set_rest_millis(request->arg("rest_millis"));
      FLTdata.set_Firm_ver(request->arg("Firm_ver"));
      FLTdata.set_serialnumber(request->arg("serialnumber"));
      FLTdata.set_emergency_used(request->arg("emergency_used"));
      FLTdata.set_from_NVS(request->arg("from_NVS"));
      FLTdata.set_name_in_keeper(request->arg("name"));
      FLTdata.set_nick_in_keeper(request->arg("nick"));
      if (request->hasArg("timer_serialnumber")) /// if comes from a Timer
      {
        FLTdata.tfd.comp_no = (uint32_t)request->arg("comp_no").toInt();
        // tfd.record_num = (uint32_t)request->arg("record_num").toInt();
        FLTdata.tfd.round_no = (uint32_t)request->arg("round_no").toInt();
        FLTdata.tfd.group_no = (uint32_t)request->arg("group_no").toInt();
        FLTdata.tfd.pilot_no = (uint32_t)request->arg("pilot_no").toInt();
        FLTdata.tfd.pilot_id = GT.pilots_table[FLTdata.tfd.pilot_no].pilot_id;
        FLTdata.tfd.timer_to_keeper_sec = (uint32_t)request->arg("timer_to_keeper_sec").toInt();
        FLTdata.tfd.timer_to_keeper_usec = (uint32_t)request->arg("timer_to_keeper_usec").toInt();
        FLTdata.tfd.emergency_used = (uint32_t)request->arg("emergency_used").toInt();
        FLTdata.tfd.from_NVS = (uint32_t)request->arg("from_NVS").toInt();
        FLTdata.tfd.local_landing_timeout_sec = (uint32_t)request->arg("local_landing_timeout_sec").toInt();
        FLTdata.tfd.local_landing_timeout_usec = (uint32_t)request->arg("local_landing_timeout_usec").toInt();
        FLTdata.tfd.timedout = (uint32_t)request->arg("timedout").toInt();
        FLTdata.tfd.landing_dist = (uint32_t)request->arg("landing_dist").toInt();
        FLTdata.tfd.penalty_pts = (uint32_t)request->arg("penalty_pts").toInt();
        FLTdata.tfd.raw_score = (uint32_t)request->arg("raw_score").toInt();
        request->arg("timer_serialnumber").toCharArray(FLTdata.tfd.timer_serialnumber, 12);
        request->arg("timer_Firm_ver").toCharArray(FLTdata.tfd.timer_Firm_ver, 12);
        GT.tfd_received++;
        FLT_mode = NEW_KEEPER_TFD;
        GT.received_from_GTimer = 1;
        myudp.syncro_status = SUCCESS_SYCHRONISING; // TODO timer-tfd?? como viene de un Timer no hace falta sincronizar
        GT.save_record();
      }
      else
      {
        myudp.clear_Myudp();
        M5.Speaker.tone(NOTE_DH2, 100);
        delay(100);
        M5.Speaker.tone(NOTE_DL5, 50);
        if (!FLTdata.check_sychro_needed()) /// empieza la Sincronizacion
        {
          myudp.syncro_status = SUCCESS_SYCHRONISING;
          Serial.print("Syncro not needed");
          delay(100);
        }
        else
        {
          myudp.syncro_status = SYCHRONISING;
          Serial.print("Syncro needed");
        }
        FLT_mode = NEW_KEEPER_TFD;
      }

      // Serial.println("NEW DATA Arrived--:)------------------");
      // Serial.print("keeperIP");
      // Serial.println(keeperIP);
      FLTdata.print_tfd(FLTdata.tfd);
      Serial.println("xxxxxxxxxxxxxxx");
      packet_received = 0;
    }
    else
    {
      // Serial.println("fallo de datos");
      // out = "BAD_FORMAT";
      //  Serial.println("NEW DATA BAD FORMAT--------------------");
    }
  }
  else
  {
    // out = "not_OK";
    // Serial.println("NOT OK :(--------------------");
  }
  request->send(200, "text/plain", "OK");

  // Serial.println("Client Closed--:)------------------");
  //  FLT_mode= NEW_DATA;
  //  GD.display_fltdata(FLTdata.tfd);
}

void check_registered_pilots(AsyncWebServerRequest *request)
{
  request->send(200, "text/plain", "OK");
}

void scan_wifi()
{
  // Serial.println("Wifi disconnected  ");
  wifiDisconnect();

  delay(100);
  // Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  int nwifis = 0;
  readGliderKeeperConfig();
  ssid = gkConfig.WiFiSSID;
  password = gkConfig.WifiPass;
  ssid1 = gkConfig.WiFiSSID1;
  password1 = gkConfig.WifiPass1;
  if (n == 0)
  {
    // Serial.println("no networks found");
  }
  else
  {
    // Serial.print(n);
    // Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
    {
      delay(10);
      String ssname;

      if (WiFi.SSID(i) != "GliderKeeper") // avoid catchin "GliderKeeper" as a device
      {
        ssname = WiFi.SSID(i).substring(0, 12);
      }
      if (ssname == "GliderKeeper")
      {
        ssname = WiFi.SSID(i).substring(12);
        ssname.toCharArray(GT.GTmenu[GT.pick_pointer + nwifis].menulabel, 24);
        GT.ssid_found[nwifis] = WiFi.SSID(i);
        // Serial.print("GUARDADA EN SCAN ");
        // Serial.println(GT.GTmenu[GT.pick_pointer + nwifis].menulabel);
        nwifis++;
      }
      if (WiFi.SSID(i) == "GliderKeeper")
      {
        ssname = WiFi.SSID(i);
        ssname.toCharArray(GT.GTmenu[GT.pick_pointer + nwifis].menulabel, 24);
        GT.ssid_found[nwifis] = WiFi.SSID(i);
        // Serial.print("GUARDADA EN SCAN ");
        // Serial.println(GT.GTmenu[GT.pick_pointer + nwifis].menulabel);
        nwifis++;
      }
      if ((WiFi.SSID(i) == String(ssid)) && (String(ssid) != "GliderKeeper"))
      {
        ssname = WiFi.SSID(i);
        ssname.toCharArray(GT.GTmenu[GT.pick_pointer + nwifis].menulabel, 24);
        GT.ssid_found[nwifis] = WiFi.SSID(i);
        // Serial.print("GUARDADA EN SCAN ");
        // Serial.println(GT.GTmenu[GT.pick_pointer + nwifis].menulabel);
        nwifis++;
      }
      if ((WiFi.SSID(i) == String(ssid1)) && (String(ssid1) != "GliderKeeper"))
      {
        ssname = WiFi.SSID(i);
        ssname.toCharArray(GT.GTmenu[GT.pick_pointer + nwifis].menulabel, 24);
        GT.ssid_found[nwifis] = WiFi.SSID(i);
        // Serial.print("GUARDADA EN SCAN ");
        // Serial.println(GT.GTmenu[GT.pick_pointer + nwifis].menulabel);
        nwifis++;
      }
      if (WiFi.SSID(i) == "GliderThrow")
      {
        ssname = WiFi.SSID(i);
        ssname.toCharArray(GT.GTmenu[GT.pick_pointer + nwifis].menulabel, 24);
        GT.ssid_found[nwifis] = WiFi.SSID(i);
        // Serial.print("GUARDADA EN SCAN ");
        // Serial.println(GT.GTmenu[GT.pick_pointer + nwifis].menulabel);
        nwifis++;
      }
      if (WiFi.SSID(i) == "GliderThrow4")
      {
        ssname = WiFi.SSID(i);
        ssname.toCharArray(GT.GTmenu[GT.pick_pointer + nwifis].menulabel, 24);
        GT.ssid_found[nwifis] = WiFi.SSID(i);
        // Serial.print("GUARDADA EN SCAN ");
        // Serial.println(GT.GTmenu[GT.pick_pointer + nwifis].menulabel);
        nwifis++;
      }
      if (WiFi.SSID(i) == "GliderTimer")
      {
        ssname = WiFi.SSID(i);
        ssname.toCharArray(GT.GTmenu[GT.pick_pointer + nwifis].menulabel, 24);
        GT.ssid_found[nwifis] = WiFi.SSID(i);
        // Serial.print("GUARDADA EN SCAN ");
        // Serial.println(GT.GTmenu[GT.pick_pointer + nwifis].menulabel);
        nwifis++;
      }
      if (WiFi.SSID(i) == "GliderTmain")
      {
        ssname = WiFi.SSID(i);
        ssname.toCharArray(GT.GTmenu[GT.pick_pointer + nwifis].menulabel, 24);
        GT.ssid_found[nwifis] = WiFi.SSID(i);
        // Serial.print("GUARDADA EN SCAN ");
        // Serial.println(GT.GTmenu[GT.pick_pointer + nwifis].menulabel);
        nwifis++;
      }
      if (nwifis > 2)
      {
        break;
      }
    }
  }
  // Serial.println("");
  if (nwifis == 0)
  {
    String ssname = "...not found";
    ssname.toCharArray(GT.GTmenu[GT.pick_pointer].menulabel, 24);
    ssname = "Scan again?";
    ssname.toCharArray(GT.GTmenu[GT.pick_pointer + 3].menulabel, 24);
  }
  GD.menu_displayUpdate(GT.menupointer);
}

int connect_to_AP(int selection)
{
  int connection_result = KEEPER_STA;
  String stssid = String(GT.ssid_found[selection]);
  const char *myssid = "GliderKeeper";
  const char *mypassword = "123456789";
  if (stssid != "GliderKeeper")
  {
    if (stssid == String(ssid))
    {
      mypassword = const_cast<char *>(password);
      connection_result = HOME_STA;
    }
    if (stssid == String(ssid1))
    {
      mypassword = const_cast<char *>(password1);
      connection_result = HOME_STA;
    }
  }
  if (stssid == "GliderKeeper")
  {
    connection_result = HOME_STA;
  }
  // if (stssid == "GliderThrow") //TODO quitando Gthrow
  // {
  //   gth.throw_status = ANGLEVIEW;
  //   connection_result = THROW_STA;
  // }
  // if (stssid == "GliderThrow4")
  // {
  //   gth.throw_status = ANGLEVIEW3;
  //   connection_result = THROW4_STA;
  // }
  if (stssid == "GliderTimer")
  {
    connection_result = TIMER_STA;
  }
  if (stssid == "GliderTmain")
  {
    connection_result = MAIN_STA;
  }
  myssid = const_cast<char *>(GT.ssid_found[selection].c_str());
  short times_to_connect = 0;
  Debug.print("Connecting to ");
  Debug.print(myssid);
  Debug.print(" with pass: ");
  Debug.println(mypassword);
  GD.clean_partial_flt();
  GD.alabel = "wait for results";
  GD.partial_flt();
  WiFi.mode(WIFI_OFF); // captive tests
  btStop();
  if (String(WiFi.SSID()) != String(ssid))
  {
    yield();
    WiFi.mode(WIFI_STA);
    //////////////////////TODO prueba maligna
    /// WiFi.setHostname("GliderCulo");
    Serial.printf("connecting to %s with passwd %s", myssid, mypassword);

    WiFi.begin(myssid, mypassword);
    delay(300);
  }
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Debug.print(".");
    times_to_connect++;
    Serial.printf("connecting to %s with passwd %s trial %d  \n", myssid, mypassword, times_to_connect);
    if (times_to_connect > 15)
    {
      // ESP.restart();
      Serial.printf("FAILED STA \n");
      return FAILED_STA;
      break;
    }
  }
  // local_RTC.ConfigLocalTimeFromNTP();
  // Debug.println("Starting WebServer");
  // dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));                        /////captive
  // httpserver.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); //only when requested from AP captive
  // httpserver.begin();
  // Serial.println("Connected to AP");
  if (connection_result == HOME_STA)
  {
    local_RTC.ConfigLocalTimeFromNTP(1);
    Debug.println("Starting WebServer");
    MDNS.begin(host);
    MDNS.addService(service, proto, port);

    // dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));                        /////captive
    // httpserver.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER); // only when requested from AP captive
    httpserver.begin();
  }
  return connection_result;
}

/*!
 * @brief This internal API is used to request to a server´s (keeper, or Timer) tfd. here, Timer is client
 * it uses the trag /last_flight
 * @param[in] void
 * @param[out] payload a String object with a tfd info
 *
 */
String get_last_FLTdata() // a Timer requests a tfd to a Keeper or Timer, Keeper will return its last flight, Timer will its Launch epoch draw_data[round][group][pilot=0]
{
  Serial.println("WAIT FLAG ENABLED");
  myudp.wait_flag = 1; // por si aca pongo esta bandera miestras está pidiendo datos
  String payload;
  if ((WiFi.waitForConnectResult() == WL_CONNECTED))
  {
    HTTPClient http;
    // Serial.print("[HTTP] begin...\n");
    // http.begin(payload); //HTTP
    payload = "http://192.168.4.1/last_flight";
    Serial.println(payload);
    http.begin(payload);       // HTTP
                               //  Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET(); // start connection and send HTTP header
    if (httpCode > 0)          // httpCode will be negative on error
    {
      //  Serial.printf("[HTTP] GET... code: %d\n", httpCode); // HTTP header has been send and Server response header has been handled
      if (httpCode == HTTP_CODE_OK) // file found at server
      {

        payload = http.getString();
        Serial.println(payload);
      }
    }
    else
    {
      payload = "[HTTP] GET... failed, error:" + http.errorToString(httpCode);
      // Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      Serial.println(payload);
    }
    http.end();
  }
  myudp.wait_flag = 0;
  return payload;
}
/*!
 * @brief This internal API is used to respons to  a  (Timer) client  tfd, here Timer is serving
 * it uses the trag /last_flight
 * @param[in]  request (pointer)
 * @param[out] pload a text string as response
 *
 */
void last_flight(AsyncWebServerRequest *request)
{
  Serial.printf("millisANTES1: %d   ", millis());
  AsyncWebServerResponse *response;

  String pload = FLTdata.last_launch_payload();
  response = request->beginResponse(200, "text/plain", pload);
  // response->addHeader("Access-Control-Allow-Origin", "*");
  request->send(response);
  Serial.printf("millisDESPUES: %d last_flight sent \n", millis());
}

bool send_single_tfd()
{
  // FLTdata.print_tfd(FLTdata.tfd);
  char pload[800];
  strcpy(pload, "");
  String payload;
  if ((WiFi.waitForConnectResult() == WL_CONNECTED))
  {
    FLTdata.sprint_tfd(FLTdata.tfd, pload);
    payload = String(pload);
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    Serial.println(payload);
    http.begin(payload); // HTTP
    // Serial.println(payload);
    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET(); // start connection and send HTTP header
    if (httpCode > 0)          // httpCode will be negative on error
    {
      // Serial.println(" Aqui llegue");
      Serial.printf("[HTTP] GET... code: %d\n", httpCode); // HTTP header has been send and Server response header has been handled
      if (httpCode == HTTP_CODE_OK)                        // file found at server
      {
        // Serial.println(" Main ha recibido con exito");
        payload = http.getString();
        // strcpy(pload, http.getString().c_str());
        http.end();
      }
      else
      {
        // payload = "FAIL";
        Serial.printf("[HTTP] GET... failed, error#: %d -> %s\n", httpCode, http.errorToString(httpCode).c_str());
        http.end();
        return false;
      }

      // Green_led.led_blink(BLINK_ON);
    }
    else
    {
      return false;
      Serial.println("not Wifi");
    }
    // if (strcmp(pload, "OK") == 0)
    if ((payload == "OK") || (payload == "ScoreUpdated"))
    {
      // flight.beep();
      M5.Speaker.tone(NOTE_DH2, 100);
      Serial.print("Uploaded: ");
      Serial.println(payload);
      delay(105);
      M5.update();
      GT.tfd_sent_no++;

      return true;
    }
    else
    {
      GT.tfd_sent_no++;
      GT.tfd_fail_no++;
      flight.boop();
      Serial.print("Failed upload:");
      Serial.println(payload);
      delay(405);
      M5.update();
      return true;
    }
  }
  return 0;
}

bool send_loop_tfd()
{
  bool good_result = 1;
  GT.tfd_sent_no = 0;
  GT.tfd_fail_no = 0;
  GD.alabel = "End Upload";
  Serial.printf("max_pilot_number_in_file  -> %d \n", GT.mycompetition.head.number_of_pilots);
  Serial.printf("group_draw_data_pointer  -> %d \n", GT.group_draw_data_pointer);
  if (GT.groups_enabled)
  {
    uint32_t ref_round = GT.group_draw_data[GT.group_draw_data_pointer].round_no;
    uint32_t ref_group = GT.group_draw_data[GT.group_draw_data_pointer].group_no;
    Serial.printf("data pointer is %d ref round :  %d  ref group %d \n", GT.group_draw_data_pointer, ref_round, ref_group);
    if (ref_round == 0)
    {
      Serial.printf("ERROR no round round was  %d  group %d \n", GT.stored_round_no, GT.stored_group_no);
      return 0;
    }
    bool still_the_same = 1;
    uint32_t recordi = GT.group_draw_data_pointer;
    int sent_recs = 0;
    while (GT.group_draw_data[recordi].fltnum == 999) // in case we have some a launch in the bottom
    {
      recordi--;
      if ((recordi == 0) && (GT.stored_record_num < MAX_GROUP_DRAW_DATA))
      {
        Serial.printf("ERROR no records to send round was  %d  group %d stored_record_num %d \n", GT.stored_round_no, GT.stored_group_no, GT.stored_record_num);
        return 0;
      }
      if (recordi == 0)
      {
        recordi = MAX_GROUP_DRAW_DATA - 1;
      }

      ref_round = GT.group_draw_data[recordi].round_no;
      ref_group = GT.group_draw_data[recordi].group_no;
    }

    while (still_the_same)
    {
      if (GT.group_draw_data[recordi].fltnum == 999)
      {
        break; /// we are done with this group this is the launch record
      }
      FLTdata.begin();
      // FLTdata.print_tfd(GT.group_draw_data[recordi]);
      FLTdata.tfd = GT.group_draw_data[recordi];
      // FLTdata.print_tfd(FLTdata.tfd);
      Serial.printf("Sending groupDrawData pointer: %d Round: %d; Group: %d; Pilot ID %d", recordi, FLTdata.tfd.round_no, FLTdata.tfd.group_no, FLTdata.tfd.pilot_id);
      good_result = perform_and_report_simple_tfd();
      still_the_same = ((GT.group_draw_data[recordi].round_no == ref_round) &&
                        (GT.group_draw_data[recordi].group_no == ref_group));
      if (recordi == 1)
      {
        recordi = MAX_GROUP_DRAW_DATA;
      }
      recordi--;
      sent_recs++;
      if (sent_recs > 15)
      {
        Serial.printf("ERROR  TOO MANY records:15 \n");
        break;
      }
    }
    // for (int recordi = 1; recordi <= MAX_GROUP_DRAW_DATA; recordi++)
    // {
    //   if ((GT.group_draw_data[recordi].round_no == GT.stored_round_no) &&
    //       (GT.group_draw_data[recordi].group_no == GT.stored_group_no))
    //   {
    //     FLTdata.begin();
    //     // FLTdata.print_tfd(GT.group_draw_data[recordi]);
    //     FLTdata.tfd = GT.group_draw_data[recordi];
    //     // FLTdata.print_tfd(FLTdata.tfd);
    //     good_result = perform_and_report_simple_tfd();
    //   }
    // }
  }
  else
  {
    for (int pilotj = 1; pilotj <= GT.mycompetition.head.number_of_pilots; pilotj++)
    {
      FLTdata.begin();
      FLTdata.tfd = GT.draw_data[GT.stored_round_no][pilotj];
      if ((GT.mycompetition.draw_list[GT.stored_round_no][pilotj].group == GT.stored_group_no))
      {
        Serial.printf("\npilotJ %d, round %d, Goup %d, flt %d\n", pilotj, GT.mycompetition.draw_list[GT.stored_round_no][pilotj].round, GT.mycompetition.draw_list[GT.stored_round_no][pilotj].group, GT.draw_data[GT.stored_round_no][pilotj].fltnum);
        if (GT.draw_data[GT.stored_round_no][pilotj].fltnum > 0)
        {
          perform_and_report_simple_tfd();
        }
      }
    }
  }
  delay(200);
  Serial.printf("sent what was avail round  %d  group %d \n", GT.stored_round_no, GT.stored_group_no);
  return 1;
}

bool perform_and_report_simple_tfd()
{ // FLTdata.begin();
  // FLTdata.tfd = GT.draw_data[GT.stored_round_no][pilotj];
  bool good_result = send_single_tfd();
  // bool good_result=0;
  // String PL= get_last_FLTdata();
  GD.clean_partial_flt();
  if (!good_result)
  {
    // Serial.println("BAD RESULT SENDING: Aborting");
    GD.alabel = "FAILED! Try again";

    // GT.tfd_sent_no++;
    // GT.tfd_fail_no++;
    GD.partial_flt();

    return 0;
  }
  else
  {
    // GT.tfd_sent_no++;
    GD.alabel = "Uploaded " + String(GT.tfd_sent_no - GT.tfd_fail_no) + "/" + String(GT.tfd_sent_no) + " rcds";
    GD.partial_flt();
    return 1;
  }
  // GD.partial_flt();
  delay(200);
}
bool send_all_tfd()
{
  bool good_result = 1;
  GT.tfd_sent_no = 0;
  FLTdata.begin();
  bool found = competition.getFirstRecord(GT.mycompetition.head.contestNum, &FLTdata.tfd);
  while (found)
  {
    good_result = send_single_tfd();
    // bool good_result=0;
    // String PL= get_last_FLTdata();
    GD.clean_partial_flt();
    if (!good_result)
    {
      // Serial.println("BAD RESULT SENDING: Aborting");
      GD.alabel = "FAILED! Try again";
      GD.partial_flt();
      return 0;
    }
    else
    {
      GT.tfd_sent_no++;
      GD.alabel = "Uploaded " + String(GT.tfd_sent_no) + " rcds";
      GD.partial_flt();
    }
    // GD.partial_flt();
    delay(100);

    FLTdata.begin();
    found = competition.getNextRecord(&FLTdata.tfd);
  }

  delay(200);
  Serial.printf("sent what was avail round  %d  group %d \n", GT.stored_round_no, GT.stored_group_no);
  return 1;
}

int download_scheduled_rounds(int file_num, bool main_sta)
{
  // String payload;
  int result = 0;
  String file_name;
  uint8_t byte3 = WiFi.localIP()[2];
  if (byte3 == 4)
  // if (main_sta)  // main_sta not used
  {
    GD.label1 = "http://192.168.4.1/scheduled_rounds.csv";
  }
  else
  {
    GD.label1 = SCHED_TRAG + String(GT.mycompetition.head.external_comp_ID);
    Serial.print("Trag for sched_rounds.: ");
    Serial.println(GD.label1);
  }
  // switch (file_num)
  // {
  // case 1:
  //   GD.label1 = GD.label1 + "pilots";
  //   break;
  // case 2:
  //   GD.label1 = GD.label1 + "draw";
  //   break;

  // default:
  //   GD.label1 = "http://192.168.4.1/scheduled_rounds.csv";
  //   break;
  // }

  if ((WiFi.waitForConnectResult() == WL_CONNECTED))
  {
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    Serial.println(GD.label1);
    http.begin(GD.label1); // HTTP
    // Serial.println(GD.label1);
    Serial.print("[HTTP] GET...\n");
    int httpCode = http.GET(); // start connection and send HTTP header
    if (httpCode > 0)          // httpCode will be negative on error
    {
      // Serial.println(" Aqui llegue");
      Serial.printf("[HTTP] GET... code: %d\n", httpCode); // HTTP header has been send and Server response header has been handled
      if (httpCode == HTTP_CODE_OK)                        // file found at server
      {
        Serial.println(" Main ha recibido con exito");
        GD.label1 = http.getString();
        // strcpy(pload, http.getString().c_str());
        Serial.println(GD.label1);
        http.end();
        int failfind = GD.label1.indexOf("Failed-");
        if (failfind > -1)
        {
          file_num = 0;
        }

        result = file_num;
      }
      else
      {
        // GD.label1 = "FAIL";
        GD.label1 = "Failed-" + http.errorToString(httpCode);
        Serial.printf("[HTTP] GET... failed, error#: %d -> %s\n", httpCode, http.errorToString(httpCode).c_str());
        http.end();
        result = 0;
        file_num = 0;
      }

      // Green_led.led_blink(BLINK_ON);
    }
    else
    {
      result = 0;
      Serial.println("Failed-not Wifi");
      GD.label1 = "not wifi";
      file_num = 0;
    }

    // read the header of file
    Serial.println("ENTRADA EN file_disp");
    GD.file_disp(file_num, GD.label1);
    Serial.println("SALIDA DE file_disp");
    //    while (true)
    // {
    //   M5.update();
    //   if (buttonA)
    //   {
    //     GD.firsttime_settings = 1;
    //     GT.back_to_prev_status();
    //     buttonC = 0;
    //     break;
    //   }
    //   if ((buttonB) || (buttonC))
    //   {
    //     switch (file_num)
    //     {
    //     case 1:
    //       file_name = "Pilots_";
    //       break;
    //     case 2:
    //       file_name = "Draw_list";
    //       break;
    //     case 3:
    //       file_name = "/scheduled_rounds.csv";
    //       break;

    //     default:
    //       file_name = "Error_dwld";
    //       break;
    //     }
    //     buttonB = 0;
    //     buttonC = 0;
    //     File file = SPIFFS.open(file_name, "w+");
    //     if (!file)
    //     {
    //       Serial.println("could not open file");
    //     }
    //     else
    //     {
    //       file.print(payload);
    //       // file.print("\n");
    //       Serial.print(payload);
    //       file.close();
    //     }
    //     break;
    //   }
    //   delay(20);
    //   return 1;
    // }
  }
  return PATATA;
}
/**
 * REad the gkconfig data from NVS if they exist, if not write current gkConfig data
 * */
void readGliderConfigNVS()
{
  Serial.println("\nReading gkConfig from NVS");
  // check if we need to erase partition flight_data using nvs data
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }

  nvs_handle my_handle;
  err = nvs_open("storage", NVS_READWRITE, &my_handle);

  if (err == ESP_OK)
  {
    size_t lgkConfig;
    err = nvs_get_blob(my_handle, "gkConfig", NULL, &lgkConfig);
    if (err == ESP_OK)
    {
      if (lgkConfig == sizeof(gkConfig))
      {
        size_t lgkConfig;
        int needRewrite;

        err = nvs_get_blob(my_handle, "gkConfig", &gkConfig_nvs, &lgkConfig);
        if (checkNVSvalues()) // Check for correctness of key params
        {
          needRewrite = memcmp(&gkConfig, &gkConfig_nvs, sizeof(gkConfig)); // To chek if we need to write wak to config file

          err = nvs_get_blob(my_handle, "gkConfig", &gkConfig, &lgkConfig);
          if (needRewrite)
          {
            Serial.println("NVS config differs from config file, writing data back");
            writeGliderKeeperConfig(); // copy this data to config file
          }
        }
        else
        {
          Serial.println("NVS Corrupted");
          //  report_error(err_code_nvs); TODO ?
          err = !ESP_OK;
        }
      }
      else
      {
        Serial.printf("Read %d bytes from NVS, but %d was expected. Discarding NVS\n", lgkConfig, sizeof(gkConfig));
        err = !ESP_OK;
      }
    }

    if (err != ESP_OK)
    {
      writeGliderConfigNVS();
    }
  }

  nvs_close(my_handle);
  Serial.println("NVS Reading finished\n");
}

void writeGliderConfigNVS()
{

  Serial.println("Writing gkConfig to NVS");
  Serial.printf("Current ssid %s pass: %s\n", gkConfig.WiFiSSID, gkConfig.WifiPass);

  // check if we need to erase partition flight_data using nvs data
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }

  nvs_handle my_handle;
  err = nvs_open("storage", NVS_READWRITE, &my_handle);

  if (err == ESP_OK)
  {
    size_t lgkConfig = sizeof(gkConfig);
    err = nvs_set_blob(my_handle, "gkConfig", &gkConfig, lgkConfig);
  }
  if (err != ESP_OK)
  {
    Serial.println("Error writing gkConfig in NVS: " + String(err));
  }
  else
  {
    Serial.println("Writing gkConfig to NVS successfull");
  }

  nvs_close(my_handle);
}

// This function check key values read from NVS, if they are not correct, show error and erase NVS
boolean checkNVSvalues()
{
  boolean correct;

  correct = gkConfig_nvs.TimeForConnect >= 0 && gkConfig_nvs.TimeForConnect <= 30;
  return correct;
}

// void beginUdp()
// {
//   // clear_Myudp();

//   // tc.toCharArray(replyPacket, 50);

//   // Udp.begin(localUdpPort);
//   if (udp.listen(myudp.localUdpPort))
//   {
//     Serial.print("UDP Listening on IP: ");
//     Serial.println(WiFi.localIP());
//     udp.onPacket([](AsyncUDPPacket packet) {
//       //handleUdp(packet);
//       // Serial.print("UDP Packet Type: ");
//       // Serial.print(packet.isBroadcast() ? "Broadcast" : packet.isMulticast() ? "Multicast"
//       //                                                                        : "Unicast");
//       // Serial.print(", From: ");
//       // Serial.print(packet.remoteIP());
//       // Serial.print(":");
//       // Serial.print(packet.remotePort());
//       // Serial.print(", To: ");
//       // Serial.print(packet.localIP());
//       // Serial.print(":");
//       // Serial.print(packet.localPort());
//       // Serial.print(", Length: ");
//       // Serial.print(packet.length());
//       // Serial.print(", Data: ");
//       // Serial.write(packet.data(), packet.length());
//       // Serial.println();
//       //reply to the client
//       //packet.printf("Got %u bytes of data", packet.length());
//       // memcpy(incomingPacket, packet.data(), packet.length());
//       //extern char incoming_Pack [100];
//       extern bool packet_received;
//       extern Myudp myudp;
//       myudp.millis_in = millis();
//       //Serial.println(millis());
//       // incoming_package = packet.readString();
//       // incoming_package += ";"+String(millis());
//       memcpy(&myudp.incomingPacket, packet.data(), packet.length());
//       myudp.incomingPacket[packet.length()] = 0; //For string termination
//       packet_received = 1;
//       //myudp.handleUdp();
//       //Serial.println(myudp.incomingPacket);
//       packet.flush();
//       //sprintf(&incomingPacket, const char *format-string, argument-list);
//     });

//     Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), myudp.localUdpPort);
//   }
// }