

#include "Gthrow.h"

extern GTdisplay GD;

void Gthrow::button_update()
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

void Gthrow::begin(int throw_type)
{

  if (throw_type == 4)
  {
    trag = "http://192.168.5.1/alfaRArq";
    ssid = ssid4;
    throw_status = ANGLEVIEW3;
  }
  else
  {
    trag = "http://192.168.4.1/alfaRArq";
    ssid = ssid1;
    throw_status = ANGLEVIEW;
  }
  WiFi.disconnect();
  delay(100);
  WiFi.mode(WIFI_OFF); //captive tests
  delay(100);
  btStop();
  delay(100);
  static short times_to_connect = 0; 
  if (String(WiFi.SSID()) != String(ssid))
  {
    delay(100);
    yield();
    WiFi.mode(WIFI_STA);
    delay(200);
    WiFi.begin(ssid, password);
    delay(300);
  }
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(600);
    Serial.print(".");
    times_to_connect++;
    if (times_to_connect > 30)
    {
      break;
    }
  }
  times_to_connect = 0;
  // Serial.println("");
  // Serial.print("Connected! IP address: ");
  // Serial.println(WiFi.localIP().toString());
  MDNS.begin(host);
  // Serial.print("Open http://");
  // Serial.print(host);
  last_throw_status = 100;
  // throw_status = ANGLEVIEW;
}
void Gthrow::client_request()
{
  bool needs_payload = 1;
  switch (throw_status)
  {
  case ANGLEVIEW:
  {
    trag = "http://192.168.4.1/alfaRArq";
  }
  break;
  case THROWVIEW:
  {
    trag = "http://192.168.4.1/strokeRArq";
  }
  break;
  case CHORD_SET_1:
  {

    trag = "http://192.168.4.1/getThrows";
    // 25:25
    //("/savearmvalues", savearmvalues);
  }
  break;
  case CHORD_SET_2:
  {
    trag = "http://192.168.4.1/savearmvalues?armRB=27&armRA=25\n";
  }
  case CHORD_SET_3:
  {
    trag = "http://192.168.5.1/getThrows";
   
  }
  break;
  case CHORD_SET_4:
  {
    trag = "http://192.168.5.1/savearmvalues";
    trag = trag + String(chord_3) + "&armRB=" + String(chord_4);
    needs_payload = 0;
   
  }
  break;

  case ANGLE_RESET:
  {
    if (last_throw_status == ANGLEVIEW)
    {
      trag = "http://192.168.4.1/datum";
    }
    else
    {
      trag = "http://192.168.5.1/datum";
    }
    needs_payload = 0;
  }
  break;
  case QUAD_ANGLE_RESET:
  {
    trag = "http://192.168.4.1/strokeRArq";
  }
  break;
  case ANGLEVIEW3:
  {
    trag = "http://192.168.5.1/alfaRArq";
  }
  break;
  case THROWVIEW3:
  {
    trag = "http://192.168.5.1/strokeRArq";
  }
  break;

  case ANGLEVIEW4:
  {
    trag = "http://192.168.5.1/get_slave_angles";
  }
  break;
  case THROWVIEW4:
  {
    trag = "http://192.168.5.1/get_slave_throws";
  }
  break;
  }
  trag = trag + "";

  if ((WiFi.waitForConnectResult() == WL_CONNECTED))
  {
    // Library changed: packages\esp32\hardware\esp32\1.0.4\libraries\HTTPClient\src. after: HttpClient crashes after HTTP request #3659
    HTTPClient http;
    // Serial.print("[HTTP] begin...\n");
    
    http.begin(trag); //HTTP
    // Serial.print("[HTTP] GET...\n");
    // Serial.print(trag);
    int httpCode = http.GET(); // start connection and send HTTP header
    if (httpCode > 0)          // httpCode will be negative on error
    {
      // Serial.printf("[HTTP] GET... code: %d\n", httpCode); // HTTP header has been send and Server response header has been handled
      if (httpCode == HTTP_CODE_OK)                        // file found at server
      {
        if (needs_payload)
        {
          payload = http.getString();
          // Serial.println(payload);
        }
      }
      else
      {
        payload = "error, unknown response";
      }
    }
    else
    {
      payload = "error,[HTTP] GET... failed, error:" + http.errorToString(httpCode);
      // Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      // Serial.println(payload);
    }
    trag = "";
    http.end();
  }
  delay(10);
}

