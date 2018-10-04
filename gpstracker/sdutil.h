#ifndef SDUtil_h
#define SDUtil_h

#include "Arduino.h"

class SDUtil
{
  public:
    static String init();
    static String increment(String prefix);
    static void remove(String file);
    static void write(String file, String dataString, bool newLine, bool thenLog);
    static void log(String dataString);
   private:
    static String _currDirectory;
   	static String _currLog;
    static String makeNewDirectory();
    static String padFilename(String prefix, int maxLength, String suffix);
};

#endif
