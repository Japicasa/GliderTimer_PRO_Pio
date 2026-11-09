#ifndef DebugPrint_h
#define DebugPrint_h
#define DEBUG_MODE
#include <Arduino.h>

class DebugPrint
{

public:
    void print(char *);
    void println(char *);
    void print(String);
    void println(String);
};

//extern DebugPrint Debug;
#endif
