# gpstracker

## proof of concept up and running
![an image examplar](./examples/proofofconcept.jpg)

## overview

### minimum viable product:
Solar-powered GPS tracker that displays user's track, saving it to kml file on sd card.

### goal features:
- [ ] draws other kml tracks on sd card (different color than curr track)
- [ ] can scroll touchscreen by 'dragging'
- [ ] can zoom in/out
- [ ] concentric circles centered on user location denoting distances

## materials

### microcontroller
- [adalogger](https://www.adafruit.com/product/2796)
### sensors
- [GPS](https://www.adafruit.com/product/746)
### interface
- [touchscreen](https://learn.adafruit.com/adafruit-3-5-tft-featherwing/overview)
### power
- [solar lithium charger](https://www.adafruit.com/product/390)
- [solar panel](https://www.adafruit.com/product/3809)
- [battery](https://www.adafruit.com/product/2011)

## steps

### wiring
- [ ] hook up materials.

### GPS
- [ ] print current gps coord on screen

### SD

#### basic gps polling
- [ ] poll gps every 1 second & write to csv: p0,p1,p2,...
- [ ] read from SD & display most recent 20 locations

#### write kml
- [ ] write & read from [kml](./kml.md) file instead of csv

### RAM test (read large kml)
Adafruit Feather M0 Adalogger has "256KB of FLASH + 32KB of RAM". kml files often exceed 32KB. Test reading a large file like [this 715KB one](./examples/kml/costa_rica_track.kml). 

KML will be stored on the SD card. You probably want to read the file in small chunks, keeping only points whose lat, lng coords translate to pixels that are visible on the display. This may not efficiently support map scrolling, as the entire kml would need to be re-processed only to use most of the same points as before. 

Maybe another file on the SD card is a list of coords sorted such that those closest to the current GPS position are first. When a current location is obtained, all list elements increase by one position? This assumes you're moving slow enough to not jump many points, right?

Or maybe your track on the SD card remains CSV and powering off or pressing a button formats it as KML, or maybe offload this to a python script run on the user's computer; this would be easier to parameterize.

### display gps track
- [ ] translate gps coords to screen pos (user's current location is screen center)
- [ ] filter current track gps coords to only points that are visible on screen
- [ ] draw circle for all visible gps points
- [ ] draw lines connecting all visible gps points
