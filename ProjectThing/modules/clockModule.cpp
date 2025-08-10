//
// Clock Module
// Handles the alarm screen which uses the rtc module on the lilygowatch to
// create alarms Input from the user is from a lvgl roller widget When alarm
// interupt is detected red screen is shown along with vibration of motor
//

#include "../ProjectThing.h"

AXP20X_Class *power;
char battery_text[8];
int battery_percentage;
PCF8563_Class *rtc;

lv_obj_t *mainScreen;

typedef struct {
  lv_obj_t *battery_level;
  lv_obj_t *hour;
  lv_obj_t *minute;
  lv_obj_t *second;
  lv_obj_t *date;
} str_datetime_t;
static str_datetime_t g_data;

void setupClockModule() // the main screen setup
{
  rtc = ttgo->rtc;
  battery_percentage = ttgo->power->getBattPercentage();
  // Use compile time
  rtc->check();
  // Lower the brightness
  ttgo->bl->adjust(150);
  setupClockGUI();
}

void setupClockGUI() {
  mainScreen = lv_obj_create(NULL, NULL);
  lv_obj_set_event_cb(mainScreen, gesture_event_handler);

  // MAIN SCREEEN //
  lv_obj_t *img1 = lv_img_create(mainScreen, NULL);
  lv_img_set_src(img1, &snoopy);
  lv_obj_align(img1, NULL, LV_ALIGN_CENTER, 0, 0);

  // battery
  g_data.battery_level = lv_label_create(img1, nullptr);
  lv_obj_add_style(g_data.battery_level, LV_OBJ_PART_MAIN, &style);
  lv_label_set_text(g_data.battery_level, "%");
  lv_obj_align(g_data.battery_level, img1, LV_ALIGN_IN_TOP_LEFT, 20, 90);

  // time
  g_data.hour = lv_label_create(img1, nullptr);
  lv_obj_add_style(g_data.hour, LV_OBJ_PART_MAIN, &style);

  lv_label_set_text(g_data.hour, "00");
  lv_obj_align(g_data.hour, img1, LV_ALIGN_IN_TOP_MID, 10, 90);

  g_data.minute = lv_label_create(img1, nullptr);
  lv_obj_add_style(g_data.minute, LV_OBJ_PART_MAIN, &style);
  lv_label_set_text(g_data.minute, "00");
  lv_obj_align(g_data.minute, g_data.hour, LV_ALIGN_OUT_RIGHT_MID, 5, 0);

  g_data.second = lv_label_create(img1, nullptr);
  lv_obj_add_style(g_data.second, LV_OBJ_PART_MAIN, &style);
  lv_label_set_text(g_data.second, "00");
  lv_obj_align(g_data.second, g_data.minute, LV_ALIGN_OUT_RIGHT_MID, 9, 0);

  // date
  g_data.date = lv_label_create(img1, nullptr);
  lv_obj_add_style(g_data.date, LV_OBJ_PART_MAIN, &style);
  lv_label_set_text(g_data.date, "YYYY-MM-DD");
  lv_obj_align(g_data.date, g_data.hour, LV_ALIGN_OUT_BOTTOM_MID, 00, 20);

  lv_task_create(
      [](lv_task_t *t) {
        battery_percentage = ttgo->power->getBattPercentage();
        lv_label_set_text(g_data.battery_level, battery_text);
      },
      30000, LV_TASK_PRIO_LOW, nullptr);

  lv_task_create(
      [](lv_task_t *t) {
        snprintf(battery_text, sizeof(battery_text), "%u%%",
                 battery_percentage);
        lv_label_set_text(g_data.battery_level, battery_text);

        RTC_Date curr_datetime = rtc->getDateTime();
        lv_label_set_text_fmt(g_data.second, "%02u", curr_datetime.second);
        lv_label_set_text_fmt(g_data.minute, "%02u", curr_datetime.minute);

        // RTC time is 1 hour ahead of UK times
        if (curr_datetime.hour > 0) {
          lv_label_set_text_fmt(g_data.hour, "%02u", curr_datetime.hour-1);
        } else {
          curr_datetime.hour = 23;
          lv_label_set_text_fmt(g_data.hour, "%02u", curr_datetime.hour);
        }

        // Update the date label
        char date_str[11]; // Buffer for the date string (YYYY-MM-DD + '\0')
        sprintf(date_str, "%04u-%02u-%02u", curr_datetime.year,
                curr_datetime.month, curr_datetime.day);
        lv_label_set_text(g_data.date, date_str);
      },
      100, LV_TASK_PRIO_MID, nullptr);
}
