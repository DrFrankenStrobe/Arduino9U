/*
 * This code demonstrates how an Arduino setup can be used to control a Whelen 9U728000 Ultra lightbar.
 * 
 * Initially developed and tested on:
 * 
 * Velleman Arduino Uno #VMA100
 * DFROBOT RS485 Shield #DFR0259 
 * Velleman 2.8" TFT    #VMA412
 * 
 * This code was adapted from "Example06-Phonecal" found here: https://www.velleman.eu/downloads/29/vma412_code_and_library_update.zip 
 * 
 * Ultrabar command hex strings were observed from Whelen Engineering's SC Programmer v 5.3.3 from within the "Simulator" window. 
 */
#include <Adafruit_GFX.h>    // Core graphics library
#include <TouchScreen.h>

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Color definitions
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F

/******************* UI details */
#define BUTTON_X 60
#define BUTTON_Y 50
#define BUTTON_W 80
#define BUTTON_H 30
#define BUTTON_SPACING_X 20
#define BUTTON_SPACING_Y 20
#define BUTTON_TEXTSIZE 2

// text box where numbers go
#define TEXT_X 10
#define TEXT_Y 10
#define TEXT_W 220
#define TEXT_H 50
#define TEXT_TSIZE 3
#define TEXT_TCOLOR ILI9341_MAGENTA

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin


#if 1
#define TS_MINX 100
#define TS_MAXX 920
#define TS_MINY 70
#define TS_MAXY 900
#else
#define TS_MINX 70
#define TS_MAXX 900
#define TS_MINY 100
#define TS_MAXY 920
#endif
// We have a status line for like, is FONA working
#define STATUS_X 10
#define STATUS_Y 10



#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Adafruit_TFTLCD tft;
Adafruit_GFX_Button buttons[15];
/* create 15 buttons, in classic candybar phone style */
char buttonlabels[12][5] = {"FRNT", "REAR", "360", "FF", "RF", "TD", "LA", "RA", "LP", "1", "2", "OFF" };
uint16_t buttoncolors[12] = {ILI9341_DARKGREEN, ILI9341_ORANGE, ILI9341_RED, 
                             ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, 
                             ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE, 
                             ILI9341_BLUE, ILI9341_BLUE,  ILI9341_LIGHTGREY };

