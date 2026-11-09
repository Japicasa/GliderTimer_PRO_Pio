#include <Arduino.h>
#include <Wire.h>

#include "MCP23017Display.h"
#include <Adafruit_SSD1306.h>
// extern Adafruit_SSD1306 display;

//#define DEBUG

extern byte display_mode;

// Digit segments labels:
//
//     a
//    ---
// f |   | b
//    --- g
// e |   | c
//    ---
//     d
//

// mapping between segments and registry for digits managed by MCP23017 port expander

uint8_t digits[10] = {
    B0111111, // 0
    B0000110, // 1
    B1011011, // 2
    B1001111, // 3
    B1100110, // 4
    B1101101, // 5
    B1111101, // 6
    B0000111, // 7
    B1111111, // 8
    B1101111  // 9
};

uint8_t segments[11] = {
    B0000001, // a
    B0000010, // b
    B0000100, // c
    B0001000, // d
    B0010000, // e
    B0100000, // f
    B1000000, // g
    B1010000, // r
    B1010011, // R
    B0011000, // gr
    B0011011, // GR
};

void writenum(int pos, int digit)
{
  int mcp;
  int gpio;
  int error;
  int num = 0;

  switch (pos)
  {
  case 1:
    mcp = MCP1;
    gpio = GPIOA;
    break;
  case 2:
    mcp = MCP1;
    gpio = GPIOB;
    break;
  case 3:
    mcp = MCP2;
    gpio = GPIOA;
    break;
  case 4:
    mcp = MCP2;
    gpio = GPIOB;
    break;
  }
  num = 0;
  do
  {
    byte d;

    if (digit != display_mode)
    {
      d = digits[digit] ^ display_mode;
    }
    else
    {
      d = display_mode;
    }

    Wire.beginTransmission(mcp);
    Wire.write(gpio); // Register
    Wire.write(d);    //
    error = Wire.endTransmission(true);
    if (error)
    {
      //#ifdef DEBUG
      Serial.print("MCP : 0x");
      Serial.print(mcp, HEX);
      Serial.print(" Reg : 0x");
      Serial.print(gpio, HEX);
      Serial.print(" Value :");
      Serial.print(digit);
      Serial.print(" Error :");
      Serial.println(error);
      //#endif
    }
    yield();
    num++;
  } while (error && (num < 3));
}

void writeseg(int pos, int segment)
{
  int mcp;
  int gpio;
  int error;
  int num = 0;

  switch (pos)
  {
  case 1:
    mcp = MCP1;
    gpio = GPIOA;
    break;
  case 2:
    mcp = MCP1;
    gpio = GPIOB;
    break;
  case 3:
    mcp = MCP2;
    gpio = GPIOA;
    break;
  case 4:
    mcp = MCP2;
    gpio = GPIOB;
    break;
  }
  num = 0;
  do
  {
    byte d;

    d = segments[segment] ^ display_mode;

    Wire.beginTransmission(mcp);
    Wire.write(gpio); // Register
    Wire.write(d);    //
    error = Wire.endTransmission(true);
    yield();
    num++;
  } while (error && (num < 3));
}

byte initMCP(uint8_t mcp)
{
  int error;
  int num = 0;
  int sumerror = 0;
  do
  {
    Wire.beginTransmission(mcp);
    error = Wire.endTransmission(true);
    //#ifdef DEBUG
    // Serial.print("check MCP, Error:");
    // Serial.println(error);
    // Serial.print(mcp);
    //#endif

    if (error != 0)
    {
      Serial.print("MCP");
      // display.print(mcp);
      Serial.print("Error");
      // display.println(error);
      // display.display();
      // delay(100);
    }
    yield();
    num++;
  } while (error && (num < 3));
  num = 0;
  do
  {
    Wire.beginTransmission(mcp);
    Wire.write(0x00); // IODIRA register
    Wire.write(0x00); // set all of port A to outputs
    error = Wire.endTransmission(true);
    //#ifdef DEBUG
    // Serial.print("Init MCP, Port A, Error:");
    // Serial.println(error);
    //#endif
    yield();
    num++;
  } while (error && (num < 3));
  sumerror += error;

  num = 0;
  do
  {
    Wire.beginTransmission(mcp);
    Wire.write(0x01); // IODIRB register
    Wire.write(0x00); // set all of port B to outputs
    error = Wire.endTransmission(true);
    //#ifdef DEBUG
    // Serial.print("Init MCP, PortB, Error:");
    // Serial.println(error);
    //#endif
    yield();
    num++;
  } while (error && (num < 3));
  sumerror += error;

  // clear clock digits
  num = 0;
  do
  {
    Wire.beginTransmission(mcp);
    Wire.write(GPIOA); // IODIRA register
    Wire.write(0x00);
    error = Wire.endTransmission(true);
#ifdef DEBUG
    // Serial.print("Clearing digits A Error:");
    // Serial.println(error);
#endif
    yield();
    num++;
  } while (error && (num < 3));
  sumerror += error;

  num = 0;
  do
  {
    Wire.beginTransmission(mcp);
    Wire.write(GPIOB); // IODIRA register
    Wire.write(0x00);
    error = Wire.endTransmission(true);
#ifdef DEBUG
    // Serial.print("Clearing digits B Error:");
    // Serial.println(error);
#endif
    yield();
    num++;
  } while (error && (num < 3));
  sumerror += error;
  return sumerror;
}

