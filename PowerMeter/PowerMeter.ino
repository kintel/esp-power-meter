#include <M5StickCPlus.h>
#include <Adafruit_INA260.h>

Adafruit_INA260 ina260_batt = Adafruit_INA260();
Adafruit_INA260 ina260_usb = Adafruit_INA260();

unsigned long lastMillis = 0;
bool displayOn = true;
float usb_offset_mA = 0;
float batt_offset_mA = 0;

void setup()
{
  M5.begin();
  Serial.println("Hello M5 INA260");

  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE, BLACK);
  M5.Lcd.setTextSize(3);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.printf("M5 + INA260\n");
  M5.Lcd.setTextSize(2);
  
  M5.Lcd.setCursor(10,  M5.Lcd.getCursorY());
  M5.Lcd.printf("I2C USB: ");
  int bx = M5.Lcd.getCursorX();
  int by = M5.Lcd.getCursorY();
  M5.Lcd.printf("\n");
  M5.Lcd.setCursor(10,  M5.Lcd.getCursorY());
  M5.Lcd.printf("I2C Bat: ");
  int ux = M5.Lcd.getCursorX();
  int uy = M5.Lcd.getCursorY();

  // Block until we find both INA 260 devices
  bool found_ina260_batt = false;
  bool found_ina260_usb = false;
  bool calibrated_ina260_batt = false;
  bool calibrated_ina260_usb = false;
  while (!calibrated_ina260_batt || !calibrated_ina260_usb) {
    if (!found_ina260_batt) {
      Serial.print("INA260 batt: ");
      found_ina260_batt = ina260_batt.begin(0x41);
      if (found_ina260_batt) {
        ina260_batt.setCurrentConversionTime(INA260_TIME_8_244_ms);
        ina260_batt.setVoltageConversionTime(INA260_TIME_140_us);
        ina260_batt.setAveragingCount(INA260_COUNT_64);
      }
      Serial.println(found_ina260_batt ? "Cal " : "Search");
      M5.Lcd.setCursor(bx, by);
      M5.Lcd.printf(found_ina260_batt ? "Cal " : "Search");
    }

    // Zero-point calibration
    if (!calibrated_ina260_batt) {
      while (!ina260_batt.conversionReady()) {}
      batt_offset_mA = ina260_batt.readCurrent();
      // If we're reading more than 4 mA, it looks like we have a device connected, so we don't calibrate
      calibrated_ina260_batt = abs(batt_offset_mA) < 4;
      M5.Lcd.setCursor(bx, by);
      M5.Lcd.printf(calibrated_ina260_batt ? "OK     " : "Cal Err");
      Serial.printf("Batt offset: %.2f mA\n", batt_offset_mA);
    }

    if (!found_ina260_usb) {
      Serial.print("INA260 USB: ");
      found_ina260_usb = ina260_usb.begin();
      if (found_ina260_usb) {
        ina260_usb.setCurrentConversionTime(INA260_TIME_8_244_ms);
        ina260_usb.setVoltageConversionTime(INA260_TIME_140_us);
        ina260_usb.setAveragingCount(INA260_COUNT_64);
      }
      Serial.println(found_ina260_usb ? "OK " : "Err");
      M5.Lcd.setCursor(ux, uy);
      M5.Lcd.printf(found_ina260_usb ? "OK " : "Err");
    }

    // Zero-point calibration
    if (!calibrated_ina260_usb) {
      while (!ina260_usb.conversionReady()) {}
      usb_offset_mA = ina260_usb.readCurrent();
      // If we're reading more than 4 mA, it looks like we have a device connected, so we don't calibrate
      calibrated_ina260_usb = abs(usb_offset_mA) < 4;
      M5.Lcd.setCursor(ux, uy);
      M5.Lcd.printf(calibrated_ina260_usb ? "OK     " : "Cal Err");
      Serial.printf("USB offset: %.2f mA\n", usb_offset_mA);
    }

    delay(1000);
  }
}

bool hasUsbSample = false;
bool hasBattSample = false;
float usb_mA;
float usb_V;
float batt_mA;
float batt_V;

bool displayMeasurements = false;
void loop()
{
  M5.update();
  if (M5.BtnA.wasPressed()) {
    displayMeasurements = true;
    M5.Lcd.fillScreen(BLACK);
  }

  if (ina260_usb.conversionReady()) {
    usb_mA = ina260_usb.readCurrent() - usb_offset_mA;
    usb_V = ina260_usb.readBusVoltage() / 1000;
    hasUsbSample = true;
    Serial.print("USB: ");
    Serial.print(usb_mA);
    Serial.print(" mA, ");
    Serial.print(usb_V);
    Serial.println(" V");
  }
  if (ina260_batt.conversionReady()) {
    batt_mA = ina260_batt.readCurrent() - batt_offset_mA;
    batt_V = ina260_batt.readBusVoltage() / 1000;
    hasBattSample = true;
    Serial.print("Batt: ");
    Serial.print(batt_mA);
    Serial.print(" mA, ");
    Serial.print(batt_V);
    Serial.println(" V");
  }
  if (hasUsbSample && hasBattSample) {
    hasUsbSample = hasBattSample = false;
    Serial.println(String(millis()/1000.0) + "," + String(batt_mA) + "," + String(usb_mA));
  }

  if (displayMeasurements) {
//    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 10, 1);
    M5.Lcd.printf("Vbat: %.2fV    \r\n", batt_V);
    M5.Lcd.setCursor(10,  M5.Lcd.getCursorY());
    M5.Lcd.printf("Ibat: %.1fmA    \r\n", batt_mA);
    M5.Lcd.setCursor(10, M5.Lcd.getCursorY());
    M5.Lcd.printf("Vusb: %.2fV    \r\n", usb_V);
    M5.Lcd.setCursor(10,  M5.Lcd.getCursorY());
    M5.Lcd.printf("Iusb: %.1fmA    \r\n", usb_mA);
    delay(100);
  }

}
