#include "Arduino.h"
#include "screenutil.h"
#include "pythagoras.h"
#include "sdutil.h"

// add other definitions from File > Examples > Adafruit HX8357 Library to support other boards
#define STMPE_CS 6
#define TFT_CS   9
#define TFT_DC   10
#define SD_CS    5t
#define TFT_RST -1

#define BACKLIGHT_PIN 12
#define BACKLIGHT_LEVEL_HI 255
#define BACKLIGHT_LEVEL_LO 3

#define NONE "??"

#define FEET_PER_MILE 5280

#define DEF_TEXT_SIZE 2
#define PADDING 5
#define ICON_SIZE 18
#define ZOOM_ICON_SIZE 48

#define FEET_PER_MILE 5280
#define MIN_DISPLAYED_FEET (FEET_PER_MILE / 3)

// Use hardware SPI and the above for CS/DC
Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

bool toggle = true;

ScreenUtil::ScreenUtil(String initMsg)
{
	// initialize TFT screen
  tft.begin(HX8357D);
  tft.setRotation(1);
  tft.fillScreen(BG);

  SDUtil::log("ScreenUtil::ScreenUtil: screen initialized. width: " + String(tft.width()) + " height: " + String(tft.height()));
  
  analogWrite(BACKLIGHT_PIN, BACKLIGHT_LEVEL_LO);

  _menu = new Menu();

  _batteryBottomPos = -999;
  _sdStatusBottomPos = -999;

  showMsg(initMsg);
}

/**
 * Handle touch state change.
 */
screen_command ScreenUtil::getScreenCommand()
{
  menu_command menuCmd = _menu->getMenuCommand();
  
  screen_command screenCmd = Screen_ignore;

  switch (menuCmd)
  {
    case Menu_sleep:
      setBacklightAndClearScreen(BACKLIGHT_LEVEL_LO);
      
      showMsg("Swipe right to wake/sleep.");
      
      screenCmd = Screen_drawBattery;
      break;

    case Menu_swiping:
      updateSwipeDisplay();
      break;
      
    case Menu_zoom:
    case Menu_wake:
      setBacklightAndClearScreen(BACKLIGHT_LEVEL_HI);
      
      updateZoomDisplay();

      screenCmd = Screen_drawMap;
      break;
  } 

  return screenCmd;
}

void ScreenUtil::setBacklightAndClearScreen(int level)
{
  analogWrite(BACKLIGHT_PIN, level);
  tft.fillScreen(BG);
}

void ScreenUtil::showMsg(String msg)
{  
  println(0, 0, DEF_TEXT_SIZE, HX8357_WHITE, msg, HX8357_RED);

  // make way for the message
  _window.x = 0;
  _window.y = textHeightForSize(DEF_TEXT_SIZE);
  _window.width = tft.width();
  _window.height = tft.height() - _window.y;
  _window.cx = (_window.width)/2 + _window.x;
  _window.cy = (_window.height)/2 + _window.y;

  _batteryTopPos = _window.y + PADDING;
  _batteryBottomPos = _batteryTopPos + textHeightForSize(DEF_TEXT_SIZE);

  _sdStatusTopPos = _batteryTopPos;
  _sdStatusBottomPos = _batteryBottomPos;
}

int ScreenUtil::textHeightForSize(int size)
{
  return max(ICON_SIZE, size * 7); // icon height
}

int ScreenUtil::textWidthForSize(int size)
{
  return size * 6;
}

void ScreenUtil::println(int x, int y, int size, int color, String str, int bg)
{
  // clear bg for this text, assuming no line breaks!
  int width = _window.width - _window.x + x; // str.length() * textWidthForSize(size)
  tft.fillRect(x, y, width, textHeightForSize(size), bg);
  tft.setCursor(x, y);
  tft.setTextSize(size);
  tft.setTextColor(color); 
  tft.println(str);

  SDUtil::log("ScreenUtil::println: " + str);
}

bool ScreenUtil::updateSDStatus(String filePath)
{
  File file = SD.open(filePath);
  
  if (!file)
  {
    drawBmp("ui/sderr.bmp", _window.x + _window.width - ICON_SIZE - PADDING, _sdStatusTopPos);
    return false;
  }
  else
  {
    drawBmp("ui/sdok.bmp", _window.x + _window.width - ICON_SIZE - PADDING, _sdStatusTopPos);
    return true;
  }
}

