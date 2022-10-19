# esp-power-meter

Power meter based on m5stick and INA 260

This is meant to measure voltage and current for a battery/USB powered system,
where the USB power could be used to charge the battery, either over regular USB or over solar or other power solutions.

Measurements are displayed on the built-in M5Stick LCD, and printed on serial

**Hardware needed:**

* M5StickC-Plus
* INA 260 x 2

**Features:**
* Self-powered to not influence the system under test
* Blocks until a valid setup is detected
* Performs zero-point calibration on startup
* Can measure 2 pairs of voltage + current
* Real-time LCD display of measurements
* Serial output of CSV-formatted values

**Missing features:**
* Publish measurements over MQTT
* Self-host a web page with measurements
* Graphs on web page
* CSV Export from web page
