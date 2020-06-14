/*
 
 /$$                           /$$$$$$$                             /$$$$$$$$                  /$$            
| $$                          | $$__  $$                           |__  $$__/                 | $$            
| $$        /$$$$$$  /$$   /$$| $$  \ $$  /$$$$$$   /$$$$$$   /$$$$$$$| $$  /$$$$$$   /$$$$$$$| $$$$$$$       
| $$       /$$__  $$|  $$ /$$/| $$  | $$ /$$__  $$ /$$__  $$ /$$_____/| $$ /$$__  $$ /$$_____/| $$__  $$      
| $$      | $$$$$$$$ \  $$$$/ | $$  | $$| $$  \ $$| $$$$$$$$|  $$$$$$ | $$| $$$$$$$$| $$      | $$  \ $$      
| $$      | $$_____/  >$$  $$ | $$  | $$| $$  | $$| $$_____/ \____  $$| $$| $$_____/| $$      | $$  | $$      
| $$$$$$$$|  $$$$$$$ /$$/\  $$| $$$$$$$/|  $$$$$$/|  $$$$$$$ /$$$$$$$/| $$|  $$$$$$$|  $$$$$$$| $$  | $$      
|________/ \_______/|__/  \__/|_______/  \______/  \_______/|_______/ |__/ \_______/ \_______/|__/  |__/      

   Copyright 2020 - Lexis Valentine

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
                                                                                                              
*/                                                                                                             
                                                       
#include <EEPROM.h>               // EEPROM to remember emote state
#include <Adafruit_DotStar.h>     // DotStar library for sake of simplicity
#include <avr/power.h>            // Power management if needed

#define NUMPIXELS  64             // Number of LEDs in strip. it's an 8x8 grid, so, 64.
#define DATAPIN    A4             // Can be changed to any pin you desire
#define CLOCKPIN   A5             // Ditto

Adafruit_DotStar strip(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG); // Set up the extension

int emote = 0;                    // Initialise the emote counter
boolean IsText = 0;               // Is this text or not? This is how to tell
boolean thisState = 0;            // declare thisState and set to zero
char TextHandle[100];             // Increase past 100 if desired. Fills with text from strings below.
int TextHandleSize = 0;           // Count how many elements are in a string for text handling.
boolean brokenloop = 0;           // On button press, break an animation loop

void setup() {
  pinMode(2, INPUT_PULLUP);       // Enable input on Pin D2 and use internal pullup
  emote = EEPROM.read(1);         // EEPROM on boot      
  strip.begin();                  // Initialize pins for output
  strip.clear();                  // Turn all LEDs off to clear garbage data from boot

// TESTING CODE
  int loopcount = 0;              // How many times have we done this?
  int      head  = 0, tail = -64; // Index of first 'on' and 'off' pixels
  uint32_t color = 0x111111;      // Set color for test sequence. DO NOT MAKE IT BRIGHTER THAN THIS!!!!!
  
  while(true) {
    strip.setPixelColor(head, color); // 'On' pixel at head
    strip.setPixelColor(tail, 0);     // 'Off' pixel at tail
    strip.show();                     // Refresh strip
    delay(20);                        // Tiny delay. Make slower if you suspect a bad pixel.
    if(++head >= NUMPIXELS) {         // Increment head index.
      head = 0;                       // Used if you want to make a little snake instead of fill the whole screen
    }
    if(++tail >= NUMPIXELS) tail = 0; // Increment, reset tail index
    loopcount++;                      // Increment loop counter
    if (loopcount > 63) {             // Break the loop!
      delay(1500);
      strip.fill(0x000000, 0, 64);    // Clear the screen!
      strip.show();
      delay(500);
      break;                          // Did it work? Hooray! Break loop as we're done testing.
    }
  }
// TESTING CODE
}

uint32_t color = 0x000000;      // Initialising the color variable.
boolean tested = 0;             // Have we tested the matrix? Displays a little "OK" if not, then sets to true.

// NOTE: Due to some programming weirdness the matricies are MIRRORED HORIZONTALLY from how they appear in the matrix below.

const uint8_t PROGMEM StatusOK[] =   {0, 0, 0, 0, 0, 0, 0, 0,  // All OK? For the testing thing!
                                      0, 0, 1, 0, 1, 1, 1, 1,
                                      1, 0, 1, 0, 1, 0, 0, 1,
                                      0, 1, 1, 0, 1, 0, 0, 1,
                                      0, 0, 1, 0, 1, 0, 0, 1,
                                      0, 1, 1, 0, 1, 0, 0, 1,
                                      1, 0, 1, 0, 1, 1, 1, 1,
                                      0, 0, 0, 0, 0, 0, 0, 0,
                                      };

// Test pattern is above.
uint8_t ARRAY_SIZE = 64;