void ScreenUtil::updateBatteryDisplay(String displayCharge, bool isLow)
{
  drawBmp("ui/battery.bmp", _window.x, _batteryTopPos);
  println(_window.x + ICON_SIZE + PADDING, _batteryTopPos, DEF_TEXT_SIZE, isLow ? HX8357_RED : HX8357_GREEN, displayCharge + "%");
}

void ScreenUtil::updateSwipeDisplay()
{
  float x0, x1, pct;
  x0 = x1 = pct = 0.0;
  _menu->getTouchOnAndOffPoints(&x0, &x1, &pct);
  showMsg("swiping: " + String(pct) + "%");
}

void ScreenUtil::updateGPSText(Adafruit_GPS gps)
{
  int left = _window.x;
  int top = _sdStatusBottomPos + PADDING; // below battery text area
  int y = 0;
  int iconPadding = ICON_SIZE + PADDING;

  String lat, lng, alt, sat;

  if (gps.fix) 
  {
    lat = String(gps.latitudeDegrees, 10);
    lng = String(gps.longitudeDegrees, 10);
    alt = Pythagoras::cmToFeet(gps.altitude);
    sat = String(gps.satellites);
  }
  else 
  {
    lat = lng = alt = sat = NONE;
  }

  String currPos = String(lat) + "_" + String(lng);

  y = top + (textHeightForSize(DEF_TEXT_SIZE) + PADDING)*0;
  drawBmp("ui/globe.bmp", left, y);
  println(left + iconPadding, y, DEF_TEXT_SIZE, HX8357_YELLOW, lat + ", " + lng);

  y = top + (textHeightForSize(DEF_TEXT_SIZE) + PADDING)*1;
  drawBmp("ui/mts.bmp", left, y);
  println(left + iconPadding, y, DEF_TEXT_SIZE, HX8357_YELLOW, alt + "ft");

  y = top + (textHeightForSize(DEF_TEXT_SIZE) + PADDING)*2;
  drawBmp("ui/sat.bmp", left, y);
  println(left + iconPadding, y, DEF_TEXT_SIZE, HX8357_YELLOW, sat);
}

// DISPLAY GPS COORDS

void ScreenUtil::updateGPSDisplay(Adafruit_GPS gps, String trackFilepath, String mapFilepath)
{
  uint32_t t0 = millis();
  
  // get curr location (last on file)
  geoloc currGeoloc = findCurrGeoloc(trackFilepath);

  SDUtil::log("currGeoloc: " + Pythagoras::geolocToString(currGeoloc));

  drawDistancesFrom(currGeoloc);

  drawGPSTrack(trackFilepath, currGeoloc);
  drawGPSTrack(mapFilepath, currGeoloc);

  SDUtil::log("Updated GPS display in " + String(millis() - t0) + "ms.");

  // central icon indicates if we have gps fix or not
  char *icon = const_cast<char *>(gps.fix ? "ui/gpsfix.bmp" : "ui/gpsnofix.bmp");
  drawBmp(icon, _window.cx - ICON_SIZE/2, _window.cy - ICON_SIZE/2);

  // show zoom icons to indicate we can now receive touches
  int xIn = 0;
  int xOut = _window.width - ZOOM_ICON_SIZE;

  drawBmp("ui/zin.bmp", xIn, _window.y + _window.height - ZOOM_ICON_SIZE);
  drawBmp("ui/zout.bmp", xOut, _window.y + _window.height - ZOOM_ICON_SIZE);
}

void ScreenUtil::drawGPSTrack(String filePath, geoloc currGeoloc)
{
  uint32_t t0 = millis();
  
  File file = SD.open(filePath);
  if (!file)
    return;

  // geoloc one-back (when parsing)
  geoloc g1 = Pythagoras::invalidGeoloc();

  int numPoints = 0;
  int numPointsOnscreen = 0;

  // now go through again so we can compare all positions to curr pos for relative placement
  while (file.available()) 
  {
    String line = file.readStringUntil(' ');

    geoloc g0 = Pythagoras::stringToGeoloc(line);

    if (drawGeoloc(g0, g1, currGeoloc))
      numPointsOnscreen++;

    g1.lat = g0.lat;
    g1.lng = g0.lng;
    
    numPoints++;
  }
  file.close();

  showMsg("Rendered " + String(numPointsOnscreen) + "/" + String(numPoints) + " locations in " + String(millis() - t0) + "ms.");
}

