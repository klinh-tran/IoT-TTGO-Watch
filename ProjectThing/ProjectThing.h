#ifndef PROJECTTHING_H
#define PROJECTTHING_H

#include <Arduino.h>
#define LILYGO_WATCH_2020_V3
#define LILYGO_WATCH_HAS_MOTOR
#define LILYGO_WATCH_LVGL // To use LVGL, you need to enable the macro LVGL
#include "BluetoothSerial.h"
#include "lvgl/lvgl.h"
#include "time.h"
#include <LilyGoWatch.h>
#include <WiFi.h>
#include "dependencies/btAudio.h"

extern lv_obj_t *mainScreen;
extern lv_obj_t *alarmScreen;
extern lv_obj_t *stepScreen;
extern lv_obj_t *bluetoothScreen;
extern TTGOClass *ttgo;

extern lv_style_t style;

LV_IMG_DECLARE(snoopy);
LV_FONT_DECLARE(lv_font_montserrat_22);

enum ScreenState {
  MAIN_SCREEN,
  ALARM_SCREEN,
  STEP_SCREEN,
  BT_SCREEN
};

extern ScreenState currentScreen;

void setupWakeup();
void loopTouchWakeup();
void gesture_event_handler(lv_obj_t *obj, lv_event_t event);
extern boolean isScreenOn;

// Step Module
extern uint32_t step;
extern float miles;
extern float kcal;
extern uint32_t oldStep;
void setupStepModule();
void loopStep();
void stepCountSetup();
void setupStepGUI(); 

// Alarm Module
void setupAlarmModule();
void loopAlarm();
void startAlarm(uint8_t hour, uint8_t minute);
void setupAlarmGUI();
static void alarm_button_event_handler(lv_obj_t *obj, lv_event_t event);

// Clock Module
void setupClockModule();
void setupClockGUI(); 

// Bluetooth Module
void setupBluetoothModule();
void setupBluetoothGUI(); 
void bluetoothSerialActions(); 
void bluetoothLoop(); 
void bluetoothSetup();
static void bluetooth_button_event_handler(lv_obj_t *obj, lv_event_t event);

// Control Module

#endif
