#include "../ProjectThing.h"

char buf[128];
bool irq_touch = false;
boolean isScreenOn = true;

void setupWakeup() {
  pinMode(AXP202_INT, INPUT_PULLUP);
  attachInterrupt(
      AXP202_INT, [] { irq_touch = true; }, FALLING);
  //! Clear IRQ unprocessed  first
  ttgo->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ, true);
  ttgo->power->clearIRQ();

  pinMode(TOUCH_INT, INPUT);
}

// Handles screen sleep and wakeup
void loopTouchWakeup() {
  // touch PEK button to turn off screen
  ttgo->power->readIRQ();
  if (ttgo->power->isPEKShortPressIRQ()) {
    if (isScreenOn) {
      ttgo->motor->onec();
      // Clean power chip irq status
      ttgo->power->clearIRQ();
      ttgo->closeBL();
      ttgo->stopLvglTick();
      ttgo->displaySleep();
    } else {
      ttgo->motor->onec();
      ttgo->startLvglTick();
      ttgo->displayWakeup();
      ttgo->openBL();
    }
    isScreenOn = !isScreenOn;
  }
  ttgo->power->clearIRQ();
}

// Handles transitioning between screens based on gestures received from LVGL events
void gesture_event_handler(lv_obj_t *obj, lv_event_t event) {
  if (isScreenOn) {
    if (event == LV_EVENT_GESTURE) {
      lv_gesture_dir_t direction = lv_indev_get_gesture_dir(lv_indev_get_act());
      switch (currentScreen) {
      case MAIN_SCREEN:
        switch (direction) {
        case LV_GESTURE_DIR_LEFT:
          lv_scr_load(alarmScreen);
          currentScreen = ALARM_SCREEN;
          break;
        case LV_GESTURE_DIR_RIGHT:
          lv_scr_load(stepScreen);
          currentScreen = STEP_SCREEN;
          break;
        default:
          break;
        }
        break;
      case ALARM_SCREEN:
        switch (direction) {
        case LV_GESTURE_DIR_RIGHT:
          lv_scr_load(mainScreen);
          currentScreen = MAIN_SCREEN;
          break;
        case LV_GESTURE_DIR_LEFT:
          ttgo->motor->onec();
          break;
        default:
          break;
        }
        break;
      case STEP_SCREEN:
        switch (direction) {
        case LV_GESTURE_DIR_RIGHT:
          currentScreen = BT_SCREEN;
          lv_scr_load(bluetoothScreen);
          break;
        case LV_GESTURE_DIR_LEFT:
          currentScreen = MAIN_SCREEN;
          lv_scr_load(mainScreen);
          break;
        default:
          ttgo->motor->onec();
          break;
        }
        break;
      case BT_SCREEN:
        switch (direction) {
        case LV_GESTURE_DIR_LEFT:
          currentScreen = STEP_SCREEN;
          lv_scr_load(stepScreen);
          break;
        case LV_GESTURE_DIR_RIGHT:
          ttgo->motor->onec();
          break;
        default:
          break;
        }
      default:
        break;
        break;
      }
    }
  }
}
