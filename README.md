
# COM3505 Lab Assessment 2

# TTGO Watch 2020 V3

## Introduction
<!-- ***Add intro about the project and the watch here*** - Cam -->
This project aims to create a simple user interface for the LilyGO T-Watch 2020 v3 providing the user with a multi screen interface implementing different aspects of the watches hardware.

## Demonstration
[![](https://i9.ytimg.com/vi/PiwBYNNtfWc/mqdefault.jpg?sqp=CJTAnLIG-oaymwEoCMACELQB8quKqQMcGADwAQH4Ac4FgAKACooCDAgAEAEYZSBVKEowDw==&rs=AOn4CLBdcHFblcNYZSEtzdvO9mVU2vW7KA)](https://youtube.com/shorts/PiwBYNNtfWc "Watch UI Demonstration")

## Installation
### Requirements
- **TTGO T-Watch 2020 v3:** connected to computer via USB
- **Platformio core:** see https://platformio.org/install/cli for more details

### Install
Follow the following command line commands to install firmware on your T-Watch 2020 v3

~~~
git clone git@gitlab.com:crobinson5/com3505-student-2024.git
cd com3505-student-2024/LA2
pio run upload
~~~

Alternatively to view build process and monitor serial line on computer run.

~~~
pio run -t upload -t monitor
~~~

## Technical Section
### Hardware Components
- **Microcontroller:**
    - Chip: ESP32 (240Mhz dual core processor)
    - Flash memory: 16MB
    - PSRAM: 8MB
    - SRAM: 520KB
    - Built-in Wi-Fi
    - Built-in Bluetooth

- **Screen:**
    - Display diagonal: 1.54 inch
    - Resolution: 240 x 240 pixels
    - Pixel colors: RGB
    - Type: IPS
    - Driver: ST7789
    - Communication: SPI

- **Touch screen:**
    - Type: Capacitive
    - Controller: FT6336
    - Communication: I2C

- **Real-Time Clock (RTC):**
    - Chip: PCF8563
    - Communication: I2C

- **Battery:**
    - Capacity: 350mAh
    - Type: Li-Po

- **Power management:**
    - Chip: AXP202
    - Li-ion/Li-Po charging circuit: Yes
    - Li-ion/Li-Po protection circuit: Yes

- **Speaker:**
     - Maximum power: 1W
     - Amplifier: MAX98357A
     - Communication: I2S

- **IMU:**
     - Chip: BMA423
     - Number of axis: 3
     - Communication: I2C
- Built-in microphone and speaker
- Vibrating motor
<!--## Hardware Setup (do we have this?)-->

## Software Implementation
The code implementation are mainly referred from [TTGO_TWatch_Library](https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library/tree/master)

In terms of GUI design, there is a combination of LVGL graphics library and TFT_eSPI - TFT graphics display for Arduino processor.

### Multiple screens
To allow multiple screens displayed on the watch, we used *switch-case* to result in the corresponding display and actions according to the state of current screen (main screen or another screen). 

Configure *TOUCH_INT* pin as an input to receive signals related to touch sceren events. With the touching events, we use left/right swipe to transition between screens. The left/right swipe is detected based on the first and last horizontal touch coordination on the screen. The threshold of difference between the two points is set to be 50 units. If the difference exceeds 50, a positive variance indicates a right swipe while a negative variance indicates a left swipe. 

When a left/right swipe is invoked, the adjacent left/right screen relative to the current screen is displayed. If there are no further screens in the corresponding direction, no change occurs, maintaining the current screen display.

The screens available on the watch:
 - ***Main screen***: This screen appears upon device start-up, features a customised background, and displays battery percentage, date and time
 - ***First left screen***: Swiping left from the main screen leads to this screen, which displays step counts, corresponding distance walked (in miles) and calories (in kcal) calculated from step counts
 - ***Second left screen***: Further left swiping brings users to this screen, which provides a button to toggle Bluetooth connection ON/OFF for audio play
- ***First right screen***:: Swiping right from the mains screen leads to the alarm screen. This allows the user to set an alarm. When the time of the alarm occurs a red screen will appear with two buzzes from the internal motor.

### Main screen
- ***Date and time***: Use the RTC (Real-Time Clock) module integrated into the watch to retrieve the current date and time. Since the project operates within the UK timezone and the RTC is one hour ahead, one hour subtracted from the RTC hour to ensure accurate UK time displayed. Date and time information are continually checked at 100ms interval
- ***Battery percentage***: Access the AXP202 module to retrieve the battery percentage value and display it on the main screen alongside other essential information. Battery perentage is checked continually at 30-second interval
- ***GUI design***: use LVGL graphics library
- ***Customised background***: use [LVGL online tool](https://lvgl.io/tools/imageconverter) to convert normal image file to C array to use in LVGL

### Step counts
- Step counts: 
    - Configure and enable BMA423 Accelerometer: output data rate, G-range, bandwidth, and filter performance mode are configured using the Acfg structure. Read data from the accelerometer
    - An interrupt is set up on pin BMA423_INT1, which triggers a callback function whenever a step is detected
    - The BMA423 step count feature is enabled using *enableFeature(BMA423_STEP_CNTR, true)*

- Distance (in miles):
    - We take "The general number of steps in a mile is about 2,000", hence
    - The calculation is *Distance = Step Counts / 20000*

- Calories (in kcal):
    - We take "0.04 calories per step", hence
    - The calculation is *Distance = Step Counts / 20000*

### Play audio with Bluetooth connection
- The code depends on the [btAudio](https://github.com/tierneytim/btAudio/tree/master) which enables Bluetooth audio functionality
- The audio device is named as ***TWATCH_Speaker***
- Configure to use the following pins for I2S communication:
    - BCK (Bit Clock): Synchronizes timing of data transmission.
    - WS (Word Select): Indicates start of a new audio sample.
    - DOUT (Data Output): Carries audio data from Bluetooth module to DAC.

### Data transfer via Bluetooth Serial
- Use [BluetoothSerial](https://github.com/espressif/arduino-esp32/tree/master/libraries/BluetoothSerial) to provide functionality for Bluetooth communication
- The device is set up to be discoverable and pairable over Bluetooth in the [Serial Bluetooth Terminal app](https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal&hl=en_GB&gl=US) (only avaialble in Google Play) under the name of ***TTGO_Watch***
- After connection is successful by selecting the device name under *Bluetooth Classic* in *Device* section, navigate to *Terminal*
- In the terminal, it displays the real-time updated information on *number of step counts, distance walked, calories, and audio Bluetooth status (on/off)* whenever there is detected change in step counts

### Display on/off
- Configure a pin connected to the AXP202 power management IC as an input with a pull-up resistor enabled *(pinMode(AXP202_INT, INPUT_PULLUP))*. This pin is typically used to receive interrupt signals from the AXP202.
-  Enable an interrupt request (IRQ) associated with a short press event on the PEK key (Power Button) of the AXP202 power management IC. It enables the device to detect when the PEK key is pressed
- Pressing PEK key will either turn on or off the display screen

### Motor Control
- Configure motor control when there is no screen transition available, audio Bluetooth status change (button touch), waking up or turning off device screen by short press of PEK key and when alarm interrupt is detected.

### Alarm Clock
- The PCF8563 RTC chip comes with built in alarm functions for setting an alarm based on minute,hour,day and weekday. User input is handled with the roller lvgl widget which allows the user to scroll through a set of values, there is one roller implemented for minutes and another for hours. A button widget is used to activate the alarm changing the state of the gui to make the rollers disapear and the alarm button to read disable instead of enable.

### GUI 
- The gui is implemented using the open source lvgl library which has been ported to the LilyGO hardware in the main development repository for the LilyGO watch. The library allows the development of graphical interfaces which use similar paradigms to those found in the web development world. Our project makes use of some of the built in widgets including the roller widget on the alarm screen and the arc widget on the step counter screen. More information about the lvgl graphics library can be found [here](https://lvgl.io/).

## Testing

### Transition between multiple screens
- Swiping across screens. Screens displaying the corect content as implemented, low latency detected. Motor/Vibration occurs at every swipe where there isnt a transition to another screen.
- When coming back and forth among different screens, the content displayed is proper with correct updated information where necessary

### Main screen
- *Date and time* are checked to displayed properly by comparing with other electronic devices having date and time running
- *Battery* is accurately displayed at the expected position on the screen. To validate its functionality, the watch was in use continuously and expected a drop in battery percentage over time. Conversely, when the watch was charging, the battery percentage increased as anticipated. The behaviours allign with the expected funcitonality, indicating proper operations and implementation
- Issue with battery display:
    - Initially, the battery value was checked every 100ms, similar to the frequecy of checking on date and time
    - However. there observed a flickering display of battery percentage, likely due to hardware issues such as electrical noise or loose connections. The hardware components are built-in, so to address this, we increased the interval for checking the battery percentage every 30 seconds, which is still a reasonable interval to detect a new battery value while providing a more stable display
    - Nevertheless, this longer interval introduced a new problem where it took 30 seconds to display the battery value after turning on the watch or returning to the main screen. To mitigating this delay, we added in the setup process to retrieve the battery percentage immediately. Therefore, the battery level is known promptly upon startup, and then the displayed value is updated every 30 seconds. 
    - As a result of those adjustments, we were able to avoid the delay in displaying the battery percentage upon device startup while minimising the flikering on the display

### Step counts
- The step count functionality is test by wearing the watch and walking as usual. While walking, the watch wearer counted the number of steps and double checked with the watch, which alligns with the number shown on display
- Distance and calories are calulated manually according to the step counts and the input formula, then compared with the displayed figure, resulting in the same results
- It is tested and verified that step counts are always mornitored once the watch is turned on, even when navigating to other screens, turning off the display, etc
- Since step counts are registered based on interrupts from the chip, the situations where the wearer maintains the watch in a stable position, e.g. holding the phone steadily for engagement while walking,may not trigger interruptions, hence steps may not be counted or recorded during these periods

### Bluetooth: Audio and Serial
- In terms of **Audio Bluetooth** connection:
    - The control button is checked with turning Bluetooth on/off, which enables/stops Bluetooth connection respectively
    - When the Bluetooth connection is set up successfully, the audio from another device can be played through the watch's speaker. The connection remains stable with the button reflecting the current status (on) even when navigating to other screens and returning
    - Toggling the button off immediately interrupts and terminates the Blueooth connection with the correct current status displayed
- In terms of **Serial Bluetooth** connection:
    - The Serial Bluetooth Connection is set up since the beginning and it is never turned off
    - The watch is discoverable in the Serial app and the connection is set up successfully
    - With the watch worn on, it is verified that the serial Bluetooth connection is not interrupted by other watch's features (but can manually disconnect or reconnect via app) by checking on the data transmitted from the watch to the terminal; whenever there are new step counts, the information is immediately transmitted to the terminal, showing the updated figures on step counts, distance, and calories, together with the current audio Bluetooth status

- It is verified that there are no conflicts between the two types of Bluetooth connection. It was set up that: the audio from another device was played while the watch wearer was walking with the watch on. It is justified that the audio is played smoothly with the accurate information sent to the terminal promptly

### Display on/off
- The feature is checked as:
    - When the screen is on and short pressing PEK key, vibration occurs and the screen turns off
    - When the screen is off and short pressing PEK key, vibration occurs and the screen turns on
    - When the screen is off:
        - it is verified that no touch events are active (touching is disabled), by attempting to swipe left or right. When the display is back on, no screen change is detected
        - it is verified that the step monitoring is still functioning. The watch is worn with the display turned off, and during walking, an increase in step counts is recorded
        - it is verified that the audio Bluetooth connection is not interrupted with playing audio if it was set up beforehand
        - it is verified that the serial Bluetooth connection is not interrupted. The transmission of required information is consistent with real-time updates
        <!-- TODO: add info about alarm -->
    - These actions have been repeated several times to make sure the functionality works as anticipated

### Alarm
- Alarm vibration tested when display is off to make sure that the user is still notified.
- Tested alarm notification appears even when not on alarm screen.
- Tested that alarms are able to be set after one alarm has gone off
- Tested that multiple alarms can be cancelled properly then set and still go off.

## Self assessment
### Challenges during implementation
- During the early stages of implementation, we used LVGL as our graphics library for GUI design. As we progressed and started implementing audio bluetooth functionality, we explored the chance to Over-The-Air (OTA) updates by incorporating a website interface to configure the audio-related features when the Bluetooth connection is established. However, we encountered a memeory constraint issue: LVGL and WiFi use the same memory space, so when setting up Bluetooth with WiFi connection active, the memory became overloaded, resulting in operational failures. At the moment, we have decided to remove OTA feature from this project.


### Future Improvements
<!--include features havent tried or what can have been better, but havent due to time contraints-->

- These are the features potentially improved if this project is extended or continued in the future:
    - **Distance and Calories Calculation**: To enhance the accuracy of distance walked and calories burnt calculations, individualised measurements must be inputted rather than relying on average calculations. As mentioned above, there is a clash between the use of LVGL and WiFi connection. Thereby, I will use another graphics library (e.g. TFT library for Arduino) rather than LVGL, so that OTA feature becomes valid. The approach involves designing a simple web interface where users can enter their personal information such as weight and height, combined with more specific conversion formula. The system will then be able to provide personalised data on distance walked and calories burnt despite the same step counts.
    <br>

    - **Motor control on/off**: With the OTA interface, the user can choose to have motor control activated on the watch or not. This is because not everyone is comfortable with the vibrating effect.
    <!-- TODO: How to improve inconsistent vibration going with touch event theoretically?-->
    - **Storing and analysis of data**: To improve the usability of the watch I would implement a syncing feature where at the end of the day the watch attempts to write the users data e.g. number of steps, kcal and miles to a web server which stores the data. The user could then access a website which displays this data in  meaningful way such as tracking average steps over a certain period of time or calories burned in a week. 

    - **Performance improvements:** To improve battery life of the watch when the screen is off further testing could be done to disable processes which aren't necessary. Also could look at reducing the amount of checks of certain functions.
