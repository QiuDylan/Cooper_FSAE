# Driver Input Board 

The purpose of the ESP32 microcontroller on the driver input board is to check for plausibility between APPS sensors and send out RTDS signal. FreeRTOS will be used to perform these functions at the same time. 

**RTOS**

FreeRTOS is used to execute tasks in parallel. The driver input board has two main functions and the ESP32 has two cores, so one function will be pinned to one core. 

**Description of Code**  

Inputs
-	APPS_1 (analog) = output of APPS sensor 1
-	APPS_2 (analog) = output of APPS sensor 2
-	TRACTIVE_SYSTEM (digital) = 0 if tractive system active, 1 if tractive system inactive
-	BRAKE_PRESSURE_IN (analog) = brake pressure at 0-100mV
-	BUTTON (digital) = 1 if button pressed, 0 if button not pressed 

Outputs
-	THROTTLE (analog) = ratio of APPS_1:APPS_2, 0 if sensors are implausible
-	BRAKE_PRESSURE_OUT (analog) = brake pressure at 0-3.3V
-	RTDS (digital) = 1 if ready to drive, 0 if not ready

Functions
-	Apps_check
    - Checks for implausibility between APPS sensors. If the difference between APPS_1 and APPS_2 is more than 10% and lasts for more than 100 ms, the throttle output is 0. Otherwise, the throttle output is the ratio between APPS_1 and APPS_2
-	RTDS_check 
    - Indicates if the vehicle is Ready to Drive. The conditions for Ready to Drive are if the tractive system is active, brake pressure > 0, and the button is pressed. 
    - Linearly converts 0-100mV brake pressure input into 0-5V brake pressure output


**ADC and DAC Conversions**

The ADC of the ESP32 reads voltages between 0 and 3.3V. analogRead() has a resolution of 12 bits, so the range of values is from 0-4095. 
-	Brake pressure: The brake pressure input has a range of 0-100mV. The DAC of the ESP32 outputs voltages between 0 and 3.3V. dacWrite() has a resolution of 8 bits, so the range of values is from 0-255. 
    -	Brake pressure: The 0-100mV signal has digital values from 0 to 124 so BRAKE_PRESSURE_OUT = map(BRAKE_PRESSURE_IN, 0, 124, 0, 255)
-	APPS: The PST360 outputs a voltage from 0-5V so a voltage divider is used to shift the signal from 5V to 3.3V. The sensor measures angle values from 0-40 degrees.   
<img src="https://i.imgur.com/D0QQ7qf.png" width=500 height=200>
    - Angle = map(APPS, 0, 4095, 0, 40)


**Executing the Code on ESP32**

1.	In Arduino IDE, go to file -> preferences and add https://dl.espressif.com/dl/package_esp32_index.json to Additional Board Manager URLs
2.	Go to tools -> board -> board manager and search for ESP32. Install esp32 by Espressif Systems. 
3.  Upload the code to the board. 


