#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

#define MATRIX_EYES         0
#define MATRIX_MOUTH_LEFT   1
#define MATRIX_MOUTH_MIDDLE 2
#define MATRIX_MOUTH_RIGHT  3
Adafruit_8x8matrix matrix[4] = {
  Adafruit_8x8matrix(), Adafruit_8x8matrix(),
  Adafruit_8x8matrix(), Adafruit_8x8matrix() };

static const uint8_t matrixAddr[] = { 0x70, 0x71, 0x72, 0x73 };

static const uint8_t PROGMEM
  blinkImg[][8] = {   
  { B00111100,       
    B01111110,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B01111110,
    B00111100 },
  { B00000000,
    B01111110,
    B11111111,
    B11111111,
    B11111111,
    B11111111,
    B01111110,
    B00111100 },
  { B00000000,
    B00000000,
    B00111100,
    B11111111,
    B11111111,
    B11111111,
    B00111100,
    B00000000 },
  { B00000000,
    B00000000,
    B00000000,
    B00111100,
    B11111111,
    B01111110,
    B00011000,
    B00000000 },
  { B00000000,        
    B00000000,
    B00000000,
    B00000000,
    B10000001,
    B01111110,
    B00000000,
    B00000000 } },
  mouthImg[][24] = {               
  { B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000,
    B01111111, B11111111, B11111110,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000 },
  { B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000,
    B00111111, B11111111, B11111100,
    B00000111, B00000000, B11100000,
    B00000000, B11111111, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000 },
  { B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000,
    B00111111, B11111111, B11111100,
    B00001000, B00000000, B00010000,
    B00000110, B00000000, B01100000,
    B00000001, B11000011, B10000000,
    B00000000, B00111100, B00000000,
    B00000000, B00000000, B00000000 },
  { B00000000, B00000000, B00000000, 
    B00000000, B00000000, B00000000,
    B00111111, B11111111, B11111100,
    B00100000, B00000000, B00000100,
    B00010000, B00000000, B00001000,
    B00001100, B00000000, B00110000,
    B00000011, B10000001, B11000000,
    B00000000, B01111110, B00000000 },
  { B00000000, B00000000, B00000000, 
    B00000000, B00111100, B00000000,
    B00011111, B11000011, B11111000,
    B00000011, B10000001, B11000000,
    B00000000, B01111110, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000 },
  { B00000000, B00111100, B00000000,
    B00000000, B11000011, B00000000,
    B00001111, B00000000, B11110000,
    B00000001, B00000000, B10000000,
    B00000000, B11000011, B00000000,
    B00000000, B00111100, B00000000,
    B00000000, B00000000, B00000000,
    B00000000, B00000000, B00000000 } };

uint8_t
  blinkIndex[] = { 1, 2, 3, 4, 3, 2, 1 }, 
  blinkCountdown = 100, 
  gazeCountdown  =  75, 
  gazeFrames     =  50, 
  mouthPos       =   0,
  mouthCountdown =  10; 
int8_t
  eyeX = 3, eyeY = 3,  
  newX = 3, newY = 3,   
  dX   = 0, dY   = 0;  

void setup() {
  randomSeed(analogRead(A0));

  for(uint8_t i=0; i<4; i++) {
    matrix[i].begin(matrixAddr[i]);
  }
}

void loop() {
  matrix[MATRIX_EYES].clear();
  matrix[MATRIX_EYES].drawBitmap(0, 0,
    blinkImg[
      (blinkCountdown < sizeof(blinkIndex)) ? 
      blinkIndex[blinkCountdown] :            
      0                                     
    ], 8, 8, LED_ON);
  if(--blinkCountdown == 0) blinkCountdown = random(5, 180);
  if(--gazeCountdown <= gazeFrames) {
    matrix[MATRIX_EYES].fillRect(
      newX - (dX * gazeCountdown / gazeFrames),
      newY - (dY * gazeCountdown / gazeFrames),
      2, 2, LED_OFF);
    if(gazeCountdown == 0) {  
      eyeX = newX; eyeY = newY; 
      do {
        newX = random(7); newY = random(7);
        dX   = newX - 3;  dY   = newY - 3;
      } while((dX * dX + dY * dY) >= 10);      
      dX            = newX - eyeX;             
      dY            = newY - eyeY;             
      gazeFrames    = random(3, 15);          
      gazeCountdown = random(gazeFrames, 120); 
    }
  } else {
    matrix[MATRIX_EYES].fillRect(eyeX, eyeY, 2, 2, LED_OFF);
  }

  drawMouth(mouthImg[mouthPos]);
  if(--mouthCountdown == 0) {
    mouthPos = random(6);
    mouthCountdown = ((mouthPos == 0) && (random(5) == 0)) ?
      random(10, 40) : 
      random(2, 8);   
  }

  for(uint8_t i=0; i<4; i++) matrix[i].writeDisplay();

  delay(20); 
}

void drawMouth(const uint8_t *img) {
  for(uint8_t i=0; i<3; i++) {
    matrix[MATRIX_MOUTH_LEFT + i].clear();
    matrix[MATRIX_MOUTH_LEFT + i].drawBitmap(i * -8, 0, img, 24, 8, LED_ON);
  }
}

