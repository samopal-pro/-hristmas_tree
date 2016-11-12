/*
* Эффект радуги на ATTINY13
* Copyright (C) 2016 Алексей Шихарбеев
* http://samopal.pro
*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "light_ws2812.h"

// Количество светодиодов в ленте 1 - 16
#define NUM_PIXEL 8
#define STEP1 16
#define STEP2 2
uint8_t TM = 10;

struct cRGB led[NUM_PIXEL];
uint8_t num_pixel = NUM_PIXEL;

int main(void)
{
// Переменные для эффкта радуги
  uint8_t color = 0;

  while(1)  {
// Цикл перебора пары цветов 0-5   
   for( int i=0; i< num_pixel; i++ ){
        Wheel(color + i*STEP1, i );
        ws2812_setleds(led,num_pixel);    
        color += STEP2;
        delay(TM);
   }
  }
} 


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
void Wheel(byte WheelPos,uint8_t n) {
  if(WheelPos < 85) {
     led[n].r = WheelPos * 3;
     led[n].g = 255 - WheelPos * 3;
     led[n].b = 0;
  } 
  else if(WheelPos < 170) {
     WheelPos -= 85;
     led[n].r = 255 - WheelPos * 3;
     led[n].g = 0;
     led[n].b = WheelPos * 3;
  } 
  else {
     WheelPos -= 170;
     led[n].r = 0;
     led[n].g = WheelPos * 3;
     led[n].b = 255 - WheelPos * 3;
  }
}

