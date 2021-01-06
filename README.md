
# Transfer :rocket:

**Hello everyone! This is my third challenge in course form Ucode IT Academy featuring Ajax Systems.**

  

## The main idea :sparkles:

**Implement wireless data transmission using the radio.**

  

## Our equipment

* **TI LaunchPad CC1352R1 x2**

* **TI Sensors BoosterPack “BOOSTXL-SENSORS”**

* **Laptop or PC**

  

## Software that I’ve used

* **Code Composer Studio**

* **UniFlash**

* **SmartRF Studio**

* **PyCharm**

  

## What is my project about

**This project was made to help gardeners grow their plants. Our system will help you to keep the correct temperature and humidity in your greenhouse. If something goes wrong - you will get notification in gmail.**

  

## How does it works

**First (TX) microcontroller has** _Sensors BoosterPack_ **with** _bme280_ **and every minute checks temperature and humidity in the greenhouse. If current temperature is not into the range** _15-20 degrees_ **or current humidity is not into the range** _60-80%_**, first (TX) microcontroller will send a packet (using radio protocol) with current values to another (RX) microcontroller. RX microcontroller takes values and writes into the serial terminal. From the serial terminal these values will be read by python script, that will send you email like:**

>Subject: GREENHOUSE EMERGENCY
>
>===================================
>
>
>
>Our Sensors have measured changes in the environment.
>
>  
>
>====================================
>
>Temperature in the greenhouse now: 25.
>
>Humidity in the greenhouse now: 57.
>
>====================================
>
> 
>
>Please, do not let our plants die.
>
>Hope to see you soon in the greenhouse, master!
>
>
>
>===================================


## How you could run it

1. **Download my project.**
2. **Using the Code Composer Studio, built a project in folder “TX”.**
3. **Flash binary file into your TX microcontroller. It should be connected with Sensors BoosterPack. You could use CCS or UniFlash.**
4. **Using the Code Composer Studio, built a project in folder “RX”.**
5. **Flash binary file into your RX microcontroller. It should be connected with your hub (laptop or pc). You could use CCS or UniFlash.**
6. **Open Python->send_msg.py. In line 5 change ‘your email’ with your email, in line 6 change ‘your password’ with your password, in line 12 change 'port that you need' with port where your RX microcontroller is connected. REMEMBER: now this file contains your personal information! Do not show it to someone.**
7. **Run send_msg.py.**


## Author
**Illia Marchenko, 18 y.o, Ukraine, Kiev, junior embedded developer, email: [ilya.marchenko02@gmail.com](mailto:ilya.marchenko02@gmail.com).**


## Note
**Do not forget that to work with TI Launchpads and Sensors BoosterPack, you should download SDKs.**



