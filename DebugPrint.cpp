#include <HardwareSerial.h>

#include "DebugPrint.h"

DebugPrint Debug;

void DebugPrint::print(char *msg)
{

#ifdef DEBUG_MODE
    Serial.print(msg);
#endif
};

void DebugPrint::println(char *msg)
{
#ifdef DEBUG_MODE
    Serial.println(msg);
#endif
};

void DebugPrint::print(String msg)
{

#ifdef DEBUG_MODE
    Serial.print(msg);
#endif
};

void DebugPrint::println(String msg)
{
#ifdef DEBUG_MODE
    Serial.println(msg);
#endif
};
