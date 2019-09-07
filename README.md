# Arduino9U
An Arduino controlling a 9U over serial link

## Hardware
* Windows PC (tested on Win7 Home Premium)
* Velleman Arduino Uno #VMA100
* DFROBOT RS485 Shield #DFR0259 
* Velleman 2.8" TFT    #VMA412
* Whelen 9U serial-controlled lightbar (i.e. #9U728000)

## Software (Development Environment)
### Download and install Arduino IDE from https://www.arduino.cc/en/main/software
* Install everything offered (i.e. drivers)
### Verify installation
* Connect the Arduiuno to the USB cable.
* Plug the USB cable into the Windows PC and let it prepare the device and assign it a COM port.
* Run "Arduino" from the menu or desktop
* Type the following in:
```
void setup() {
  pinMode(13, OUTPUT);
}
void loop() {
  delay(800);
  for (int i = 0; i < 4; i++) {
    digitalWrite(13,1);
    delay(10);
    digitalWrite(13,0);
    delay(80);
  }
}
```
* Click on the arrow icon (to compile this scetch, download and run it)
* You should see the Arduino built-in LED flashing "quad-flash".

### Prepare TFT libraries:
* In Arduino, "Tools"->"Manage Libraries"
* Type "GFX" in the filter box.
* You should see "Adafruit GFX by Adafruit". Click "install".
* Type "TouchScreen" in the filter box.
* You should see "Adafruit TouchSceen by Adafruit v 1.0.2". Click "install".
* Type "mcufriend" in the filter box.
* You should see "MCUFRIEND_kbv by David Prentice". Click "Install"
* Close library manager

### Create an Arduino sketch and program the device
* IMPORTANT: make sure the inner switch in the DFROBOT RS485 is set to "MANU" and the outermost switch is set to "off".
* Connect lightbar's blue and gray wire to DFROBOT's "A" and "B" terminal respectively.
* Plug the TFT into the DFROBOT shield, then the DFROBOT into the Arduino.
* copy/paste the Arduino9U.ino file (found at this site) into your sketch
* Compile and run the sketch
* You should see the TFT display nine "function" buttons similar to a phone key-pad layout.
* IMPORTANT: now _change_ the outermost switch on the DFROBOT to "on". This enables the DFROBOT to talk to the lightbar.
* Press "360". All strobes should flash (fingers crossed)
* Remember - you always have to switch the DFROBOT "off" to program it but "on" to talk to the lightbar