struct StaticEmote {
  const uint8_t paletteSize;
  const uint32_t* colorMap;
  const uint8_t* pixelData;

  StaticEmote(const uint8_t paletteNum, const uint32_t colors[], const uint8_t pixels[]) :
    paletteSize(paletteNum), colorMap(colors), pixelData(pixels) {
  }

  int getColorAtIndex(uint8_t pixelIndex) {
    if (pixelIndex >= ARRAY_SIZE || pixelData[pixelIndex] >= paletteSize) {
      return 0;
    }
    return colorMap[pixelData[pixelIndex]];
  }

  void drawImage() {
  for(uint8_t i; i< NUMPIXELS; i++){
    strip.setPixelColor(i, getColorAtIndex(i));
  }
  strip.show();
  delay(80);
  }
};



//  CCCCCC  OOOOO   L        OOOOOO   U    U   RRRRR    SSSSSS    !!
//  C       O   O   L        O    O   U    U   R    R   S         !!
//  C       O   O   L        O    O   U    U   RRRR     SSSSSS    !!
//  C       O   O   L        O    O   U    U   R   R         S  
//  CCCCCC  OOOOO   LLLLLLL  OOOOOO   UUUUUU   R    R   SSSSSS    !!


// WARNING!!!! UNDER NO CIRCUMSTANCES WHATSOEVER SHOULD YOU SET ANY COLOR WITH A VALUE GREATER THAN #30
// OF #20 IF MIXING THE PRIMARIES. THE DOTSTAR CAN DRAW MULTIPLE AMPS OF CURRENT AND IF YOU SET THE
// ENTIRE MATRIX TO SHOW #FFFFFF IT -WILL- CAUSE THE BOARD REGULATOR (RATED 500 MILLIAMPS) TO FAIL AND
// THAT'S THE BEST CASE SCENARIO. YOU HAVE BEEN WARNED.

/*

       /$$                    /$$ /$$           /$$       /$$                                                  
      | $$                   | $/| $$          | $$      | $$                                                  
  /$$$$$$$  /$$$$$$  /$$$$$$$|_//$$$$$$        | $$$$$$$ | $$  /$$$$$$  /$$  /$$  /$$       /$$   /$$  /$$$$$$ 
 /$$__  $$ /$$__  $$| $$__  $$ |_  $$_/        | $$__  $$| $$ /$$__  $$| $$ | $$ | $$      | $$  | $$ /$$__  $$
| $$  | $$| $$  \ $$| $$  \ $$   | $$          | $$  \ $$| $$| $$  \ $$| $$ | $$ | $$      | $$  | $$| $$  \ $$
| $$  | $$| $$  | $$| $$  | $$   | $$ /$$      | $$  | $$| $$| $$  | $$| $$ | $$ | $$      | $$  | $$| $$  | $$
|  $$$$$$$|  $$$$$$/| $$  | $$   |  $$$$/      | $$$$$$$/| $$|  $$$$$$/|  $$$$$/$$$$/      |  $$$$$$/| $$$$$$$/
 \_______/ \______/ |__/  |__/    \___/        |_______/ |__/ \______/  \_____/\___/        \______/ | $$____/ 
                                                                                                     | $$      
                                                                                                     | $$      
                                                                                                     |__/      

   /$$     /$$                                                 /$$               /$$           /$$
  | $$    | $$                                                | $$              |__/          | $$
 /$$$$$$  | $$$$$$$   /$$$$$$        /$$$$$$/$$$$   /$$$$$$  /$$$$$$    /$$$$$$  /$$ /$$   /$$| $$
|_  $$_/  | $$__  $$ /$$__  $$      | $$_  $$_  $$ |____  $$|_  $$_/   /$$__  $$| $$|  $$ /$$/| $$
  | $$    | $$  \ $$| $$$$$$$$      | $$ \ $$ \ $$  /$$$$$$$  | $$    | $$  \__/| $$ \  $$$$/ |__/
  | $$ /$$| $$  | $$| $$_____/      | $$ | $$ | $$ /$$__  $$  | $$ /$$| $$      | $$  >$$  $$     
  |  $$$$/| $$  | $$|  $$$$$$$      | $$ | $$ | $$|  $$$$$$$  |  $$$$/| $$      | $$ /$$/\  $$ /$$
   \___/  |__/  |__/ \_______/      |__/ |__/ |__/ \_______/   \___/  |__/      |__/|__/  \__/|__/
                                                                                                  
                                                                                                  


*/
                                           