unsigned char bstate[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

/*
 * basic messages for a Whelen 9U serial-link lightbar
 */
unsigned char cmdstr[] =   {0x64, 0x02, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF };
unsigned char cmdhalof[] = {0x64, 0x03, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF };
unsigned char cmdhalor[] = {0x64, 0x04, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF };
int clock = 0;

void setup(void) {
  Serial.begin(9600);  
  // 240X320
  tft.reset();
  
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  
  for (uint8_t row=0; row<4; row++) {
    for (uint8_t col=0; col<3; col++) {
      buttons[col + row*3].initButton(&tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
                 BUTTON_Y+row*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
                  BUTTON_W, BUTTON_H, ILI9341_WHITE, buttoncolors[col+row*3], ILI9341_WHITE,
                  buttonlabels[col + row*3], BUTTON_TEXTSIZE); 
      buttons[col + row*3].drawButton();
    }
  }  
}
// Print something in the mini status bar with either flashstring
void status(const __FlashStringHelper *msg) {
  tft.fillRect(STATUS_X, STATUS_Y, 240, 8, ILI9341_BLACK);
  tft.setCursor(STATUS_X, STATUS_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.print(msg);
}
// or charstring
void status(const char *msg) {
  tft.fillRect(STATUS_X, STATUS_Y, 240, 8, ILI9341_BLACK);
  tft.setCursor(STATUS_X, STATUS_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.print(msg);
}
#define MINPRESSURE 10
#define MAXPRESSURE 1000

/*
 * calc a Mod256 check sum
 */
unsigned char csum(unsigned char *p, unsigned char len)
{
  unsigned char cc = 0;
  for (unsigned char i = 0; i < len; i++) {
    cc += p[i];
  }
  return cc;
}
/*
 * get a single character if one is available.
 */
int getachar(unsigned char *pf)
{
  int cr = 0;
  for (int i = 0; i < 4; i++) {          
    if (Serial.available()) {                        
      *pf = (unsigned char) Serial.read();            
      cr = 1;      
      break;
    } else {
      if (i > 1) digitalWrite(13,HIGH);            
      delay(1);            
    }          
  }
  return cr;
}
/*
 * exchange a message sequence with the lightbar
 */
void exchange(unsigned char *px, unsigned len)
{
    unsigned char cc = csum(px, len-1);
    px[len-1] = cc;
    Serial.write(px, len);
    Serial.flush();
        
    while(getachar(&cc)) cc = 0;
}
/*
 * Ye Olde Loop
 */
void loop(void) {
  int x, y;
  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);    

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    y = tft.height() - map(p.x, TS_MINY, TS_MAXY, tft.height(), 0);
    x = tft.width() - map(p.y, TS_MINX, TS_MAXX, tft.width(), 0);    
   }
   
  // go thru all the buttons, checking if they were pressed
  for (uint8_t b=0; b<15; b++) {
    if (buttons[b].contains(x, y)) {      
      buttons[b].press(true);  // tell the button it is pressed
    } else {
      buttons[b].press(false);  // tell the button it is NOT pressed
    }
  }

  // now we can ask the buttons if their state has changed
  for (uint8_t b=0; b<15; b++) {
    if (buttons[b].justReleased()) {
      // Serial.print("Released: "); Serial.println(b);
      buttons[b].drawButton();  // draw normal
    }    
    if (buttons[b].justPressed()) {
        buttons[b].drawButton(true);  // draw invert!        
        bstate[b] = !bstate[b];        
        if (b == 0) {
          status("Full Frontal...");          
          cmdstr[5] = 0x10;       // quad flash
          cmdstr[6] = 0x06;       // front lamps
          cmdstr[7] = 0x66;       // more front lamps             
        } else if (b == 1) {
          status("Rear Coverage...");                   
          cmdstr[5] = 0x10;
          cmdstr[6] = 0x09;       // rear lamps
          cmdstr[7] = 0x99;       // more rear lamps
          
        } else if (b == 2) {          
          status("360 deg's...");                   
          cmdstr[5] = 0x10;
          cmdstr[6] = 0x0F;       // both front and back
          cmdstr[7] = 0xFF;       // "

        } else if (b == 3) {                    
          status("Front Flashers...");                    
          cmdhalof[6] = 0x11;     // change number to change pattern, i.e. 0x22. CALI STEADY would be 0xF1 (one lamp will stay on, the other flash) 
          
        } else if (b == 4) {
          status("Rear Flashers...");          
          cmdhalor[6] = 0x11;     // change number to change pattern     
           
        } else if (b == 5) {                    
          status("Takedowns...");
          cmdhalof[6] = 0xFF;     // FF both lamps in the "always on" pattern.

        } else if (b == 6) {          
          status("Left Alley...");          
          cmdhalof[7] = 0xF0;     // just the left "always on"
          
        } else if (b == 7) {
          status("Right Alley...");                    
          cmdhalof[7] = 0x0F;     // just right "always on"
          
        } else if (b == 8) {
          status("Low Power...");
          cmdstr[10] = 0x01;      // low-power
          
        } else if (b == 11) {
          status("OFF...");          
          cmdstr[5] = 0x00;       // set everything back to ZERO
          cmdstr[6] = 0x00;
          cmdstr[7] = 0x00; 
          cmdstr[10] = 0x00;
                             
          cmdhalof[5] = 0x00;
          cmdhalof[6] = 0x00;
          cmdhalof[7] = 0x00;
          
          cmdhalor[5] = 0x00;
          cmdhalor[6] = 0x00;
          cmdhalor[7] = 0x00;
        }
      delay(100);
    }
  }
  delay(1);  
  clock++;
  if (clock == 128) {
    // at roughtly 128 milliseconds, we'll talk to the strobes.
    exchange(cmdstr, sizeof(cmdstr));
    
  } else if (clock == 256) {
    // at 256, we'll talk to the FRONT HALOGENS (this includes alley lamps)
    exchange(cmdhalof, sizeof(cmdhalof));  
    
  } else if (clock == 512) {
    // at 512, we talk to the REAR HALOGENS 
    exchange(cmdhalor, sizeof(cmdhalor));
    // reset the clock
    clock = 0;
  }  
}
