#include "../ProjectThing.h"

// for step count
BMA *sensor;
bool irq = false;

uint32_t step = 0;
float miles = 0;
float kcal = 0;

lv_obj_t *stepScreen;
lv_obj_t *stepsLabel;
lv_obj_t *milesLabel;
lv_obj_t *caloriesLabel;
lv_obj_t *arc;

void setupStepModule() {
  setupStepGUI();
  stepCountSetup();
}

void setupStepGUI() {
  stepScreen = lv_obj_create(NULL, NULL);
  lv_obj_set_event_cb(stepScreen, gesture_event_handler);

  arc = lv_arc_create(stepScreen, NULL);
  lv_obj_set_size(arc, 240, 240);
  lv_arc_set_range(arc, 0, 10000);
  lv_arc_set_rotation(arc, 135);
  lv_arc_set_bg_angles(arc, 0, 270);
  lv_arc_set_value(arc, step);
  lv_obj_align(arc, NULL, LV_ALIGN_CENTER, 0, 0);

  stepsLabel = lv_label_create(stepScreen, NULL);
  lv_label_set_text_fmt(stepsLabel, "Steps: %d", step);
  lv_obj_add_style(stepsLabel, LV_OBJ_PART_MAIN, &style);
  lv_obj_align(stepsLabel, NULL, LV_ALIGN_CENTER, -10, -25);

  milesLabel = lv_label_create(stepScreen, NULL);
  lv_label_set_text_fmt(milesLabel, "Miles: %d", miles);
  lv_obj_add_style(milesLabel, LV_OBJ_PART_MAIN, &style);
  lv_obj_align(milesLabel, NULL, LV_ALIGN_CENTER, -10, 0);

  caloriesLabel = lv_label_create(stepScreen, NULL);
  lv_label_set_text_fmt(caloriesLabel, "Kcal: %d", kcal);
  lv_obj_add_style(caloriesLabel, LV_OBJ_PART_MAIN, &style);
  lv_obj_align(caloriesLabel, NULL, LV_ALIGN_CENTER, -10, 25);
}

void stepCountSetup() {
  // Receive objects for easy writing
  sensor = ttgo->bma;

  // Accel parameter structure
  Acfg cfg;
  cfg.odr = BMA4_OUTPUT_DATA_RATE_400HZ;
  /*!
      G-range
  */
  cfg.range = BMA4_ACCEL_RANGE_2G;
  /*!
      Bandwidth parameter, determines filter configuration
  */
  cfg.bandwidth = BMA4_ACCEL_RES_AVG16;

  // Filter performance mode
  cfg.perf_mode = BMA4_CONTINUOUS_MODE;

  // Configure the BMA423 accelerometer
  sensor->accelConfig(cfg);
  // Enable BMA423 accelerometer
  sensor->enableAccel();

  pinMode(BMA423_INT1, INPUT);
  attachInterrupt(
      BMA423_INT1,
      [] {
        // Set interrupt to set irq value to 1
        irq = 1;
      },
      RISING); // It must be a rising edge

  // Enable BMA423 step count feature
  sensor->enableFeature(BMA423_STEP_CNTR, true);

  // Turn on step interrupt
  sensor->enableStepCountInterrupt();
}

void loopStep() {
  if (irq) {
    irq = 0;
    bool rlst;
    do {
      // Read the BMA423 interrupt status,
      // need to wait for it to return to true before continuing
      rlst = sensor->readInterrupt();
    } while (!rlst);

    // Check if it is a step interrupt
    if (sensor->isStepCounter()) {
      // Get step data from register
      step = sensor->getCounter();
      lv_label_set_text_fmt(stepsLabel, "Steps: %d", step);
      lv_arc_set_value(arc, step);

      miles = (step) / 2000.0;
      char miles_str[32];
      sprintf(miles_str, "Miles: %.2f", miles);
      lv_label_set_text(milesLabel, miles_str);

      kcal = static_cast<float>(step) * 0.04;
      char kcal_str[32];
      sprintf(kcal_str, "Kcal: %.2f", kcal);
      lv_label_set_text(caloriesLabel, kcal_str);
    }
  }
}