// ORDER OF COLORS IS GREEN - RED - BLUE
const uint32_t COLOR_RED      = 0x001000;      
const uint32_t COLOR_ORANGE   = 0x091000;      
const uint32_t COLOR_YELLOW   = 0x101000;      
const uint32_t COLOR_GREEN    = 0x100000;      
const uint32_t COLOR_BLUE     = 0x000020;
const uint32_t COLOR_CYAN     = 0x101020;       
const uint32_t COLOR_PURPLE   = 0x000A0A;      
const uint32_t COLOR_GRAY     = 0x101010;      
const uint32_t COLOR_PINK     = 0x090F09;
const uint32_t COLOR_WHITE    = 0x101010;  
const uint32_t COLOR_OFF      = 0x000000; 

// WARNING!!!! UNDER NO CIRCUMSTANCES WHATSOEVER SHOULD YOU SET ANY COLOR WITH A VALUE GREATER THAN #30
// OR #20 IF MIXING PRIMARY COLORS. THE DOTSTAR CAN DRAW MULTIPLE AMPS OF CURRENT AND IF YOU SET THE
// ENTIRE MATRIX TO SHOW #FFFFFF IT -WILL- CAUSE THE BOARD REGULATOR (RATED 500 MILLIAMPS) TO FAIL AND
// THAT'S THE BEST CASE SCENARIO. YOU HAVE BEEN WARNED.      

void initLights(){
  {
for(int i=0; i<sizeof(StatusOK); i) {                     
    byte coltest = (pgm_read_byte(&StatusOK[i]));
    if (coltest == 1) strip.setPixelColor(i, 0x202020);
    else if (coltest == 0) strip.setPixelColor(i, 0);
    i++;
  };
strip.show();
delay(1000);
for(int i=0; i<sizeof(StatusOK); i) {                     
    byte coltest = (pgm_read_byte(&StatusOK[i]));
    if (coltest == 0) strip.setPixelColor(i, 0x202020);
    else if (coltest == 1) strip.setPixelColor(i, 0);
    i++;
  };
strip.show();
delay(1000);
strip.fill(0x000000, 0, 64);
strip.show();
tested = 1;
delay(250);
}
}

void handleButtonPress(){

// EMOTE SWITCH BUTTON - EDIT (emote > n) WHERE n IS NUMBER OF EMOTES.   
      unsigned int buttonHeldTime = millis();
      EEPROM.write(1,emote); 
      for(int i=0; i<ARRAY_SIZE; i) {                     
          strip.setPixelColor(i, 0);
          i++;
      };
      strip.show();
      delay(150);  
      
      while(true){
          if (digitalRead(2) == HIGH) break;
      }
       buttonHeldTime = millis() - buttonHeldTime;
      if(buttonHeldTime < 1000){
        emote++;
        emote = emote % 49;
      }
      else{
        emote = 0;
      }
IsText = 0;
}


void renderImage(){
  switch(emote){
  case 0:{ //The "standard" Pride Flag design
      const uint8_t numColors = 7;
      const uint32_t colorPallete[numColors] = {COLOR_OFF, COLOR_RED, COLOR_ORANGE, COLOR_YELLOW, COLOR_GREEN, COLOR_BLUE, COLOR_PURPLE};
      const uint8_t pixelColorIndexes[64] =  {0, 0, 0, 0, 0, 0, 0, 0,
                                              1, 1, 1, 1, 1, 1, 1, 1,
                                              2, 2, 2, 2, 2, 2, 2, 2,
                                              3, 3, 3, 3, 3, 3, 3, 3,
                                              4, 4, 4, 4, 4, 4, 4, 4,
                                              5, 5, 5, 5, 5, 5, 5, 5,
                                              6, 6, 6, 6, 6, 6, 6, 6,
                                              0, 0, 0, 0, 0, 0, 0, 0,
                                              };
      StaticEmote thisEmote = StaticEmote(numColors, colorPallete, pixelColorIndexes);
      thisEmote.drawImage();
  }
    break;
  }
}




void loop() {
  bool needsUpdateImage = true;
if (tested == 0)          {initLights();};
if(digitalRead(2) == LOW) {handleButtonPress();};  
if(needsUpdateImage)      {renderImage();};
delay(20);
}



  
/*
   strip.show();
   int t = 0;
   while(true){
      delay(10);
      t++;
      if (brokenloop == 1) break;
      if (t == 25) break;           // Alter this to speed up or slow down the text. 10ms intervals.
      if (digitalRead(2) == LOW){
        brokenloop = 1;
        delay(80);
        break;
      }
   }
  t = 0; 
  for(int i=0; i<sizeof(Blank); i) {                     
          strip.setPixelColor(i, 0);
          i++;
  }
  strip.show();     
  while(true){
      delay(10);
      t++;
      if (brokenloop == 1) break;
      if (t == 8) break;           // Alter this to change the blanking interval. 10ms intervals.
      if (digitalRead(2) == LOW){
        brokenloop = 1;
        delay(80);
        break;
      }
   }                
  if (brokenloop == 1){
    IsText = 0;
    brokenloop = 0;
    break;
    }
  y++;
};
}
else;
*/