geoloc ScreenUtil::findCurrGeoloc(String filePath)
{
  uint32_t t0 = millis();
  
  File file = SD.open(filePath);
  if (!file)
    return Pythagoras::invalidGeoloc();
  
  String lastLine;
  geoloc currGeoloc;
  
  while (file.available()) 
  {
    lastLine = file.readStringUntil(' '); 
  }
  file.close();
  
  geoloc g = Pythagoras::stringToGeoloc(lastLine);

  SDUtil::log("found current geoloc in " + String(millis() - t0) + "ms.");

  return g;
}

float ScreenUtil::feetToPixels(float feet)
{
  // _window.width                       px
  // -------------                     = -------------
  // _menu->currMaxDisplayableFeet()     feet
  //
  // => px * _menu->currMaxDisplayableFeet() = feet * _window.width
  // => px = (feet * _window.width) / _menu->currMaxDisplayableFeet()
  return (feet * _window.width) / _menu->currMaxDisplayableFeet();
}

float ScreenUtil::pixelsToFeet(float px)
{
  // _window.width                       px
  // -------------                     = -------------
  // _menu->currMaxDisplayableFeet()     feet
  //
  // => feet * _window.width = px * _menu->currMaxDisplayableFeet()
  // => feet = (px * _menu->currMaxDisplayableFeet()) / _window.width
  return (px * _menu->currMaxDisplayableFeet()) / _window.width;
}

bool ScreenUtil::displayFeet()
{
  return _menu->currMaxDisplayableFeet() < MIN_DISPLAYED_FEET;
}

void ScreenUtil::updateZoomDisplay()
{
  if (displayFeet())
    showMsg("ZOOM: " + String(pixelsToFeet(_window.width)) + " feet");
  else
    showMsg("ZOOM: " + String(pixelsToFeet(_window.width)/FEET_PER_MILE) + " miles");
}

/**
 * Concentric circles around curr loc to show distances from where you are
 */
void ScreenUtil::drawDistancesFrom(geoloc currGeoloc)
{
  float numRings = 6;
  float feetPerRing = (_menu->currMaxDisplayableFeet() / 2) / numRings;
  
  // octants
  int numRays = 32;
  float angle = 2 * PI / numRays; // number of dividing lines in 180-deg
  int lineLength = _window.width;
  for (int i = 0; i < numRays; i++)
  {
    // find this rotation on the unit circle
    float angle1 = i * angle;
    point vec0 = {1, 0}; 
    // (1, 0) on the unit circle is a horizontal line
    point vec1 = Pythagoras::rotate(vec0, angle1); // rotate to a new point on the unit circle

    // transform the point into window coordinates
    point p1;
    p1.x = _window.cx + vec1.x * lineLength;
    p1.y = _window.cy + vec1.y * lineLength;

    // now find corresponding 180-deg rotated point
    float angle2 = i * angle + PI;
    point vec2 = Pythagoras::rotate(vec0, angle2);
    point p2;
    p2.x = _window.cx + vec2.x * lineLength;
    p2.y = _window.cy + vec2.y * lineLength;
    
    tft.drawLine(p1.x, p1.y, p2.x, p2.y, HX8357_WHITE);
  }

  // clear center
  tft.fillCircle(_window.cx, _window.cy, feetToPixels(feetPerRing)-1, BG); 

  // draw concentric circles indicating distance from center
  for (int i = 1; i < numRings; i++)
  {
    float feet = i * feetPerRing;
    float r = feetToPixels(feet);
    tft.drawCircle(_window.cx, _window.cy, r, HX8357_WHITE);

    // legend
    if (i % 2 == 1)
    {
      // circles with legend are double-width
      tft.drawCircle(_window.cx, _window.cy, r+1, BG);
      tft.drawCircle(_window.cx, _window.cy, r+2, BG);
      tft.drawCircle(_window.cx, _window.cy, r+3, HX8357_WHITE);

      tft.setCursor(_window.cx + feetToPixels(feet) + PADDING, _window.cy - 10);
      tft.setTextSize(1);
      tft.setTextColor(HX8357_YELLOW); 

      if (displayFeet())
        tft.println("<-" + String(round(feet)) + "f"); 
      else
      {
        float mls = feet / FEET_PER_MILE;
        tft.println("<-" + String(mls) + "m");
      }
    }
  }
}