void Gthrow::tests_trag(String trag, bool needs_payload)
{
  //trag = "http://192.168.4.1/savearmvalues?armRB=27&armRA=25\n";
  trag = trag + "savearmvalues?armRA=";
  trag = trag + String(GD.label1.toInt()) + "&armRB=" + String(GD.label2.toInt());
  HTTPClient http;
  http.begin(trag); //HTTP
  // Serial.print("[HTTP] GET...\n");
  // Serial.print(trag);
  int httpCode = http.GET(); // start connection and send HTTP header
  if (httpCode > 0)          // httpCode will be negative on error
  {
    // Serial.printf("[HTTP] GET... code: %d\n", httpCode); // HTTP header has been send and Server response header has been handled
    if (httpCode == HTTP_CODE_OK)                        // file found at server
    {
      if (needs_payload)
      {
        payload = http.getString();
        // Serial.println(payload);
      }
    }
    else
    {
      payload = "error, unknown response";
    }
  }
  else
  {
    payload = "error,[HTTP] GET... failed, error:" + http.errorToString(httpCode);
    // Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    // Serial.println(payload);
  }
  http.end();
}

void Gthrow::toggle_trag(String trag, bool needs_payload)
{
  HTTPClient http;
  http.begin(trag); //HTTP
  // Serial.print("[HTTP] GET...\n");
  // Serial.print(trag);
  int httpCode = http.GET(); // start connection and send HTTP header
  if (httpCode > 0)          // httpCode will be negative on error
  {
    // Serial.printf("[HTTP] GET... code: %d\n", httpCode); // HTTP header has been send and Server response header has been handled
    if (httpCode == HTTP_CODE_OK)                        // file found at server
    {
      if (needs_payload)
      {
        payload = http.getString();
        // Serial.println(payload);
      }
    }
    else
    {
      payload = "error, unknown response";
    }
  }
  else
  {
    payload = "error,[HTTP] GET... failed, error:" + http.errorToString(httpCode);
    // Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    // Serial.println(payload);
  }
  http.end();
}

void Gthrow::manage_throw()
{
  switch (throw_status)
  {
  case ANGLEVIEW:
  {
    client_request();
    split_payload();
    GD.displayThrow();

    if (buttonA)
    {
      buttonA = 0;      
    }
    if (buttonB)
    {
      throw_status = THROWVIEW;
      buttonB = 0;
    }
    if (buttonC)
    {
      throw_status = ANGLE_RESET;
      buttonC = 0;
    }
    //button_pressed=0;
    if (button_pressed)
    {

      button_pressed = 0;
    }
  }
  break;
  case THROWVIEW:
  {
    client_request();
    split_payload();
    GD.displayThrow();
    if (buttonA)
    {
      throw_status = CHORD_SET_1;
      buttonA = 0;
    }
    if (buttonB)
    {
      throw_status = ANGLEVIEW;
      buttonB = 0;
    }
    if (buttonC)
    {
      throw_status = ANGLE_RESET;
      buttonC = 0;
    }
    //button_pressed=0;
    if (button_pressed)
    {

      button_pressed = 0;
    }
  }
  break;
  case CHORD_SET_1:
  {
    ///access only first time
    chord_button_and_display(); //and request
  }
  break;
  case CHORD_SET_2:
  {
    chord_button_and_display();
  }
  break;
  case CHORD_SET_3:
  {
    ///access only first time
    chord_button_and_display();
  }
  break;
  case CHORD_SET_4:
  {
    chord_button_and_display();
  }
  break;
  case ANGLE_RESET:
  {
    client_request();
    //throw_status = ANGLEVIEW;
    throw_status = last_throw_status;
  }
  break;
  case ANGLEVIEW3:
  {
    client_request();
    split_payload();
    GD.displayThrow();

    if (buttonA)
    {
      buttonA = 0;

      throw_status = ANGLEVIEW4;
      toggle_trag("http://192.168.5.1/quadview", 1);
    }
    if (buttonB)
    {
      throw_status = THROWVIEW3;
      buttonB = 0;
    }
    if (buttonC)
    {
      throw_status = ANGLE_RESET;
      buttonC = 0;
    }
    //button_pressed=0;
    if (button_pressed)
    {

      button_pressed = 0;
    }
  }
  break;
  case ANGLEVIEW4:
  {
    client_request();
    split_payload();
    GD.displayThrow();

    if (buttonA)
    {
      buttonA = 0;
      throw_status = ANGLEVIEW3;
      toggle_trag("http://192.168.5.1/angleview", 1);
    }
    if (buttonB)
    {
      throw_status = THROWVIEW4;
      buttonB = 0;
      toggle_trag("http://192.168.5.1/quadthrow", 1);
    }
    if (buttonC)
    {
      throw_status = ANGLE_RESET;
      buttonC = 0;
    }
    //button_pressed=0;
    if (button_pressed)
    {

      button_pressed = 0;
    }
  }
  break;
  case THROWVIEW3:
  {
    client_request();
    split_payload();
    GD.displayThrow();
    if (buttonA)
    {
      throw_status = CHORD_SET_3;
      buttonA = 0;
    }
    if (buttonB)
    {
      throw_status = ANGLEVIEW3;

      buttonB = 0;
    }
    if (buttonC)
    {
      throw_status = ANGLE_RESET;
      buttonC = 0;
    }
    //button_pressed=0;
    if (button_pressed)
    {

      button_pressed = 0;
    }
  }
  break;
  case THROWVIEW4:
  {
    client_request();
    split_payload();
    GD.displayThrow();
    if (buttonA)
    {
      throw_status = CHORD_SET_3;
      buttonA = 0;
    }
    if (buttonB)
    {
      throw_status = ANGLEVIEW4;
      buttonB = 0;
      toggle_trag("http://192.168.5.1/quadview", 1);
    }
    if (buttonC)
    {
      throw_status = ANGLE_RESET;
      buttonC = 0;
    }
    //button_pressed=0;
    if (button_pressed)
    {

      button_pressed = 0;
    }
  }
  break;
  }
}
void Gthrow::simplesplit()
{
  //Sensor 1= -0.8 deg:Sensor 2=99.0 deg:Differential= 99.8 deg //sample
  int index = payload.indexOf(":");
  int k;
  if (payload.substring(index - 2, index - 1) == "m")
  {
    k = 3;
  }
  else
  {
    k = 4;
  }
  angle_1 = payload.substring(9, index - k).toFloat();
  payload = payload.substring(index + 1);
  index = payload.indexOf(":");
  angle_2 = payload.substring(9, index - k).toFloat();
  if (k == 3)
  {
    diff2_1 = angle_2 - angle_1;
  }
  else
  {
    diff2_1 = payload.substring(index + 15, (payload.length() - k + 1)).toFloat();
  }
  GD.label1 = padea5(angle_1);
  GD.label2 = padea5(angle_2);
  GD.label3 = padea5(diff2_1);
}

