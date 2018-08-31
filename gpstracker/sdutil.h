#ifndef SDUtil_h
#define SDUtil_h

#include "Arduino.h"

class SDUtil
{
  public:
    static void init();
    static void remove(String file);
    static void println(String file, String dataString);
};

#endif