void displaycounter(int counter, int displayon, int showcounter)
{
  static int pos1, pos2, pos3, pos4;
  int min;
  int secs;

  if (!displayon)
  {
    pos1 = pos2 = pos3 = pos4 = display_mode;
  }
  else if (!showcounter)
  {
    //   pos1=pos2=pos3=pos4=0; //////////////////////////////////////////////////////////////////////
  }
  else
  {
    min = counter / 60;
    secs = counter % 60;
    pos1 = min / 10;
    pos2 = min % 10;
    pos3 = secs / 10;
    pos4 = secs % 10;
  }

#ifdef DEBUG
  // Serial.print("Contador: ");
  // Serial.print(pos1);
  // Serial.print(pos2);
  // Serial.print(":");
  // Serial.print(pos3);
  // Serial.print(pos4);
  // Serial.println();
#endif
  writenum(1, pos1);
  writenum(2, pos2);
  writenum(3, pos3);
  writenum(4, pos4);
}

void initMCPs()
{
  // i2c setup

  // Wire.begin();

  // setup first MCP23017 registries
  int myerror1;
  int myerror2;

  myerror1 = initMCP(MCP1);
  if (myerror1 == 0)
  {
    Serial.println("MCP1 OK");
    delay(100);
  }
  else
  {
    Serial.println("MCP1 FAILED");
  }
  myerror2 = initMCP(MCP2);
  if (myerror2 == 0)
  {
    Serial.println("MCP2 OK");
    delay(100);
  }
  else
  {
    Serial.println("MCP2 FAILED");
  }
  if ((myerror1 == 0) && (myerror2 == 0))
  {
    test_I2C_panel();
  }
}

void test_I2C_panel()
{
  Serial.println("Testing Panel");
  for (int i = 0; i <= 7; i++)
  {

    for (int j = 1; j <= 4; j++)
    {
      writeseg(j, i);
    }
    delay(500);
  }

  for (int j = 1; j <= 4; j++)
  {
    writenum(j, 8);
  }
}

void display_round_group(int round, int group, int displayon, int showcounter)
{
  static int pos1, pos2, pos3, pos4;

  if (!displayon)
  {
    pos1 = pos2 = pos3 = pos4 = display_mode;
  }
  else if (!showcounter)
  {
    //   pos1=pos2=pos3=pos4=0; //////////////////////////////////////////////////////////////////////
  }
  else
  {
    if (group < 10)
    {
      pos3 = 9;
      pos4 = group;
    }
    else
    {
      pos3 = 10;
      pos4 = group - 10;
    }
    if (round < 10)
    {
      pos1 = 7;
      pos2 = round;
    }
    else
    {
      pos1 = 8;
      pos2 = round - 10;
    }
  }

#ifdef DEBUG
  // Serial.print("Contador: ");
  // Serial.print(pos1);
  // Serial.print(pos2);
  // Serial.print(":");
  // Serial.print(pos3);
  // Serial.print(pos4);
  // Serial.println();
#endif
  writeseg(1, pos1);
  writenum(2, pos2);
  writeseg(3, pos3);
  writenum(4, pos4);
}