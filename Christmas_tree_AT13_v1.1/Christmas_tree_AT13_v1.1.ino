/*
* Эффект метеора на ATTINY13
* Copyright (C) 2016 Алексей Шихарбеев
* http://samopal.pro
*/

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "light_ws2812.h"

// Количество светодиодов в ленте 8 - 15
#define NUM_PIXEL 8
#define TM 60

struct cRGB led[NUM_PIXEL];
uint8_t num_pixel = NUM_PIXEL;
uint8_t DX = 2;


int main(void)
{

  while(1)  {
// Цикл перебора пары цветов 0-5   
     for( uint8_t i=0; i<6; i++)Mode1(TM,i);   
  }
} 

/*
 * Эффект метеора
 * @param wite - Таймаут между фазами эффекта, мс
 * @param pair - пара цветов 0-5
 */
void Mode1(uint16_t wait, uint8_t pair){
   uint8_t x = 1;
   if( num_pixel > 8 ) x = 0;
// Метеор летит  
   for( uint8_t i1=0; i1<num_pixel; i1++ ){
// Чистим все    
      for( uint8_t i2=0; i2<num_pixel; i2++ )SetStarColor1(i2, 0, pair);
// Светодиод-ядро метеора      
      SetStarColor1(num_pixel-i1,15, pair);
// Хвост метеора      
      for( uint8_t i2=0; i2<i1; i2++ )SetStarColor1(num_pixel-i2-x, 16-(i1-i2)*DX, pair);
// Отобразить эффект и ждать паузу
      ws2812_setleds(led,num_pixel);    
      delay(wait);
   }
// Метеор пролетел, затухание хвоста   
   for( uint8_t i1=0; i1<num_pixel; i1++ ){
      for( uint8_t i2=0; i2<num_pixel; i2++ ){
         int c = 16-(i1-(i2-8))*2;
         if( c < 0 )c = 0;
         SetStarColor1(num_pixel-i2-1,c, pair);
      }
// Отобразить эффект и ждать паузу
      ws2812_setleds(led,num_pixel);
      delay(wait);
   }
}

/**
 * Разгорание одного пиксела на каждом луче 
 * @param n - Номер пиксела в луче 
 * @param br - Ярклсть
 * @param pair - Пара цветов 0-5
 * 
 */
void SetStarColor1( uint8_t n, uint8_t br, uint8_t pair){
   if( br > 15 )br=15;
   if( br < 8 ){
      switch( pair ){
        case 0 :
        case 3 :
           led[n].r = br*32;
           break;
        case 1 :
        case 4 :
           led[n].g = br*32;
           break;
        case 2 :
        case 5 :
           led[n].b = br*32;
           break;
      }
   }
   else {
      switch( pair ){
         case 0 :
            led[n].r = 255;
            led[n].g = (br-8)*32;
            break;
         case 1 :
            led[n].g = 255;
            led[n].b = (br-8)*32;
            break;
         case 2 :
            led[n].b = 255;
            led[n].r = (br-8)*32;
            break;
         case 3 :
            led[n].r = 255;
            led[n].b = (br-8)*32;
            break;
         case 4 :
            led[n].g = 255;
            led[n].r = (br-8)*32;
            break;
         case 5 :
            led[n].b = 255;
            led[n].g = (br-8)*32;
            break;
      }
   }

}

