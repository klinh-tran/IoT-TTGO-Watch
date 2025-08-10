// ProjectThing.ino
// Handles intial setup and main loop 

#include "ProjectThing.h"

TTGOClass *ttgo = nullptr;
ScreenState currentScreen = MAIN_SCREEN;
lv_style_t style;

void setup() {
  Serial.begin(115200);
  ttgo = TTGOClass::getWatch();
  ttgo->begin();
  ttgo->openBL();
  ttgo->lvgl_begin();
  ttgo->motor_begin();
  setCpuFrequencyMhz(240);


  // main screen
  lv_style_init(&style);
  lv_style_set_text_color(&style, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  lv_style_set_text_font(&style, LV_STATE_DEFAULT, &lv_font_montserrat_22);

  setupClockModule();
  setupAlarmModule();
  setupStepModule();
  setupBluetoothModule();

  setupWakeup();
  lv_scr_load(mainScreen);
  

  // start serial bluetooth connection
  Serial.println("The device started, now you can pair it with bluetooth!");

  pinMode(RTC_INT_PIN, INPUT_PULLUP);
}

void loop() {
  lv_task_handler();

  loopAlarm();
  loopStep();

  loopTouchWakeup();
}
