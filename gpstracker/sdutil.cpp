#include "Arduino.h"
#include "sdutil.h"

// log battery usage to SD
#include <SD.h>

#define SD_CHIP 4

#define FILENAME_LENGTH 7 // num chars not including suffix

// for debugging, all calls to print to console are logged on SD card
#define LOG_PREFIX "LOG"

String SDUtil::_currLog = "";

String SDUtil::init()
{
  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CHIP)) 
    return "ERROR: SD CARD FAILED OR NOT PRESENT. ";

  _currLog = increment(LOG_PREFIX);
  remove(_currLog);

  log("SD initialized.");
  
  return "SD initialized.";
}

/**
 * Tag filename prefixes w/ incrementing number to prevent overwrite on board reset.
 */
String SDUtil::increment(String prefix)
{
  String suffix = ".TXT";

  int n = 0;
  bool unique = true;
  String checkFilename = "";
  
  do
  {
    String id = "";
    String nstr = String(++n);

    // pad prefix with zeros so filename without extension is FILENAME_LENGTH
    int padZeros = (FILENAME_LENGTH - prefix.length()) - nstr.length();

    // (will not pad if no room in filename length for padding)
    for (int i = 0; i < padZeros; i++)
      id += "0";
    id += nstr;
    
    checkFilename = prefix + id + suffix;
  } while (SD.exists(checkFilename));

  log("SDUtil creating file: " + checkFilename);

  return checkFilename;
}

void SDUtil::remove(String file)
{
  if (SD.remove(file))
    log("deleted existing file: " + file);
  else
    log("could not find existing file: " + file); 
}

/**
 * All calls to log go through here so we have logs even when not connected to USB to Arduino IDE
 */
void SDUtil::log(String dataString)
{
  Serial.println(dataString);
  
  String dat = String(millis()) + "::" + dataString;
  write(_currLog, dat, true, false);
}

/**
 * Write a line to a file
 */
void SDUtil::write(String file, String dataString, bool newLine, bool thenLog)
{
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(file, FILE_WRITE); // file name must be 10 chars!

  // if the file is available, write to it:
  if (dataFile)
  {
    if (newLine)
      dataFile.println(dataString);
    else
      dataFile.print(dataString);
    
    dataFile.close();

    if (thenLog)
    {
      log("SDUtil::write::" + String(dataFile.name()) + "::" + dataString);
    }
  }
}
