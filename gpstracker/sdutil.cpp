#include "Arduino.h"
#include "sdutil.h"

// log battery usage to SD
#include <SD.h>

#define SD_CHIP 4

String SDUtil::init()
{
  // see if the card is present and can be initialized:
  if (!SD.begin(SD_CHIP)) 
    return "ERROR: SD CARD FAILED OR NOT PRESENT. ";
  
  return "SD initialized.";
}

void SDUtil::remove(String file)
{
  if (SD.remove(file))
    Serial.println("deleted existing file: " + file);
  else
    Serial.println("could not find existing file: " + file); 
}

void SDUtil::print(String file, String dataString, bool newLine)
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
    
    Serial.println("wrote to SD: " + dataString);
  }
  else
    Serial.println("ERROR: NO SD FILE");
}

