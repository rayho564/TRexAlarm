# TRexAlarm
Android and Embedded Systems Alarm

Description
------
A portable motion alarm system that uses several sensors connected through Bluetooth to an Android app. The portable motion alarm system is stickable to walls and runs on three 1.5V batteries. Once turned on when a person or object passes the sensor, the phone will vibrate and send a notification.


Parts List (Click for Info)
------
[ATMega1284P](http://www.atmel.com/images/doc8059.pdf)

[HC-06 Bluetooth](https://www.olimex.com/Products/Components/RF/BLUETOOTH-SERIAL-HC-06/resources/hc06.pdf)
OR [HC-05 Bluetooth](http://www.electronicaestudio.com/docs/istd016A.pdf)

[HC-SR04 Ranging Detector Mod Distance Sensor](https://www.sainsmart.com/products/ultrasonic-ranging-detector-mod-hc-sr04-distance-sensor?utm_medium=cpc&utm_source=googlepla&variant=45100776468&gclid=EAIaIQobChMIn5aQxYb01wIVBpNpCh2K0AFJEAQYBCABEgLmC_D_BwE)

[Mini Breadboard](https://learn.sparkfun.com/tutorials/how-to-use-a-breadboard)

[3x AA Battery Holder with Cover](https://www.jameco.com/Jameco/Products/ProdDS/216144.pdf)

One LED, one resistor, and wires 

Where to buy? Examples
-----
[ATMega1284P](https://www.arrow.com/en/products/atmega1284p-pu/microchip-technology?utm_source=google&utm_campaign=g-shp-us-microcontrollers&utm_medium=cpc&utm_term=PRODUCT+GROUP&gclid=EAIaIQobChMIrpqliIX01wIVCYGzCh1JoAdCEAQYASABEgKzN_D_BwE&gclsrc=aw.ds&dclid=CMS2i4qF9NcCFcFYfgodPJUBNQ)

[HC-06 Bluetooth](https://www.newegg.com/Product/Product.aspx?Item=9SIAD4R5YW3571&ignorebbr=1&nm_mc=KNC-GoogleMKP-PC&cm_mmc=KNC-GoogleMKP-PC-_-pla-_-EC+-+Circuit+Protection-_-9SIAD4R5YW3571&gclid=EAIaIQobChMIosfyyYX01wIVBAlpCh1KxArJEAQYASABEgJChvD_BwE&gclsrc=aw.ds)
OR [HC-05 Bluetooth](https://www.amazon.com/HC-05-Bluetooth-Pass-through-Wireless-Communication/dp/B01G9KSAF6/ref=pd_lpo_vtph_147_bs_t_1?_encoding=UTF8&psc=1&refRID=2X3PJXHJQRPCZEE7YR6Q)

[HC-SR04 Ranging Detector Mod Distance Sensor](https://www.sainsmart.com/products/ultrasonic-ranging-detector-mod-hc-sr04-distance-sensor?utm_medium=cpc&utm_source=googlepla&variant=45100776468&gclid=EAIaIQobChMIn5aQxYb01wIVBpNpCh2K0AFJEAQYBCABEgLmC_D_BwE)

[Mini Breadboard](https://www.amazon.com/dp/B0135IQ0ZC/ref=asc_df_B0135IQ0ZC5292224/?tag=hyprod-20&creative=395033&creativeASIN=B0135IQ0ZC&linkCode=df0&hvadid=198091709182&hvpos=1o5&hvnetw=g&hvrand=10222761446287662604&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=2840&hvtargid=pla-407203040794)

[3x AA Battery Holder with Cover](https://www.jameco.com/z/SBH-331-AS-R-3x-AA-Battery-Holder-with-Cover-and-Switch_216144.html?CID=GOOG&gclid=EAIaIQobChMI3bnn9on01wIVCKlpCh1UgAjdEAQYASABEgKAuvD_BwE)

[Starter Kit with Mini breadboard, wires, and LED](https://www.walmart.com/ip/Electronic-Starter-Kit-Resistor-Buzzer-Breadboard-LED-Cable-Electronic-Fans/695297834?wmlspartner=wlpa&selectedSellerId=13978&adid=22222222227113970845&wl0=&wl1=g&wl2=c&wl3=233448459494&wl4=pla-383201117416&wl5=2840&wl6=&wl7=&wl8=&wl9=pla&wl10=117875654&wl11=online&wl12=695297834&wl13=&veh=sem)


Other options include Amazon and ebay

Wiring
-----
#### Block Diagram
![block diagram](https://user-images.githubusercontent.com/7788505/33683567-2a0490e8-da80-11e7-900d-5c7435978980.JPG)

Download or Clone this repository
-----
[Current working version of Repository](https://github.com/rayho564/TRexAlarm/tree/c16be11f1522a9c7db8db721decdd1182b943a30)

If downloading as Zip, extract the folder.


Atmega/Atmel
-----
#### Steps

1. Download and Install Atmel Studios ([Link](http://www.atmel.com/products/microcontrollers/avr/start_now.aspx))
2. Launch Atmel Studios
3. Go to File > New > Project/Solution

![part1](https://user-images.githubusercontent.com/7788505/33684638-cfa889ca-da83-11e7-992e-a2a75464db06.png)

4. Search and Select TRexMicroChip > Slave

![part2](https://user-images.githubusercontent.com/7788505/33684639-d13a3d56-da83-11e7-90e3-72ac4e5c4194.JPG)

5. Connect your ATMEL AVR Programmer (Assuming you have one and used one if not follow this [link](http://www.atmel.com/tools/avrispmkii.aspx))
6. Click Start Debugging or Start Without Debugging

![part3](https://user-images.githubusercontent.com/7788505/33684641-d299a362-da83-11e7-9c64-8615cca4c3d6.JPG)

Android
-----
#### Steps

1. Download and Install Android Studios ([Link](https://developer.android.com/studio/index.html))
2. Launch Android Studios
3. Go to File > Open...
4. Search and Select TRexAndroid

![apart1](https://user-images.githubusercontent.com/7788505/33685476-929143da-da86-11e7-8664-5179c5cc8028.png)

5. Let Gradle Build (This might take a while first time launching)
6. Connect your phone

*If you haven't enabled Developer Mode follow this [link](https://www.androidcentral.com/how-enable-developer-settings-android-42)

7. Press Run

![apart2](https://user-images.githubusercontent.com/7788505/33685479-93f71150-da86-11e7-864f-abd37c1a2641.png)

8. Select your phone

![apart3](https://user-images.githubusercontent.com/7788505/33685481-95438958-da86-11e7-9451-9905398ab950.png)

9. Connect to the HC-06/05 (Default pass is 0000 or 1234)
10. Open the TRexAlarm application on your phone

# Have fun with my project!

Ending product Picture
![FinishedProduct](https://user-images.githubusercontent.com/7788505/33686123-abaefff4-da88-11e7-86af-2232a0d45627.png)







