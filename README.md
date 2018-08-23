# gpstracker

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
- [ ] write & read from kml file instead of csv

### display gps track
- [ ] translate gps coords to screen pos (user's current location is screen center)
- [ ] filter current track gps coords to only points that are visible on screen
- [ ] draw circle for all visible gps points
- [ ] draw lines connecting all visible gps points