void Gthrow::longsplit()
{
  //    Sensor 1= 0.1 deg:Sensor 2=-0.1 deg:Differential= -0.1 deg:Sensor 3= 0.2 deg:Sensor 4= 0.4 deg:Differential= 0.2 deg
  // Sensor 1=0.0 mm:Sensor 2=-0.0 mm:Differential= 0.0 mm:Sensor 3= 0.1 mm:Sensor 4= 0.2 mm:Differential= 0.1 mm

  int k = 3;
  int index = payload.indexOf(":");
  if (payload.substring(index - 1, index - 1) == "m")
  {
    k = 2;
  }
  else
  {
    k = 3;
  }
  angle_1 = payload.substring(9, index - k).toFloat();
  payload = payload.substring(index + 1);
  index = payload.indexOf(":");
  angle_2 = payload.substring(9, index - k).toFloat();
  payload = payload.substring(index + 1);
  index = payload.indexOf(":");
  diff2_1 = payload.substring(15, index - k).toFloat();
  payload = payload.substring(index + 1);
  index = payload.indexOf(":");
  angle_3 = payload.substring(9, index - k).toFloat();
  payload = payload.substring(index + 1);
  index = payload.indexOf(":");
  angle_4 = payload.substring(9, index - k).toFloat();
  payload = payload.substring(index + 1);
  diff4_3 = payload.substring(14, payload.length() - k + 1).toFloat();
  GD.label1 = padea5(angle_1);
  GD.label2 = padea5(angle_2);
  GD.label3 = padea5(diff2_1);
  GD.label4 = padea5(angle_3);
  GD.label5 = padea5(angle_4);
  GD.label6 = padea5(diff4_3);
}
void Gthrow::split_payload()
{
  if ((payload.substring(0, 5) == "error") || (payload.length() < 3))
  {

    GD.label1 = "***.*";
    GD.label2 = "***.*";
    GD.label3 = "***.*";
    GD.label4 = "***.*";
    GD.label5 = "***.*";
    GD.label6 = "***.*";
  }
  else
  {
    switch (throw_status)
    {
    case ANGLEVIEW:
    {
      simplesplit();
    }
    break;
    case ANGLEVIEW3:
    {
      simplesplit();
    }
    break;
    case ANGLEVIEW4:
    {
      longsplit();
    }
    break;
    case THROWVIEW:
    {
      simplesplit();
    }
    break;
    case THROWVIEW3:
    {
      simplesplit();
    }
    break;
    case THROWVIEW4:
    {
      longsplit();
    }
    break;
    case CHORD_SET_1:
    {
      int index = payload.indexOf(":");
      chord_1 = payload.substring(0, index).toInt();
      chord_2 = payload.substring(index + 1).toInt();

      GD.label1 = padea3(chord_1);
      GD.label2 = padea3(chord_2);
    }
    case CHORD_SET_3:
    {
      int index = payload.indexOf(":");
      chord_3 = payload.substring(0, index).toInt();
      chord_4 = payload.substring(index + 1).toInt();

      GD.label1 = padea3(chord_3);
      GD.label2 = padea3(chord_4);
    }
    break;
    }
  }
}