bool ScreenUtil::drawGeoloc(geoloc g0, geoloc g1, geoloc currGeoloc)
{
//    String latStr = String(pos.lat, GpsUtil::precision());
//    String lngStr = String(pos.lng, GpsUtil::precision());
//    String posStr = "(" + latStr + ", " + lngStr + ")";
//    SDUtil::log("READ GPS POSITION FROM DISC: " + posStr);
    
    // only draw visible locations (gps coordinates that translate onto visible screen coords)
    point p0 = geolocToPoint(g0, currGeoloc);

    if (!pointIsOnscreen(p0))
      return false;
    else
      tft.fillCircle(p0.x, p0.y, 3, HX8357_YELLOW);

    // assume we already drew a dot at this location because it was the last one we parsed!
    point p1 = geolocToPoint(g1, currGeoloc);

    if (pointIsOnscreen(p1))
      tft.drawLine(p1.x, p1.y, p0.x, p0.y, HX8357_YELLOW); 

    return true;
}

bool ScreenUtil::pointIsOnscreen(point p)
{
  return (p.x >= _window.x && p.x <= _window.x + _window.width) && (p.y >= _window.y && p.y <= _window.y + _window.height);
}

/**
 * Translate geolocation lng, lat to x, y s.t. the geolocation centerPos is at the center of the screen
 */
point ScreenUtil::geolocToPoint(geoloc pos, geoloc centerPos)
{
  // distance between these geopositions
  float ft = Pythagoras::getFeetBetweenGeolocs(pos, centerPos);
  float d = feetToPixels(ft);

  // angle between these
  float r = atan2(pos.lat - centerPos.lat, pos.lng - centerPos.lng);

  // we need to rotate 90deg ccw
  r += PI/2;

//  SDUtil::log("positionToPoint ft " + String(ft) + " d " + String(d) + " r " + String(r) + " width " + String(_window.width) + " height " + String(_window.height) + " cx " + String(_window.cx) + " cy " + String(_window.cy));
  
  point p;
  p.x = _window.cx + d * sin(r);
  p.y = _window.cy + d * cos(r);
  
  return p;
}

geoloc ScreenUtil::pointToGeoloc(point pt)
{
    geoloc g = Pythagoras::invalidGeoloc();
    
    return g;
}

/** BITMAP DRAWING **/

// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 20

void ScreenUtil::drawBmp(char *filename, uint16_t x, uint16_t y) 
{
  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();

  if((x >= tft.width()) || (y >= tft.height()))
  {
    SDUtil::log("ScreenUtil::drawBmp:" + String(filename) + ":ERROR: Image position exceeds screen bounds.");
    return;
  }

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    SDUtil::log("ScreenUtil::drawBmp:" + String(filename) + ":ERROR: File not found.");
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    read32(bmpFile); // print this if you want to know the file size
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    // Read DIB header
    read32(bmpFile); // print this if you want to know header size
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.startWrite(); // Start TFT transaction
        tft.setAddrWindow(x, y, w, h);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            tft.endWrite(); // End TFT transaction
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
            tft.startWrite(); // Start new TFT transaction
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              tft.endWrite(); // End TFT transaction
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
              tft.startWrite(); // Start new TFT transaction
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            tft.pushColor(tft.color565(r,g,b));
          } // end pixel
        } // end scanline
        tft.endWrite(); // End last TFT transaction
      } // end goodBmp
    }
  }

  bmpFile.close();
  
  if (!goodBmp)
    SDUtil::log("ScreenUtil::drawBmp:" + String(filename) + ":ERROR: BMP format not recognized.");
  else
    SDUtil::log("ScreenUtil::drawBmp:" + String(filename) + ":OK: loaded in " + String(millis() - startTime) + "ms");
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t ScreenUtil::read16(File &f) 
{
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t ScreenUtil::read32(File &f) 
{
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}
