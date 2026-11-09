/**
 * Fucnines para manejar el estado del PWM (Motor)
 * */

#ifndef NVSFunction_h
#define NVSFunction_h

#include "Arduino.h"
#include "keeper.h"

class NVSClass
{
private:


public:
  bool erase_nvs();
  bool check_updateInProgress();
  bool write_updateInProgress(uint8_t fwUpdateInProgress);
  bool delete_updateInProgress();
};

#endif