void Gthrow::next_chord()
{
  switch (throw_status)
  {
  case CHORD_SET_1:
    throw_status = CHORD_SET_2;
    break;
  case CHORD_SET_2:
    tests_trag("http://192.168.4.1/", 0);
    //client_request();
    throw_status = THROWVIEW;
    break;
  case CHORD_SET_3:
    throw_status = CHORD_SET_4;
    break;
  case CHORD_SET_4:
    tests_trag("http://192.168.5.1/", 0);
    throw_status = THROWVIEW3;
    break;
  }
}
void Gthrow::chord_increase()
{
  switch (throw_status)
  {
  case CHORD_SET_1:
    chord_1++;
    if (chord_1 > 300)
      chord_1 = 300;
    GD.label1 = padea3(chord_1);

    break;
  case CHORD_SET_2:
    chord_2++;
    if (chord_2 > 300)
      chord_2 = 300;
    GD.label2 = padea3(chord_2);
    break;
  case CHORD_SET_3:
    chord_3++;
    if (chord_3 > 300)
      chord_3 = 300;
    GD.label1 = padea3(chord_3);
    break;
  case CHORD_SET_4:
    chord_4++;
    if (chord_4 > 300)
      chord_4 = 300;
    GD.label2 = padea3(chord_4);
    break;
  }
}
void Gthrow::chord_decrease()
{
  switch (throw_status)
  {
  case CHORD_SET_1:
    chord_1 = chord_1 - 1;
    if (chord_1 < 1)
      chord_1 = 1;
    GD.label1 = padea3(chord_1);
    break;
  case CHORD_SET_2:
    chord_2 = chord_2 - 1;
    if (chord_2 < 1)
      chord_2 = 1;
    GD.label2 = padea3(chord_2);
    break;
  case CHORD_SET_3:
    chord_3 = chord_3 - 1;
    if (chord_3 < 1)
      chord_3 = 1;
    GD.label1 = padea3(chord_3);
    break;
  case CHORD_SET_4:
    chord_4 = chord_4 - 1;
    if (chord_4 < 1)
      chord_4 = 1;
    GD.label2 = padea3(chord_4);
    break;
  }
}

String Gthrow::padea5(float num)
{
  TC = String(num, 1);
  if (TC.length() > 5)
  {
    TC = String(int(num));
  }
  else
  {
    if (TC.length() < 3)
    {
      TC = " " + TC;
    }
    if (TC.length() < 4)
    {
      TC = " " + TC;
    }
    if (TC.length() < 5)
    {
      TC = " " + TC;
    }
  }
  return TC;
}

String Gthrow::padea3(int num)
{
  // TC = String(num);

  // if (TC.length() < 2)
  // {
  //   TC = " " + TC;
  // }
  // if (TC.length() < 3)
  // {
  //   TC = " " + TC;
  // }
  char tc[7];
  sprintf(tc,"%3d", num);
  return String(tc);
}

void Gthrow::chord_button_and_display()
{
  GD.displayThrow();
  if (buttonA)
  {
    chord_decrease();
    buttonA = 0;
  }
  if (buttonB)
  {
    chord_increase();
    buttonB = 0;
  }
  if (buttonC)
  {
    next_chord();
    buttonC = 0;
  }
  //button_pressed=0;
  if (button_pressed)
  {

    button_pressed = 0;
  }
}