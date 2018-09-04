#include "Arduino.h"
#include "sdutil.h"

// log battery usage to SD
#include <SD.h>

#define SD_CHIP 4

void SDUtil::init()
{
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CHIP)) {
    Serial.println("Card failed, or not present!");
    // don't do anything more:
    return;
  }
  
  Serial.println("card initialized!");
}

void SDUtil::remove(String file)
{
  if (SD.remove(file))
    Serial.println("deleted existing file: " + file);
  else
    Serial.println("could not find existing file: " + file); 
}

void SDUtil::println(String file, String dataString)
{
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  File dataFile = SD.open(file, FILE_WRITE); // file name must be 10 chars!

  // if the file is available, write to it:
  if (dataFile)
  {
    dataFile.println(dataString);
    dataFile.close();
    Serial.println("wrote to SD: " + dataString);
  }
  else
    Serial.println("ERROR: NO SD FILE");
}