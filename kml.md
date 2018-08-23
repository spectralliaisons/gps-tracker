# KML

Kml is xml, so we can parse this in C++ using [tinyxml2](https://github.com/leethomason/tinyxml2). [Here](https://developers.google.com/kml/documentation/kml_tut) is a kml primer.

Example kml file:
```
<?xml version="1.0" encoding="UTF-8"?>
<kml xmlns="http://www.opengis.net/kml/2.2" xmlns:gx="http://www.google.com/kml/ext/2.2" xmlns:kml="http://www.opengis.net/kml/2.2" xmlns:atom="http://www.w3.org/2005/Atom">
<Document>
	<name>current.kml</name>
	<Style id="path0Style">
		<LineStyle>
			<color>FF8200FF</color>
			<width>3</width>
		</LineStyle>
	</Style>
	<Placemark>
		<name>Current Track</name>
		<styleUrl>#path0Style</styleUrl>
		<MultiGeometry>
			<LineString>
				<coordinates>
					-84.9989524763,9.702389957399999,0 -84.998823395,9.702493390100001,0 -84.9987180345,9.7025454417,0 -84.9986613728,9.7025708389,0 -84.9985845946,9.702595565499999,0 -84.9984419346,9.702629596,0 -84.9983187206,9.7027237248,0 -84.9982884619,9.7027394827,0 -84.9980868772,9.702789187400001,0 -84.99789736229999,9.7028574999,0 -84.9978375155,9.702875604899999,0 
				</coordinates>
			</LineString>
		</MultiGeometry>
	</Placemark>
</Document>
</kml>
```

Looks like each location is separated by a space (" "), and each location consists of three comma-separated values: lat, lng, something else.
