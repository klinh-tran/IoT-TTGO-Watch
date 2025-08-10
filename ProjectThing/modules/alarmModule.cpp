// 
// Alarm Module
// Handles the alarm screen which uses the rtc module on the lilygowatch to create alarms
// Input from the user is from a lvgl roller widget 
// When alarm interupt is detected red screen is shown along with vibration of motor
//

#include "../ProjectThing.h"

boolean alarmActive = false;
bool rtcIrq = false;

lv_obj_t *alarmScreen;
lv_obj_t *alarmNotificationScreen;
lv_obj_t *alarmLabel;
lv_obj_t *alarmSetLabel;
lv_obj_t *hourRoller;
lv_obj_t *minuteRoller;

void setupAlarmModule() { setupAlarmGUI(); }

// Function which handles the setting and enabling of an alarm
void startAlarm(uint8_t hour, uint8_t minute) {
  Serial.println("Calling alarm setup");
  alarmActive = true;

  Serial.println(hour);
  Serial.println(minute);

  attachInterrupt(
      RTC_INT_PIN, [] { rtcIrq = 1; }, FALLING);

  RTC_Date curDate = ttgo->rtc->getDateTime();
  // ttgo->rtc->setAlarm(alarm);
  ttgo->rtc->disableAlarm();
  ttgo->rtc->setAlarm(hour, minute, PCF8563_NO_ALARM, PCF8563_NO_ALARM);
  ttgo->rtc->enableAlarm();
}

// Loop to check to see if alarm interrupt is activated
// if true interrupt is detatched, notification screen is shown and alarm is disabled
// could improve by using lvgl screen for notification gui
void loopAlarm() {
  if (alarmActive) {
    if (rtcIrq) {
      Serial.println("Alarm ALERT");
      rtcIrq = 0;
      detachInterrupt(RTC_INT_PIN);
      ttgo->rtc->resetAlarm();
      ttgo->tft->fillScreen(TFT_RED);
      ttgo->motor->onec();
      delay(1000);
      lv_scr_load(lv_scr_act());
      ttgo->motor->onec();
      alarmActive = false;
    }
  }
}

// Function to setup lvgl alarm screen gui
void setupAlarmGUI() {
  alarmScreen = lv_obj_create(NULL, NULL);
  lv_obj_set_event_cb(alarmScreen, gesture_event_handler);

  hourRoller = lv_roller_create(alarmScreen, NULL);
  lv_obj_add_style(hourRoller, LV_OBJ_PART_MAIN, &style);
  lv_roller_set_options(hourRoller,
                        "00\n"
                        "01\n"
                        "02\n"
                        "03\n"
                        "04\n"
                        "05\n"
                        "06\n"
                        "07\n"
                        "08\n"
                        "09\n"
                        "10\n"
                        "11\n"
                        "12\n"
                        "13\n"
                        "14\n"
                        "15\n"
                        "16\n"
                        "17\n"
                        "18\n"
                        "19\n"
                        "20\n"
                        "21\n"
                        "22\n"
                        "23",
                        LV_ROLLER_MODE_INFINITE);

  lv_obj_set_width(hourRoller, 100);
  lv_obj_align(hourRoller, NULL, LV_ALIGN_CENTER, -25, -50);

  minuteRoller = lv_roller_create(alarmScreen, NULL);
  lv_obj_add_style(minuteRoller, LV_OBJ_PART_MAIN, &style);
  lv_roller_set_options(minuteRoller,
                        "00\n"
                        "01\n"
                        "02\n"
                        "03\n"
                        "04\n"
                        "05\n"
                        "06\n"
                        "07\n"
                        "08\n"
                        "09\n"
                        "10\n"
                        "11\n"
                        "12\n"
                        "13\n"
                        "14\n"
                        "15\n"
                        "16\n"
                        "17\n"
                        "18\n"
                        "19\n"
                        "20\n"
                        "21\n"
                        "22\n"
                        "23\n"
                        "24\n"
                        "25\n"
                        "26\n"
                        "27\n"
                        "28\n"
                        "29\n"
                        "30\n"
                        "31\n"
                        "32\n"
                        "33\n"
                        "34\n"
                        "35\n"
                        "36\n"
                        "37\n"
                        "38\n"
                        "39\n"
                        "40\n"
                        "41\n"
                        "42\n"
                        "43\n"
                        "44\n"
                        "45\n"
                        "46\n"
                        "47\n"
                        "48\n"
                        "49\n"
                        "50\n"
                        "51\n"
                        "52\n"
                        "53\n"
                        "54\n"
                        "55\n"
                        "56\n"
                        "57\n"
                        "58\n"
                        "59",
                        LV_ROLLER_MODE_INFINITE);

  lv_obj_set_width(minuteRoller, 100);
  lv_obj_align(minuteRoller, NULL, LV_ALIGN_CENTER, 25, -50);

  lv_obj_t *btn1 = lv_btn_create(alarmScreen, NULL);
  lv_obj_set_size(btn1, 150, 50);
  lv_obj_align(btn1, NULL, LV_ALIGN_CENTER, 0, 60);
  lv_obj_set_event_cb(btn1, alarm_button_event_handler);

  alarmLabel = lv_label_create(btn1, NULL);
  lv_obj_add_style(alarmLabel, LV_OBJ_PART_MAIN, &style);
  lv_label_set_text(alarmLabel, "Start");

  alarmSetLabel = lv_label_create(alarmScreen, NULL);
  lv_obj_add_style(alarmSetLabel, LV_OBJ_PART_MAIN, &style);
  lv_label_set_text(alarmSetLabel, "ALARM SET");
  lv_obj_align(alarmSetLabel, NULL, LV_ALIGN_CENTER, 0, -20);
  lv_obj_set_hidden(alarmSetLabel, true);
}


// Handles the onclick event of the button on the alarm screen
// If alarm is not active: Disables the roller on the alarm screen and calls the set alarm function
// If alarm is active: Disable alarm and interrupt, then enable roller 
static void alarm_button_event_handler(lv_obj_t *obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    Serial.println("Button Clicked");
    if (!alarmActive) {
      alarmActive = true;
      lv_label_set_text(alarmLabel, "Stop");
      lv_obj_set_hidden(hourRoller, true);
      lv_obj_set_hidden(minuteRoller, true);
      lv_obj_set_hidden(alarmSetLabel, false);

      char minuteBuf[32];
      lv_roller_get_selected_str(minuteRoller, minuteBuf, sizeof(minuteBuf));

      char hourBuf[32];
      lv_roller_get_selected_str(hourRoller, hourBuf, sizeof(hourBuf));

      uint8_t hour = atoi(hourBuf);
      uint8_t minute = atoi(minuteBuf);

      Serial.println("Alarm Set");
      startAlarm(hour, minute);

    } else {
      alarmActive = false;
      detachInterrupt(RTC_INT_PIN);
      ttgo->rtc->resetAlarm();

      lv_label_set_text(alarmLabel, "Start");
      lv_obj_set_hidden(hourRoller, false);
      lv_obj_set_hidden(minuteRoller, false);
      lv_obj_set_hidden(alarmSetLabel, true);
    }
  }
}
