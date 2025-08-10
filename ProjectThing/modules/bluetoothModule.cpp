#include "../ProjectThing.h"

lv_obj_t *bluetoothLabel;
BluetoothSerial SerialBT;

lv_obj_t *bluetoothScreen;

bool bluetoothOff = true;
// Sets the name of the audio device
btAudio audio = btAudio("TWATCH_Speaker");

// serial bluetooth
// APP link:
// https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal&hl=en_GB&gl=US
// only available on Google Play
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

void setupBluetoothModule() {
  SerialBT.begin("TTGO_Watch"); // Bluetooth device name
  setupBluetoothGUI();
}

void setupBluetoothGUI() {
  bluetoothScreen = lv_obj_create(NULL, NULL);
  lv_obj_set_event_cb(bluetoothScreen, gesture_event_handler);

  lv_obj_t *bluetoothButton = lv_btn_create(bluetoothScreen, NULL);
  lv_obj_set_size(bluetoothButton, 150, 50);
  lv_obj_align(bluetoothButton, NULL, LV_ALIGN_CENTER, 0, 50);
  lv_obj_set_event_cb(bluetoothButton, bluetooth_button_event_handler);

  bluetoothLabel = lv_label_create(bluetoothButton, NULL);
  lv_obj_add_style(bluetoothLabel, LV_OBJ_PART_MAIN, &style);
  lv_label_set_text(bluetoothLabel, "Enable");

  lv_obj_t *btSpeakerLabel = lv_label_create(bluetoothScreen, NULL);
  lv_obj_add_style(btSpeakerLabel, LV_OBJ_PART_MAIN, &style);
  lv_label_set_text(btSpeakerLabel, "BT SPEAKER");
  lv_obj_align(btSpeakerLabel, NULL, LV_ALIGN_CENTER, 0, -50);
}

static void bluetooth_button_event_handler(lv_obj_t *obj, lv_event_t event) {
  if (event == LV_EVENT_CLICKED) {
    bluetoothOff = !bluetoothOff;
    if (bluetoothOff == false) {
      //! Open aduio power
      ttgo->enableLDO3();
      // start bluetooth audio
      audio.begin();
      bluetoothSetup();
      lv_label_set_text(bluetoothLabel, "Disable");
    } else {
      audio.end();
      lv_label_set_text(bluetoothLabel, "Enable");
    }
  }
}

void bluetoothSetup() {

  //  attach to pins
  int bck = TWATCH_DAC_IIS_BCK;
  int ws = TWATCH_DAC_IIS_WS;
  int dout = TWATCH_DAC_IIS_DOUT;
  audio.I2S(bck, dout, ws);
}

void bluetoothLoop() {
  if (oldStep != step) {
    bluetoothSerialActions();
    oldStep = step;
  }
}

void bluetoothSerialActions() {
  SerialBT.print("Steps: ");
  SerialBT.println(step);

  SerialBT.print("Distance: ");
  SerialBT.print(miles);
  SerialBT.println(" miles");

  SerialBT.print("Calories: ");
  SerialBT.print(kcal);
  SerialBT.println(" kcal");

  SerialBT.println(bluetoothOff ? "Bluetooth OFF" : "Bluetooth ON");
  SerialBT.println();
}
