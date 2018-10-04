# Images

## Formatting

In order to fit on a line of text, images must be 14x14. 
Images must be 24-bit little-endian BMP.
Use imagemagick to make sure images are correctly formatted and sized:

```mogrify -format FROM_FORMAT -depth 24 -endian LSB *.bmp```

e.g. to convert png to bmp:

```mogrify -format png -depth 24 -endian LSB *.bmp```
