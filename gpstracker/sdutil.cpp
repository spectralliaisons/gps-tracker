#include "Arduino.h"
#include "sdutil.h"

// Date and time functions using a DS1307 RTC connected via I2C and Wire lib
#include <Wire.h>
#include "RTClib.h"

// real-time clock
RTC_Millis rtc;

// log battery usage to SD
#include <SD.h>

#define SD_CHIP 4

#define FILENAME_LENGTH 7 // num chars not including suffix
#define MAX_DIRECTORY_NAME_LENGTH 8 // num chars not including final "/"

// for debugging, all calls to print to console are logged on SD card
#define LOG_PREFIX "LOG"

String SDUtil::_currLog = "";
String SDUtil::_currDirectory = "";

String SDUtil::init()
{
  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CHIP)) 
  {
    Serial.println("ERROR: SD CARD FAILED OR NOT PRESENT.");
    return "ERROR: SD CARD FAILED OR NOT PRESENT.";
  }

  // following line sets the RTC to the date & time this sketch was compiled
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));

  // create new directory for this run's logs
  String err = makeNewDirectory();
  if (err.length())
    return err;

  _currLog = increment(LOG_PREFIX);
  remove(_currLog);

  log("SD initialized at UTC time: " + String(rtc.now().unixtime()) + ". Created log file: " + _currLog);
  
  return "SD initialized.";
}

/**
 * Create directories for logs. First directory is stringified yearmonthday, containing directory of incrementing number.
 */
String SDUtil::makeNewDirectory()
{
  DateTime now = rtc.now();
  
  String year = String(now.year());
  String month = String(now.month());
  if (month.length() < 2)
    month = "0" + month;
  String day = String(now.day());
  if (day.length() < 2)
    day = "0" + day;

  String dir0 = year + month + day;
  String dir1 = padFilename("", MAX_DIRECTORY_NAME_LENGTH, "/");
  _currDirectory = dir0 + "/" + dir1;
  
  if (SD.mkdir(_currDirectory))
  {
    return "";
  }
  else
  {
    Serial.println("ERROR: Could not create directory for logs.");
    return "ERROR: Could not create directory for logs.";
  }
}

/**
 * Tag filename prefixes w/ incrementing number to prevent overwrite on board reset.
 */
String SDUtil::increment(String prefix)
{
  return _currDirectory + padFilename(prefix, FILENAME_LENGTH, ".TXT");
}

String SDUtil::padFilename(String prefix, int maxLength, String suffix)
{
  int n = 0;
  String checkFilename = "";
  
  do
  {
    String id = "";
    String nstr = String(++n);

    // pad prefix with zeros so filename without extension is FILENAME_LENGTH
    int padZeros = (maxLength - prefix.length()) - nstr.length();

    // (will not pad if no room in filename length for padding)
    for (int i = 0; i < padZeros; i++)
      id += "0";
    id += nstr;
    
    checkFilename = prefix + id + suffix;
  } while (SD.exists(checkFilename));

  log("SDUtil creating: " + checkFilename);

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
  String dat = String(String(rtc.now().unixtime())) + "::" + dataString;
  
  Serial.println(dat);
  
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
